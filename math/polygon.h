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
#ifndef MATH_POLYGON_H_
#define MATH_POLYGON_H_

#include "math/polygon.hh"
#include "math/point2f.h"

#include <QVector>
#include <QList>
#include <QPolygonF>


//! Representation of a 2D polygon.
/*!
 * An ordered list of points that defines the polygon is provided to the
 * object constructor.
 */
struct math::Polygon
{
//! \name Object construction.
//@{
	//! Constructs an empty polygon.
	Polygon();

	//! Constructs a polygon from the specified points.
	Polygon(QVector<Point2f> const & points);

	//! Constructs a polygon from the specified points.
	Polygon(QList<Point2f> const & points);
//@}

	//! Returns the number of points that define this polygon.
	int numPoints() const;

	//! Returns the points that make up this polygon.
	QVector<Point2f> points() const;

	//! Returns the area enclosed inside the polygon.
	float area() const;

	//! Returns the polygon's centroid.
	Point2f centroid() const;

	//! Tests whether the specified point is inside the polygon.
	bool contains(Point2f const & p) const;

	//! Scales the polygon by the specified factor.
	void scale(float f);

private:
	//! Vertex points.
	QVector<Point2f> m_points;
	//! Calculated area.
	float m_area;
	//! Calculated centroid.
	Point2f m_centroid;
	//! Related QPolygonF instance.
	QPolygonF m_qpolygon;

	//! Calculates area and centroid.
	/*!
	 * Calculated area and centroid is saved to m_area and m_centroid, respectively.
	 */
	void calcAreaAndCentroid();

	//! Construct QPolygonF instance.
	/*!
	 * Calculated QPolygonF is saved in m_qpolygon.
	 */
	void buildQPolygon();
};


#endif // ifndef MATH_POLYGON_H_
