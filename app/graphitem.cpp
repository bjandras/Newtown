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

#include "graphitem.h"
#include "app/scene.h"
#include "core/edge.h"
#include "core/point.h"
#include "core/district.h"
#include "math/vector2f.h"
#include "math/funcs.h"

#include <QtGui>


GraphItem::GraphItem(Scene * scene)
	: QGraphicsItemGroup(NULL, scene)
{
}

GraphItem::GraphItem(QGraphicsItem * parent, Scene * scene)
	: QGraphicsItemGroup(parent, scene)
{
}

Scene * GraphItem::scene() const
{
	Scene * s = dynamic_cast<Scene*>(QGraphicsItem::scene());
	Q_ASSERT(s);
	return s;
}

void GraphItem::keyReleaseEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Backspace)
	{
		remove();
		event->accept();
	}
}

void GraphItem::remove()
{
}


/////////////////////////////


EdgeGraphItem::EdgeGraphItem(Scene * scene, core::Edge * edge)
	: GraphItem(scene)
	, m_edge(edge)
	, m_pathItem(NULL)
{
	setFlag(ItemIsFocusable);

	m_pathItem = new QGraphicsPathItem;
	addToGroup(m_pathItem);
	m_pathItem->setFlag(ItemIsFocusable);

	setPen();
	setPath();
	setZValue(2);
}

core::Edge * EdgeGraphItem::edge() const
{
	return m_edge;
}

void EdgeGraphItem::setPath()
{
	QPainterPath path;

	QPointF pos = scene()->toSceneCoords(edge()->v1().pos());
	path.moveTo(pos);

	foreach (core::Point const & sp, edge()->trace())
	{
		pos = scene()->toSceneCoords(sp.pos());
		
		path.lineTo(pos);
		path.moveTo(pos);
	}

	pos = scene()->toSceneCoords(edge()->v2().pos());
	path.lineTo(pos);

	m_pathItem->setPath(path);
}

void EdgeGraphItem::setPen(bool focus)
{
	QPen pen;

	switch (edge()->type())
	{
	case core::Edge::TypeBridge:
		pen.setWidth(4);
		pen.setColor(Qt::gray);
		break;
	case core::Edge::TypeMajorRoad:
		pen.setWidthF(3);
		pen.setColor(Qt::yellow);
		break;
	case core::Edge::TypeMinorRoad:
		pen.setWidthF(1);
		pen.setColor(Qt::darkYellow);
		break;
	case core::Edge::TypeBoundary:
		pen.setWidth(1);
		pen.setColor(Qt::cyan);
		break;
	default:
		break;
	}

	if (focus)
	{
		pen.setColor(Qt::red);
	}

	m_pathItem->setPen(pen);
}

void EdgeGraphItem::focusInEvent(QFocusEvent * event)
{
	setPen(true);
	GraphItem::focusInEvent(event);
}

void EdgeGraphItem::focusOutEvent(QFocusEvent * event)
{
	setPen(false);
	GraphItem::focusOutEvent(event);
}

void EdgeGraphItem::keyReleaseEvent(QKeyEvent * event)
{
	bool shift = event->modifiers() & Qt::ShiftModifier;

	switch (event->key())
	{
	case Qt::Key_B:
		edge()->setType(shift ? core::Edge::TypeBridge : core::Edge::TypeBoundary);
		setPen(hasFocus());
		event->accept();
		break;
	case Qt::Key_M:
		edge()->setType(shift ? core::Edge::TypeMajorRoad : core::Edge::TypeMinorRoad);
		setPen(hasFocus());
		event->accept();
		break;
	default:
		GraphItem::keyReleaseEvent(event);
	}
}

void EdgeGraphItem::remove()
{
	scene()->model().removeEdge(m_edge);
}


/////////////////////////////


SeedGraphItem::SeedGraphItem(Scene * scene, core::Point const & p)
	: GraphItem(scene)
	, m_location(p)
{
	QRectF rect(scene->toSceneCoords(p.pos())-QPointF(5,5), QSizeF(10, 10));
	m_rectItem = new QGraphicsRectItem(rect, this);
	addToGroup(m_rectItem);

	setPen();
	setZValue(3);

	setFlag(ItemIsFocusable);
	m_rectItem->setFlag(ItemIsFocusable);
}

void SeedGraphItem::setPen(bool focus)
{
	QPen pen(Qt::green);

	if (focus)
	{
		pen.setColor(Qt::red);
	}

	m_rectItem->setPen(pen);
}

void SeedGraphItem::focusInEvent(QFocusEvent * event)
{
	setPen(true);
	GraphItem::focusInEvent(event);
}

void SeedGraphItem::focusOutEvent(QFocusEvent * event)
{
	setPen(false);
	GraphItem::focusOutEvent(event);
}

void SeedGraphItem::remove()
{
	scene()->model().removeSeed(location());
}


/////////////////////////////


DistrictGraphItem::DistrictGraphItem(Scene * scene, core::District * district)
	: GraphItem(scene)
	, m_district(district)
{
	setFlag(ItemIsFocusable);

	QVector<math::Point2f> polygon = district->polygon().points();

	QPolygonF qpolygon(polygon.size());
	for (int i = 0; i < polygon.size(); ++i)
	{
		qpolygon[i] = scene->toSceneCoords(polygon[i].pos());
	}

	QColor color = Qt::darkGray;
	color.setAlphaF(0.5);

	m_polyItem = new QGraphicsPolygonItem(qpolygon, this);
	m_polyItem->setBrush(QBrush(color));
	addToGroup(m_polyItem);
}

void DistrictGraphItem::focusInEvent(QFocusEvent * event)
{
	QColor color = Qt::lightGray;
	color.setAlphaF(0.5);

	QBrush brush = m_polyItem->brush();
	brush.setColor(color);
	m_polyItem->setBrush(brush);

	GraphItem::focusInEvent(event);
}

void DistrictGraphItem::focusOutEvent(QFocusEvent * event)
{
	QColor color = Qt::darkGray;
	color.setAlphaF(0.5);

	QBrush brush = m_polyItem->brush();
	brush.setColor(color);
	m_polyItem->setBrush(brush);

	GraphItem::focusOutEvent(event);
}

void DistrictGraphItem::remove()
{
	scene()->model().removeDistrict(district());
}
