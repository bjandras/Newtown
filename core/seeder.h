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
#ifndef CORE_SEEDER_H_
#define CORE_SEEDER_H_

#include "core/seeder.hh"
#include "core/mapimage.hh"
#include "core/point.h"

#include <QObject>
#include <QList>


//! Container for seed-points.
/*!
 * Keeps seed-points order based on their priority.
 *
 * Information needed to calculate priority must be provided by using
 * the setBoundaries() and addSingularity() methods.
 */
class core::Seeder : public QObject
{
	Q_OBJECT;

public:
	//! Construct the object.
	explicit Seeder(QObject * parent = NULL);

	//! Tests whether the container is empty.
	bool empty() const;

	//! Adds the specified seed-point.
	/*!
	 * \return indication whether the seed-point has been added
	 */
	bool insert(Point const & p);

	//! Removes the specified seed-point.
	/*!
	 * \return indication whether the seed-point has been removed
	 */
	bool remove(Point const & p);

	//! Removes and returns the seed-point with the highest priority.
	/*!
	 * \return a seed point if there is one, or a point at infinity if the seeder is empty
	 */
	Point pop();

//! \name Information that affects priority.
//@{
	//! Assigns the natural-boundary map.
	void setBoundaries(core::MapImage const & mapImage);

	//! Adds a point to the list of singularities.
	void addSingularity(Point const & p);
	//! Removes a point from the list of singularities.
	void removeSingularity(Point const & p);
//@}

private:
	//! Priority value type.
	typedef float Priority;

	//! Internal seed-point type.
	struct SeedPoint : public Point
	{
		//! Constructs the object with necessary information.
		SeedPoint(Point const & p, Priority priority)
			: Point(p), m_priority(priority)
		{}

		//! Returns the priority value calculated for this point.
		Priority priority() const { return m_priority; }

		//! Less-than comparison operator.
		/*!
		 * Used for keeping an ordered list of seed-points.
		 */
		bool operator<(SeedPoint const & other) const
		{
			return this->priority() < other.priority();
		}

	private:
		//! Calculated priority value.
		Priority m_priority;
	};

	//! Seed-point list type.
	typedef QList<SeedPoint> SeedPointList;

	//! Segment of a line in 2D.
	struct LineSegment
	{
		//! Constructs the line.
		/*!
		 * \param q1 first end-point
		 * \param q2 second end-point
		 */
		LineSegment(Point const & q1, Point const & q2)
			: m_q1(q1), m_q2(q2)
		{}

		//! Calculates the distance of the specified point from the line segment.
		float distance(Point const & p) const;

	private:
		Point m_q1;
		Point m_q2;
	};

	//! List of line segments.
	typedef QList<LineSegment> LineSegmentList;
	//! List of singularities.
	typedef QList<Point> SingularityList;

	//! Ordered list of seed-points.
	/*!
	 * Note: order must be handled manually.
	 */
	SeedPointList m_seedPoints;
	//! List of boundary segments.
	LineSegmentList m_boundarySegments;
	//! List of singular (degenerate) points.
	SingularityList m_singularities;

	//! Finds the seed-point at the specified position.
	/*!
	 * Avoids potentially expensive priority calculation.
	 */
	SeedPointList::iterator findSeedPoint(Point const & p);

	//! Calculates priority for the seed-point at the specified position.
	Priority calculatePriority(Point const & p) const;
};


#endif // ifndef CORE_SEEDER_H_
