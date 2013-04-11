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
#ifndef CORE_EDGE_H
#define CORE_EDGE_H

#include "core/edge.hh"
#include "core/point.h"

#include <QObject>
#include <QList>


//! Represents an edge in a road network.
/*!
 * Each edge corresponds to a single road segment, both withing
 * the major road network and within local roads.
 */
class core::Edge : public QObject
{
public:
	//! Edge types.
	enum Types
	{
		TypeZero = 0,  //!< Unknown edge type.
		TypeMajorRoad, //!< A segment of the major road network.
		TypeMinorRoad, //!< A segment of the minor road network.
		TypeBoundary,  //!< Domain or obstacle boundary.
		TypeBridge,    //!< A bridge edge.
		NumTypes
	};

	typedef QList<Point> Trace;

	//! Construct the object.
	/*!
	 * \param v1 starting point
	 * \param v2 ending point
	 * \param trace list of points that defint edge's trace line
	 * \param type type of this edge
	 */
	Edge(Point const & v1, Point const & v2, Trace const & trace, Types type);

//! \name Type of the edge.
//@{
	//! Assigns the edge type.
	void setType(Types type);

	//! Returns the edge type.
	Types type() const;

	//! Returns whether this edge is some kind of a road.
	/*!
	 * This is just a short-hand for checking the type against all road types.
	 */
	bool isRoad() const;
//@}

//! \name Geometry information.
//@{
	//! Returns the starting point.
	Point const v1() const;

	//! Returns the end point.
	Point const v2() const;

	//! Returns trace points.
	Trace const trace() const;

	//! Returns trace points reversed.
	Trace const traceReversed() const;
//@}

	//! Joins two edges together.
	/*!
	 * \return pointer to new edge, or NULL if join is not possible
	 */
	static Edge * join(Edge * e1, Edge * e2);

private:
	//! Starting point.
	Point m_v1;
	//! Ending point.
	Point m_v2;
	//! Trace points.
	Trace m_trace;
	//! Edge type.
	Types m_type;
};


#endif // ifndef CORE_EDGE_H
