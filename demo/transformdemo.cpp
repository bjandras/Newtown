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

#include "transformdemo.h"
#include "math/vector2f.h"
#include "math/funcs.h"

#include <QtGui>

#include <cmath>


using math::Vector2f;
using math::cos;
using math::sin;


extern "C" int transformdemo_main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	QMainWindow win;

	TransformDemo * demo = new TransformDemo(&win);
	win.setCentralWidget(demo);

	win.setWindowTitle("Transform [Demo]");
	win.show();
	win.resize(600, 600);

	return app.exec();
}


TransformDemo::TransformDemo(QWidget * parent)
	: QWidget(parent)
{
	m_f = M_PI/3;
	m_r = 0.5f;
}


void TransformDemo::paintEvent(QPaintEvent * /*event*/)
{
	QPainter painter(this);

	painter.setWindow(-1,-1, 2,2);
	painter.scale(1.0, -1.0);

	QPointF origin(0.0, 0.0);
	painter.drawLine(origin, QPointF(1.0, 0.0));
	painter.drawLine(origin, QPointF(0.0, 1.0));
	painter.drawEllipse(origin, 1.0, 1.0);

	painter.setPen(Qt::red);
	painter.drawLine(origin, QPointF(m_r*cos(m_f), m_r*sin(m_f)));
	painter.setPen(Qt::green);
	painter.drawLine(origin, QPointF(m_p(0), m_p(1)));
	painter.setPen(Qt::blue);
	painter.drawLine(origin, QPointF(m_t(0), m_t(1)));

	painter.end();
}

void TransformDemo::mouseMoveEvent(QMouseEvent * event)
{
	qreal x = event->posF().x() / width();
	qreal y = 1 - event->posF().y() / height();

	// scale
	x *= 2; y *= 2;
	// translate
	x -= 1; y -= 1;

	float cf = m_r * cos(2*m_f);
	float sf = m_r * sin(2*m_f);

	m_p = Vector2f(x,y);
	m_t = Vector2f(x*cf + y*sf, x*sf - y*cf);

	event->accept();
	update();
}
