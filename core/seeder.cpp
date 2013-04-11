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

#include "core/seeder.h"
#include "core/mapimage.h"
#include "core/border.h"
#include "math/vector2f.h"
#include "math/funcs.h"

#include <QtAlgorithms>
#include <QDebug>

#include <cmath>


using namespace core;
using math::Vector2f;


Seeder::Seeder(QObject * parent)
	: QObject(parent)
{
}


bool Seeder::empty() const
{
	return m_seedPoints.empty();
}


bool Seeder::insert(Point const & p)
{
	if (findSeedPoint(p) == m_seedPoints.end())
	{
		SeedPoint sp(p, calculatePriority(p));
		SeedPointList::iterator it = qUpperBound(m_seedPoints.begin(), m_seedPoints.end(), sp);
		m_seedPoints.insert(it, sp);

		return true;
	}

	return false;
}

bool Seeder::remove(Point const & p)
{
	SeedPointList::iterator it = findSeedPoint(p);

	if (it != m_seedPoints.end())
	{
		m_seedPoints.erase(it);

		return true;
	}

	return false;
}

Point Seeder::pop()
{
	if (! m_seedPoints.empty())
	{
		return m_seedPoints.takeLast();
	}
	else
	{
		return Point();
	}
}

Seeder::SeedPointList::iterator Seeder::findSeedPoint(Point const & p)
{
	for (SeedPointList::iterator it = m_seedPoints.begin(); it != m_seedPoints.end(); ++it)
	{
		if (it->pos() == p.pos())
		{
			return it;
		}
	}

	return m_seedPoints.end();
}


void Seeder::setBoundaries(core::MapImage const & mapImage)
{
	border::Regions regions;
	border::findRegions(regions, mapImage);

	foreach (border::Boundary const & boundary, regions)
	{
		foreach (border::BoundarySegment const & segment, boundary)
		{
			QPoint a = segment[0];
			Vector2f af = mapImage.toFieldCoords(a);

			foreach (QPoint b, segment)
			{
				Vector2f bf = mapImage.toFieldCoords(b);

				Vector2f d = bf - af;
				if (d.norm() > 0.01) // TODO: configurable parameter
				{
					m_boundarySegments.append(LineSegment(af, bf));

					a = b;
					af = mapImage.toFieldCoords(a);
				}
			}
		}
	}
}

void Seeder::addSingularity(Point const & p)
{
	m_singularities.append(p);
}

void Seeder::removeSingularity(Point const & p)
{
	m_singularities.removeAll(p);
}

Seeder::Priority Seeder::calculatePriority(Point const & p) const
{
	float distBoundary =  INFINITY;
	for (LineSegmentList::const_iterator it = m_boundarySegments.begin(); it != m_boundarySegments.end(); ++it)
	{
		distBoundary = qMin(distBoundary, it->distance(p));
	}

	float distSingularity = INFINITY;
	for (SingularityList::const_iterator it = m_singularities.begin(); it != m_singularities.end(); ++it)
	{
		distSingularity = qMin(distSingularity, (it->pos() - p.pos()).norm());
	}

	return expf(-distBoundary) + expf(-distSingularity);
}


float Seeder::LineSegment::distance(Point const & p) const
{
	Vector2f p0 = p.pos();
	Vector2f q1 = m_q1.pos();
	Vector2f q2 = m_q2.pos();

	return qMin((q1-p0).norm(), (q2-p0).norm());
}
