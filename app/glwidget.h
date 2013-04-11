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
#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include "core/volumebox.h"

#include <QGLWidget>
#include <QVector3D>
#include <QQuaternion>


//! Widget for displaying OpenGL context.
class GLWidget : public QGLWidget
{
	Q_OBJECT;
public:
	//! Constructs the object.
	/*!
	 * \param format GL format to use
	 * \param parent parent object
	 */
	GLWidget(QGLFormat const & format, QWidget * parent = NULL);

	//! Assigns viewing coordinates.
	/*!
	 * Viewing is specified in polar coordinate system.
	 *
	 * \param radius length of the radi-vector
	 * \param azimuth angular elevation, in radians
	 */
	void setViewingCoords(float radius, float azimuth);

	//! Assigns the city texture.
	void setTexture(QImage const & image);

	//! Assigns list of buildings.
	void setBuildings(QVector<core::VolumeBox> const & buildings);

public slots:
	//! Performs one animation step.
	void animate();

protected:
	//! Called to initialize OpenGL system.
	void initializeGL();

	//! Called when widget has been resized.
	/*!
	 * \param width new widget width
	 * \param height new widget height
	 */
	void resizeGL(int width, int height);

	//! Called when widget needs to be painted.
	void paintGL();

private:
	//! ID of the bound texture.
	GLuint m_texId;
	//! Assigned buildings.
	QVector<core::VolumeBox> m_buildings;
	//! Look-at vectors.
	QVector3D m_lookAt[3];
	//! Animation transformation.
	QQuaternion m_quat;
};


#endif // ifndef GLWIDGET_H_
