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
#ifndef GRAPHITEM_H
#define GRAPHITEM_H

#include <QGraphicsItemGroup>

#include "core/edge.hh"
#include "core/point.h"
#include "core/district.hh"

class Scene;
class QGraphicsPathItem;
class QGraphicsRectItem;
class QGrapicsPolygonItem;


class GraphItem : public QGraphicsItemGroup
{
public:
	GraphItem(Scene * scene);
	GraphItem(QGraphicsItem * parent, Scene * scene);

	Scene * scene() const;

protected:
	void keyReleaseEvent(QKeyEvent * event);

	//! Remove the associated graph item.
	virtual void remove();
};


class EdgeGraphItem : public GraphItem
{
public:
	EdgeGraphItem(Scene * scene, core::Edge * edge);

	//! Returns the associated edge object.
	core::Edge * edge() const;

protected:
	void focusInEvent(QFocusEvent * event);
	void focusOutEvent(QFocusEvent * event);
	void keyReleaseEvent(QKeyEvent * event);

	//! Removes the associated edge from street graph.
	void remove();

private:
	core::Edge * m_edge;
	QGraphicsPathItem * m_pathItem;

	void setPath();
	void setPen(bool focus = false);
};


class SeedGraphItem : public GraphItem
{
public:
	SeedGraphItem(Scene * scene, core::Point const & p);

	core::Point location() const { return m_location; }

protected:
	void focusInEvent(QFocusEvent * event);
	void focusOutEvent(QFocusEvent * event);

	//! Removes the associated seed-point from the seeder.
	void remove();

private:
	core::Point m_location;
	QGraphicsRectItem * m_rectItem;

	void setPen(bool focus = false);
};


class DistrictGraphItem : public GraphItem
{
public:
	DistrictGraphItem(Scene * scene, core::District * district);

	core::District * district() const { return m_district; }

protected:
	void focusInEvent(QFocusEvent * event);
	void focusOutEvent(QFocusEvent * event);

	//! Removes the associated district from the model.
	void remove();

private:
	core::District * m_district;
	QGraphicsPolygonItem * m_polyItem;
};


#endif // GRAPHITEM_H
