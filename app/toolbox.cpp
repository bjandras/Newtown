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
#include "toolbox.h"
#include "ui_toolbox.h"
#include "model.h"
#include "core/parameters.h"

#include <QtGui>


ToolBox::ToolBox(QWidget * parent, Qt::WindowFlags flags)
	: QToolBox(parent, flags)
	, m_ui(new Ui_toolBox)
{
	m_ui->setupUi(this);
	m_currentMapName = "boundaryMap";

	m_ui->heightWeight->setValue(DEFAULT_WEIGHT_HEIGHT*100);
	m_ui->boundaryWeight->setValue(DEFAULT_WEIGHT_BOUNDARY*100);
	m_ui->userEditWeight->setValue(DEFAULT_WEIGHT_USEREDIT*100);
	
	//m_ui->heightDecay->setValue(DEFAULT_DECAY_HEIGHT);
	m_ui->boundaryDecay->setValue(DEFAULT_DECAY_BOUNDARY);
	m_ui->userEditDecay->setValue(DEFAULT_DECAY_USEREDIT);
	
	connect(m_ui->heightWeight, SIGNAL(valueChanged(int)), this, SLOT(weightValueChanged(int)));
	connect(m_ui->boundaryWeight, SIGNAL(valueChanged(int)), this, SLOT(weightValueChanged(int)));
	connect(m_ui->userEditWeight, SIGNAL(valueChanged(int)), this, SLOT(weightValueChanged(int)));
	connect(m_ui->heightDecay, SIGNAL(valueChanged(int)), this, SLOT(decayValueChanged(int)));
	connect(m_ui->boundaryDecay, SIGNAL(valueChanged(int)), this, SLOT(decayValueChanged(int)));
	connect(m_ui->userEditDecay, SIGNAL(valueChanged(int)), this, SLOT(decayValueChanged(int)));
}

ToolBox::~ToolBox()
{
	delete m_ui;
}

void ToolBox::changeEvent(QEvent * event)
{
    QToolBox::changeEvent(event);

	switch (event->type())
	{
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;

	default:
		break;
    }
}


void ToolBox::selectDefaultTool()
{
	QObject * current = currentWidget();

	if (current != NULL)
	{
		QToolButton * button = current->findChild<QToolButton *>("pointerTool");

		if (button == NULL)
		{
			button = current->findChild<QToolButton *>(current->objectName() + "_pointerTool");
		}

		if (button != NULL)
		{
			button->click();
		}
	}
}


void ToolBox::on_mapButtons_buttonClicked(QAbstractButton * button)
{
	m_currentMapName = button->objectName();
	emit mapSelected(button->objectName());
}

void ToolBox::on_fieldTools_buttonClicked(QAbstractButton * button)
{
	static QRegExp const reg("^page[a-zA-Z]+_");

	QString name = button->objectName();
	name.remove(reg);

	emit toolSelected(name);
}

void ToolBox::on_graphTools_buttonClicked(QAbstractButton * button)
{
	static QRegExp const reg("^page[a-zA-Z]+_");

	QString name = button->objectName();
	name.remove(reg);

	emit toolSelected(name);
}


void ToolBox::on_loadMapButton_clicked()
{
	QAbstractButton * button = m_ui->mapButtons->checkedButton();

	QString path = QFileDialog::getOpenFileName();
	if (path.isEmpty()) return;
	
	QImage image(path);
	if (! image.isNull())
	{
		button->setIcon(QPixmap::fromImage(image));
		emit mapLoaded(button->objectName(), image);
	}
}

void ToolBox::on_saveMapButton_clicked()
{
	// TODO
}

void ToolBox::on_generateMapButton_clicked()
{
	// TODO
}

void ToolBox::weightValueChanged(int value)
{
	QString name = sender()->objectName();
	name.remove("Weight");
	emit weightValueChanged(name, value/100.0f);
}

void ToolBox::decayValueChanged(int value)
{
	QString name = sender()->objectName();
	name.remove("Decay");
	emit decayValueChanged(name, value);
}


void ToolBox::on_viewingRadiusSlider_valueChanged(int value)
{
	float radius = value / 100.0f;
	float azimuth = m_ui->viewingAzimuthSlider->value() * M_PI / 180.0;

	emit viewingCoordsChanged(radius, azimuth);
}

void ToolBox::on_viewingAzimuthSlider_valueChanged(int value)
{
	float radius = m_ui->viewingRadiusSlider->value() / 100.0f;
	float azimuth = value * M_PI / 180.0;

	emit viewingCoordsChanged(radius, azimuth);
}


void ToolBox::on_distSep_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/major/distSep", value);
}
void ToolBox::on_koefTest_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/major/koefTest", value);
}
void ToolBox::on_koefSegment_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/major/koefSegment", value);
}
void ToolBox::on_koefLookahead_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/major/koefLookahead", value);
}
void ToolBox::on_distSample_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/major/distSample", value);
}

void ToolBox::on_distSep_local_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/local/distSep", value);
}
void ToolBox::on_koefTest_local_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/local/koefTest", value);
}
void ToolBox::on_koefSegment_local_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/local/koefSegment", value);
}
void ToolBox::on_koefLookahead_local_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/local/koefLookahead", value);
}
void ToolBox::on_distSample_local_valueChanged(double value) {
	core::Parameters::instance()->set("tracer/local/distSample", value);
}
