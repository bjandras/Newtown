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

#include "math/polygon.h"
#include "math/funcs.h"

#include <qalgorithms.h>


using namespace math;


Polygon::Polygon()
{
	m_area = 0.0f;
}

Polygon::Polygon(QVector<Point2f> const & points)
	: m_points(points)
{
	calcAreaAndCentroid();
	buildQPolygon();
}

Polygon::Polygon(QList<Point2f> const & points)
	: m_points(points.size())
{
	for (int i = 0; i < points.size(); ++i)
	{
		m_points[i] = points[i];
	}

	calcAreaAndCentroid();
	buildQPolygon();
}

void Polygon::calcAreaAndCentroid()
{
	int const n = numPoints();

	// calculate area
	//
	float area = 0.0f;
	for (int i = 0; i < n; ++i)
	{
		int j = (i+1) % n;

		Vector2f vi = m_points[i].pos();
		Vector2f vj = m_points[j].pos();

		area += vi(0)*vj(1);
		area -= vj(0)*vi(1);
	}
	area /= 2.0f;

	if (area < 0)
	{
		// negative area signals that border points are specified clockwise

		area = -area;

		// reverse point order
		//
		for (int i = 0; i < n/2; ++i)
		{
			int j = n-1 - i;

			qSwap(m_points[i], m_points[j]);
		}
	}

	// calculate centroid
	//
	Vector2f centroid(0,0);
	for (int i = 0; i < n; ++i)
	{
		int j = (i+1) % n;

		Vector2f pi = m_points[i].pos();
		Vector2f pj = m_points[j].pos();

		float f = pi(0)*pj(1) - pj(0)*pi(1);

		centroid += (pi + pj) * f;
	}
	centroid /= 6 * area;

	m_area = area;
	m_centroid = centroid;
}

void Polygon::buildQPolygon()
{
	int const n = numPoints();
	m_qpolygon = QPolygonF(n);

	for (int i = 0; i < n; ++i)
	{
		Point2f p = m_points[i];
		m_qpolygon[i] = QPointF(p.x(), p.y());
	}
}


int Polygon::numPoints() const
{
	return m_points.size();
}

QVector<Point2f> Polygon::points() const
{
	return m_points;
}

float Polygon::area() const
{
	return m_area;
}

Point2f Polygon::centroid() const
{
	return m_centroid;
}


bool Polygon::contains(Point2f const & p) const
{
	return m_qpolygon.containsPoint(QPointF(p.x(), p.y()), Qt::OddEvenFill);
}


void Polygon::scale(float f)
{
	int const n = numPoints();
	Vector2f c = centroid().pos();

	for (int i = 0; i < n; ++i)
	{
		Vector2f p = m_points[i].pos();

		p = (p - c)*f + c;

		m_points[i] = p;
	}

	buildQPolygon();
}
