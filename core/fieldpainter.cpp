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
#include "core/fieldpainter.h"
#include "core/field.h"

#include "math/vector2f.h"
#include "math/tensor.h"
#include "math/funcs.h"

#include <QtCore>
#include <QtOpenGL>

#include <GL/glu.h>

using namespace core;
using math::Vector2f;


int   const NPN    = 256;
int   const NMESH  = 100;
float const DM     = 1.0f/(NMESH-1);
int   const NPAT   = 32;

int   const alpha  = 0.12*255;


enum Lists
{
	List_MeshVx = 1,
	List_MeshVy,
	List_TexN,
};


static
QGLFormat makeFormat()
{
	QGLFormat format = QGLFormat::defaultFormat();
//	format.setDirectRendering(false);
	return format;
}


FieldPainter::FieldPainter(QSize const & size, TensorField * field, QObject * parent)
	: QObject(parent)
	, m_size(size)
	, m_field(field)
	, m_pbx(NULL)
	, m_pby(NULL)
	, m_frame(0) 
	, m_needsRemake(true)
{
	m_pbx = new QGLPixelBuffer(size, makeFormat());
	m_pbx->makeCurrent();
	initializeGL();
	resizeGL(size.width(), size.height());
	m_pbx->doneCurrent();

	m_pby = new QGLPixelBuffer(size, makeFormat());
	m_pby->makeCurrent();
	initializeGL();
	resizeGL(size.width(), size.height());
	m_pby->doneCurrent();
}

FieldPainter::~FieldPainter()
{
	delete m_pbx; m_pbx = NULL;
	delete m_pby; m_pby = NULL;
}


bool FieldPainter::needsRemake() const
{
	return m_needsRemake;
}

void FieldPainter::needsRemake(bool value)
{
	m_needsRemake = value;
}


void FieldPainter::paintImages(QImage & imgx, QImage & imgy)
{
	remakeIfNeeded();
	
	m_pbx->makeCurrent();
	paintGL(true);
	m_pbx->doneCurrent();
	imgx = m_pbx->toImage();

	m_pby->makeCurrent();
	paintGL(false);
	m_pby->doneCurrent();
	imgy = m_pby->toImage();

	m_frame += 1;
}

void FieldPainter::blend(QImage & imgx, QImage const & imgy, QImage const & wx)
{
	// Test whether Porter-Duff compositing is available.
	//
	static int painterSupportsPorterDuffModes = -1;
	if (painterSupportsPorterDuffModes == -1)
	{
		QPainter painter(&imgx);
		painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
		painterSupportsPorterDuffModes = painter.compositionMode() == QPainter::CompositionMode_DestinationIn;
	}

	if (!painterSupportsPorterDuffModes)
	{
		// Porter-Duff not supported -- use more costly method.
		QImage tmp(imgx.size(), QImage::Format_ARGB32);
		blend(tmp, imgx, imgy, wx);
		imgx = tmp;
		return;
	}
	
	QPainter painter(&imgx);
	
	painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	painter.drawImage(0,0, wx);
	
	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.drawImage(0,0, imgy);
	
	painter.end();
}

void FieldPainter::blend(QImage & into, QImage const & imgx, QImage const & imgy, QImage const & wx)
{
	QPainter painter(&into);

	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.drawImage(0,0, wx);
	
	painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
	painter.drawImage(0,0, imgx);

	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.drawImage(0,0, imgy);
	
	painter.end();
}

QImage FieldPainter::blendImage()
{
	remakeIfNeeded();
	return m_blendImage;
}


void FieldPainter::remakeIfNeeded()
{
	if (! needsRemake()) return;
	needsRemake(false);
	
	m_blendImage = makeBlendImage();
	makeMesh();
}


QImage FieldPainter::makeBlendImage()
{
	int width  = m_size.width();
	int height = m_size.height();

	QImage image(m_size, QImage::Format_ARGB32);

	qreal widthf  = (qreal)width;
	qreal heightf = (qreal)height;
	
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			qreal px =           x  / widthf;
			qreal py = (height - y) / heightf;

			float f, r;
			getT(Vector2f(px,py), f,r);

			float wx = math::pow2(cosf(f));
			image.setPixel(x,y, QColor(0,0,0, wx*255).rgba());
		}
	}

	return image;
}


void FieldPainter::initializeGL()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear(GL_COLOR_BUFFER_BIT);
	makePatterns();
}

void FieldPainter::resizeGL(int w, int h)
{
	glViewport(0, 0, (GLint)w, (GLint)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
}

void FieldPainter::paintGL(bool flag)
{
	paintTexture(flag);
}


void FieldPainter::paintTexture(bool flag)
{
	Verticen::iterator tex = m_meshTexCoords.begin();
	Verticen::iterator vrt = flag ? m_meshVertexVx.begin() : m_meshVertexVy.begin();
	
	for (int i = 0; i < NMESH-1; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NMESH; j++)
		{
			glTexCoord2fv(*tex++); glVertex2fv(*vrt++);
			glTexCoord2fv(*tex++); glVertex2fv(*vrt++);
		}
		glEnd();
	}

	glCallList((m_frame % NPAT) + List_TexN);

	glEnable(GL_BLEND);
	glBegin(GL_QUAD_STRIP);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
	glEnd();
	glDisable(GL_BLEND);

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_size.width(), m_size.height(), 0);
}

void FieldPainter::makeMesh()
{
	m_meshTexCoords.clear();
	m_meshVertexVx.clear();
	m_meshVertexVy.clear();
	
	Vector2f p1(0,0);
	Vector2f p2(DM,0);
	
	for (int i = 0; i < NMESH-1; i++)
	{
		p1(1) = p2(1) = 0;

		for (int j = 0; j < NMESH; j++)
		{
			Vector2f vx, vy;

			getV(p1, vx, vy);
			m_meshTexCoords.push_back(p1); 
			m_meshVertexVx.push_back(p1 + vx);
			m_meshVertexVy.push_back(p1 + vy);

			getV(p2, vx, vy);
			m_meshTexCoords.push_back(p2); 
			m_meshVertexVx.push_back(p2 + vx);
			m_meshVertexVy.push_back(p2 + vy);

			p1(1) += DM;
			p2(1) += DM;
		}

		p1(0) += DM;
		p2(0) += DM;
	}
}

void FieldPainter::makePatterns(void) 
{ 
	int lut[256];
	for (int i = 0; i < 256; i++)
		lut[i] = i < 127 ? 0 : 255;

	int phase[NPN][NPN];
	for (int i = 0; i < NPN; i++)
		for (int j = 0; j < NPN; j++)
			phase[i][j] = rand() % 256;

	for (int k = 0; k < NPAT; k++)
	{
		int t = k*256/NPAT;
		GLubyte pat[NPN][NPN][4];

		for (int i = 0; i < NPN; i++)
		{
			for (int j = 0; j < NPN; j++)
			{
				pat[i][j][0] =
				pat[i][j][1] =
				pat[i][j][2] = lut[(t + phase[i][j]) % 255];
				pat[i][j][3] = alpha;
			}
		}
		
		glNewList(k + List_TexN, GL_COMPILE);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, NPN, NPN, 0, GL_RGBA, GL_UNSIGNED_BYTE, pat);
		glEndList();
	}
}


void FieldPainter::getT(math::Vector2f const & p, float & f, float & r)
{
	math::Tensor t = (*m_field)(p);
	f = t.angle();
	r = t.value();
}

void FieldPainter::getV(math::Vector2f const & p, math::Vector2f & vx, math::Vector2f & vy)
{
	float f, r;
	getT(p, f,r);

	Vector2f v  = 0.01 * r * Vector2f(cosf(f), sinf(f));

	vx = v(0) >= 0 ? v : -1*v;
	vy = v(1) >= 0 ? v : -1*v;
}
