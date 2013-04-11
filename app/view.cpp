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
#include "view.h"

#include <QtGui>


View::View(QWidget * parent)
	: QGraphicsView(parent)
{
//	setDragMode(RubberBandDrag);
}

View::~View()
{
}


void View::zoomIn()
{
	scale(1.2, 1.2);
}

void View::zoomOut()
{
	scale(0.8, 0.8);
}

void View::zoomReset()
{
	resetTransform();
}


void View::setImage(QImage const & image)
{
	m_image = image;
	setBackgroundBrush(image);
}

void View::drawBackground(QPainter * painter, QRectF const & rect)
{
	painter->save();
	painter->setBrush(QBrush(Qt::black));
	painter->drawRect(rect);
	painter->restore();
	
	painter->drawImage(sceneRect(), m_image, m_image.rect());
}

