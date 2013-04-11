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
#ifndef TOOLBOX_H_
#define TOOLBOX_H_

#include <QtGui/QToolBox>

class Ui_toolBox;
class QAbstractButton;

class ToolBox : public QToolBox
{
	Q_OBJECT
	Q_DISABLE_COPY(ToolBox)
public:
	explicit ToolBox(QWidget * parent = NULL, Qt::WindowFlags flags = 0);
	virtual ~ToolBox();

public slots:
	void selectDefaultTool();

signals:
	void toolSelected(QString const & name);
	void mapSelected(QString const & name);
	void mapLoaded(QString const & name, QImage const & image);
	void weightValueChanged(QString const & fieldName, float value);
	void decayValueChanged(QString const & fieldName, float value);
	void viewingCoordsChanged(float radius, float azimuth);
	
protected:
	virtual void changeEvent(QEvent * event);

private:
	Ui_toolBox * m_ui;
	QString m_currentMapName;

private slots:
	virtual void on_mapButtons_buttonClicked(QAbstractButton * button);
	virtual void on_fieldTools_buttonClicked(QAbstractButton * button);
	virtual void on_graphTools_buttonClicked(QAbstractButton * button);

	virtual void on_loadMapButton_clicked();
	virtual void on_saveMapButton_clicked();
	virtual void on_generateMapButton_clicked();

	virtual void weightValueChanged(int value);
	virtual void decayValueChanged(int value);

	virtual void on_viewingRadiusSlider_valueChanged(int value);
	virtual void on_viewingAzimuthSlider_valueChanged(int value);

	virtual void on_distSep_valueChanged(double value);
	virtual void on_koefTest_valueChanged(double value);
	virtual void on_koefSegment_valueChanged(double value);
	virtual void on_koefLookahead_valueChanged(double value);
	virtual void on_distSample_valueChanged(double value);

	virtual void on_distSep_local_valueChanged(double value);
	virtual void on_koefTest_local_valueChanged(double value);
	virtual void on_koefSegment_local_valueChanged(double value);
	virtual void on_koefLookahead_local_valueChanged(double value);
	virtual void on_distSample_local_valueChanged(double value);
};


#endif // TOOLBOX_H_
