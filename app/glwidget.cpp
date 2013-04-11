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

#include <app/glwidget.h>

#include <QtOpenGL>

#include <GL/glu.h>


GLWidget::GLWidget(QGLFormat const & format, QWidget * parent)
	: QGLWidget(format, parent)
	, m_texId(0)
{
	setViewingCoords(1.0f, M_PI/4); // reflect in toolbox.ui
}


void GLWidget::setViewingCoords(float radius, float azimuth)
{
	float x = 0.0f;
	float z = radius * cosf(azimuth);
	float y = radius * sinf(azimuth);

	m_lookAt[0] = QVector3D(x, y, z);
	m_lookAt[1] = QVector3D(0.0f, 0.0f, 0.0f);
	m_lookAt[2] = QVector3D(0.0f, 1.0f, 0.0f);
}


void GLWidget::setTexture(QImage const & image)
{
	m_texId = bindTexture(image, GL_TEXTURE_2D);
}

void GLWidget::setBuildings(QVector<core::VolumeBox> const & buildings)
{
	m_buildings = buildings;
}

void GLWidget::animate()
{
	static QVector3D const rotAxis(0.0f, 1.0f, 0.0f);
	static qreal const rotAngle = 1.0f;

	m_quat *= QQuaternion::fromAxisAndAngle(rotAxis, rotAngle);

	updateGL();
}


void GLWidget::initializeGL()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_FLAT);
}

void GLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)width/height, 0.1, 100.0);
}

void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();

	// view setup
	//
	QVector3D eyePos = m_quat.rotatedVector(m_lookAt[0]);
	QVector3D eyeCen = m_quat.rotatedVector(m_lookAt[1]);
	QVector3D eyeUpv = m_quat.rotatedVector(m_lookAt[2]);
	gluLookAt(
		eyePos.x(), eyePos.y(), eyePos.z(),
		eyeCen.x(), eyeCen.y(), eyeCen.z(),
		eyeUpv.x(), eyeUpv.y(), eyeUpv.z());

	// assign texture
	glBindTexture(GL_TEXTURE_2D, m_texId);

	// draw the polygon
	//
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.0f,  0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,  0.0f,  0.5f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.0f, -0.5f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.0f, -0.5f);
	glEnd();

	// draw buildings
	//
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.8f, 0.8f, 0.8f, 0.6f);
	foreach (core::VolumeBox building, m_buildings)
	{
		// draw walls
		//
		glBegin(GL_QUADS);
		foreach (core::VolumeBox::Quad quad, building.walls())
		{
			foreach (QVector3D v, quad)
			{
				glVertex3d(-0.5+v.x(), v.z(), 0.5-v.y());
			}
		}
		glEnd();

		// draw roof
		//
		glBegin(GL_POLYGON);
		foreach (QVector3D v, building.roof())
		{
			glVertex3d(-0.5+v.x(), v.z(), 0.5-v.y());
		}
		glEnd();

	}
	glEnable(GL_TEXTURE_2D);

	glPopMatrix();

	swapBuffers();
}
