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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core/fieldpainter.hh"

#include <QtGui/QMainWindow>

class Ui_mainWindow;
class Ui_fieldPainterView;
class QMdiArea;
class QLabel;
class QTimer;
class GLWidget;
class Model;
class View;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void handleFieldChange();

private:
	Ui_mainWindow * m_ui;
	Ui_fieldPainterView * m_ui_fieldPainterView;
	QMdiArea * m_mdiArea;
	QTimer * m_animationTimer;
	QTimer * m_tracingTimer;
	QTimer * m_previewAnimationTimer;
	Model * m_model;
	core::FieldPainter * m_fp;
	GLWidget * m_gl;
	View * m_view;

	QString m_currentPageName;
	QString m_currentMapName;

	QImage m_fieldImage;
	QImage m_boundaryImage;
	QImage m_heightMapImage;
	QImage m_populationMapImage;
	QImage m_backgroundImage;

	QImage fieldImage() const;
	void setFieldImage(QImage const & image);
	QImage boundaryImage() const;
	void setBoundaryImage(QImage const & image);
	QImage heightMapImage() const;
	void setHeightMapImage(QImage const & image);
	QImage populationMapImage() const;
	void setPopulationMapImage(QImage const & image);

	void updateViewImage();
						
private slots:
	void animationUpdate();
	void tracingUpdate();
	void on_toolBox_currentChanged(int index);
	void on_toolBox_mapSelected(QString const & name);
	void on_toolBox_mapLoaded(QString const & name, QImage const & image);
	void on_toolBox_weightValueChanged(QString const & fieldName, float value);
	void on_toolBox_decayValueChanged(QString const & fieldName, float value);
	void on_toolBox_viewingCoordsChanged(float radius, float azimuth);
	void on_actionQuit_triggered();
	void on_actionViewFieldPainter_toggled(bool checked);
	void on_actionViewToolbar_Main_toggled(bool checked);
	void on_actionViewToolbar_Tools_toggled(bool checked);
	void on_actionViewFieldContinuous_toggled(bool checked);
	void on_actionViewFieldEnabled_toggled(bool checked);
	void on_actionBuild_triggered();
	void on_actionRebuild_triggered();
	void on_actionClear_triggered();
	void on_actionBuildComplete_triggered();
	void on_actionSubregions_triggered();
	void on_actionExportStreetMap_triggered();
};


#endif // ifndef MAINWINDOW_H
