/*
 * This file is part of Newtown.
 *
 * Copyright (C) 2013 Borko Jandras <bjandras@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_fieldpainterview.h"
#include "toolbox.h"
#include "glwidget.h"
#include "model.h"
#include "view.h"
#include "scene.h"
#include "util.h"
#include "core/fieldpainter.h"

#include <QtCore>
#include <QtGui>
#include <QTimer>
#include <QProcessEnvironment>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_ui(new Ui_mainWindow)
	, m_ui_fieldPainterView(new Ui_fieldPainterView)
	, m_mdiArea(NULL)
	, m_animationTimer(NULL)
	, m_tracingTimer(NULL)
	, m_previewAnimationTimer(NULL)
	, m_fp(NULL)
	, m_gl(NULL)
	, m_view(NULL)
{
	QSize fieldSize(257, 257);
	QRect sceneRect(QPoint(0,0), fieldSize*3);

	// field object
	//
	m_model = new Model;
	m_model->setObjectName("model");

	// field painter
	m_fp = new core::FieldPainter(fieldSize, m_model, this);

	// OpenGL widget
	m_gl = new GLWidget(QGLFormat(QGL::SampleBuffers), this);
	m_gl->setWindowTitle("3D view");

	// field view
	//
	m_view = new View(this);
	m_view->setScene(new Scene(m_model, this));
	m_view->setSceneRect(sceneRect);
	m_view->scene()->setSceneRect(sceneRect);
	m_view->setWindowTitle("2D view");

	// MDI area
	//
	m_mdiArea = new QMdiArea(this);
	m_mdiArea->setViewMode(QMdiArea::TabbedView);
	m_mdiArea->setDocumentMode(true);
	foreach (QObject * obj, m_mdiArea->children())
	{
		QTabBar * tab = qobject_cast<QTabBar *>(obj);
		if (tab) tab->setEnabled(false);
	}

	// "tools" toolbox
	//
	QDockWidget * toolBoxDock = new QDockWidget("Tools", this);
	toolBoxDock->setObjectName("toolBoxDock");
	ToolBox * toolBox = new ToolBox(toolBoxDock);
	toolBoxDock->setWidget(toolBox);

	// timer for animating field flow
	//
	m_animationTimer = new QTimer(this);
	m_animationTimer->setInterval(20);

	// timer for animating road seeding
	//
	m_tracingTimer = new QTimer(this);
	m_tracingTimer->setInterval(10);

	// timer for animating road seeding
	//
	m_previewAnimationTimer = new QTimer(this);
	m_previewAnimationTimer->setInterval(50);

	// UI setup
	//
	m_ui->setupUi(this);
	m_ui_fieldPainterView->setupUi(new QWidget(this, Qt::Tool));
	setCentralWidget(m_mdiArea);
	addDockWidget(Qt::RightDockWidgetArea, toolBoxDock);
	m_mdiArea->addSubWindow(m_gl);
	m_mdiArea->addSubWindow(m_view);

	// connect actions
	//
	connect(m_ui->actionZoomIn, SIGNAL(triggered()), m_view, SLOT(zoomIn()));
	connect(m_ui->actionZoomOut, SIGNAL(triggered()), m_view, SLOT(zoomOut()));
	connect(m_ui->actionZoomReset, SIGNAL(triggered()), m_view, SLOT(zoomReset()));
	connect(m_animationTimer, SIGNAL(timeout()), this, SLOT(animationUpdate()));
	connect(m_tracingTimer, SIGNAL(timeout()), this, SLOT(tracingUpdate()));
	connect(m_previewAnimationTimer, SIGNAL(timeout()), m_gl, SLOT(animate()));
	connect(m_model, SIGNAL(fieldChanged()), this, SLOT(handleFieldChange()));
	connect(toolBox, SIGNAL(toolSelected(QString)), m_view->scene(), SLOT(selectTool(QString)));

	// default show/hide
	//
	on_actionViewFieldPainter_toggled(m_ui->actionViewFieldPainter->isChecked());
	on_actionViewToolbar_Main_toggled(m_ui->actionViewToolbar_Main->isChecked());
	on_actionViewToolbar_Tools_toggled(m_ui->actionViewToolbar_Tools->isChecked());

	animationUpdate();

	resize(900,900);
}

MainWindow::~MainWindow()
{
	delete m_ui; m_ui = NULL;
	delete m_ui_fieldPainterView; m_ui_fieldPainterView = NULL;
}


void MainWindow::animationUpdate()
{
	QImage imgx, imgy, wx = m_fp->blendImage();
	m_fp->paintImages(imgx, imgy);
	
	m_ui_fieldPainterView->label1->setPixmap(QPixmap::fromImage(imgx));
	m_ui_fieldPainterView->label2->setPixmap(QPixmap::fromImage(imgy));
	m_ui_fieldPainterView->label3->setPixmap(QPixmap::fromImage(wx));

	m_fp->blend(imgx, imgy, wx);

	m_ui_fieldPainterView->label4->setPixmap(QPixmap::fromImage(imgx));

	setFieldImage(imgx);

	// update animation duration
	//
	QVariant numTicks = m_animationTimer->property("numTicks");
	if (numTicks.isValid())
	{
		numTicks = numTicks.toInt() - 1;
		m_animationTimer->setProperty("numTicks", numTicks);

		if (numTicks.toInt() == 0)
		{
			m_animationTimer->stop();
		}
	}
}

void MainWindow::tracingUpdate()
{
	bool more = m_model->traceStep();

	if (!more)
	{
		m_tracingTimer->stop();
	}
}

void MainWindow::handleFieldChange()
{
	if (! m_ui->actionViewFieldEnabled->isChecked())
	{
		// field flow visualization is turned off
		return;
	}

	m_fp->needsRemake(true);

	if (! m_animationTimer->isActive())
	{
		// animation is not user-started, start it with duration
		//
		m_animationTimer->setProperty("numTicks", 10);
		m_animationTimer->start();
	}
}


QImage MainWindow::fieldImage() const
{
	return m_fieldImage;
}

void MainWindow::setFieldImage(QImage const & image)
{
	m_fieldImage = image;
	updateViewImage();
}

QImage MainWindow::boundaryImage() const
{
	return m_boundaryImage;
}

void MainWindow::setBoundaryImage(QImage const & image)
{
	m_model->setBoundaryImage(image);
	m_boundaryImage = image;
	updateViewImage();
}

QImage MainWindow::heightMapImage() const
{
	return m_heightMapImage;
}

void MainWindow::setHeightMapImage(QImage const & image)
{
	m_model->setHeightMapImage(image);
	m_heightMapImage = image;
	updateViewImage();
}

QImage MainWindow::populationMapImage() const
{
	return m_populationMapImage;
}

void MainWindow::setPopulationMapImage(QImage const & image)
{
	m_model->setPopulationMapImage(image);
	m_populationMapImage = image;
	updateViewImage();
}

void MainWindow::updateViewImage()
{
	// select view background image
	//
	QImage backImage;
	if (m_currentMapName == "boundaryMap" || m_currentMapName.isEmpty())
	{
		backImage = m_boundaryImage;
	}
	else if (m_currentMapName == "heightMap")
	{
		backImage = m_heightMapImage;
	}
	else if (m_currentMapName == "populationMap")
	{
		backImage = m_populationMapImage;
	}
	else if (m_currentMapName == "backgroundImage")
	{
		backImage = m_backgroundImage;
	}

	if (m_currentPageName == "pageField")
	{
		QImage image(fieldImage());

		// draw boundary image on top
		//
		if (! boundaryImage().isNull())
		{
			QPainter painter(&image);
			painter.drawImage(image.rect(), boundaryImage());
			painter.end();
		}

		m_view->setImage(image);
	}
	else
	{
		m_view->setImage(backImage);
	}
}


void MainWindow::on_toolBox_currentChanged(int index)
{	
	ToolBox * toolbox = dynamic_cast<ToolBox *>(sender());
	if (toolbox != NULL)
	{
		QWidget * current = toolbox->currentWidget();
		if (current != NULL)
		{
			m_currentPageName = current->objectName();
			updateViewImage();
		}

		toolbox->selectDefaultTool();
	}

	QMdiSubWindow * subWindow = NULL;
	
	switch (index)
	{
	case 0:
	case 1:
	case 2:
		subWindow = m_mdiArea->subWindowList()[1];
		m_previewAnimationTimer->stop();
		break;

	case 3:
		subWindow = m_mdiArea->subWindowList()[0];
		m_gl->setTexture(m_model->renderPreviewTexture(m_backgroundImage));
		m_gl->setBuildings(m_model->buildings());
		m_previewAnimationTimer->start();
		break;
	}

	if (subWindow != NULL)
	{
		m_mdiArea->setActiveSubWindow(subWindow);
	}
}

void MainWindow::on_toolBox_mapSelected(QString const & name)
{
	m_currentMapName = name;
	updateViewImage();
}

void MainWindow::on_toolBox_mapLoaded(QString const & name, QImage const & image)
{
	if (name == "boundaryMap")
	{
		setBoundaryImage(image);
	}
	else if (name == "heightMap")
	{
		setHeightMapImage(image);
	}
	else if (name == "populationMap")
	{
		setPopulationMapImage(image);
	}
	else if (name == "backgroundImage")
	{
		m_backgroundImage = image;
		updateViewImage();
	}
}

void MainWindow::on_toolBox_weightValueChanged(QString const & fieldName, float value)
{
	statusBar()->showMessage(QString("Set %1 weight to %2").arg(fieldName).arg(value), 2000);
	m_model->setWeight(fieldName, value);
}

void MainWindow::on_toolBox_decayValueChanged(QString const & fieldName, float value)
{
	statusBar()->showMessage(QString("Set %1 decay to %2").arg(fieldName).arg(value), 2000);
	m_model->setDecay(fieldName, value);
}

void MainWindow::on_toolBox_viewingCoordsChanged(float radius, float azimuth)
{
	m_gl->setViewingCoords(radius, azimuth);
}

void MainWindow::on_actionViewFieldPainter_toggled(bool checked)
{
	QWidget * fieldPainterView = findChild<QWidget*>("fieldPainterView");
	Q_ASSERT(fieldPainterView);
	
	fieldPainterView->setVisible(checked);
}

void MainWindow::on_actionQuit_triggered()
{
	QApplication::closeAllWindows();
}

void MainWindow::on_actionViewToolbar_Main_toggled(bool checked)
{
	m_ui->mainToolBar->setVisible(checked);
}

void MainWindow::on_actionViewToolbar_Tools_toggled(bool checked)
{
	QDockWidget * toolBoxDock = findChild<QDockWidget*>("toolBoxDock");
	if (toolBoxDock)
	{
		toolBoxDock->setVisible(checked);
	}
}

void MainWindow::on_actionViewFieldEnabled_toggled(bool checked)
{
	m_ui->actionViewFieldContinuous->setEnabled(checked);

	if (!checked)
	{
		m_animationTimer->stop();
	}
}

void MainWindow::on_actionViewFieldContinuous_toggled(bool checked)
{
	if (checked)
	{
		m_animationTimer->start();
		m_animationTimer->setProperty("numTicks", QVariant()); // no duration
	}
	else
	{
		m_animationTimer->stop();
	}
}

void MainWindow::on_actionBuild_triggered()
{
	if (! m_tracingTimer->isActive())
	{
		m_tracingTimer->start();
	}
	else
	{
		m_tracingTimer->stop();
	}
}

void MainWindow::on_actionRebuild_triggered()
{
	m_model->clear();
	m_tracingTimer->start();
}

void MainWindow::on_actionClear_triggered()
{
	m_model->clear();
}

void MainWindow::on_actionBuildComplete_triggered()
{
	m_model->traceComplete();
}

void MainWindow::on_actionSubregions_triggered()
{
	m_model->findSubregions();
}

void MainWindow::on_actionExportStreetMap_triggered()
{
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QString home = env.value("HOME", "/home/edgy");

	QString saveFile = QFileDialog::getSaveFileName(this, "Save street map", home);
	if (! saveFile.isEmpty())
	{
		QImage streets = m_model->renderStreetMap();
		streets.save(saveFile);
	}
}
