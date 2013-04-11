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
#ifndef CORE_DISTRICT_H
#define CORE_DISTRICT_H

#include "core/district.hh"
#include "core/region.h"
#include "core/block.hh"
#include "core/point.hh"
#include "math/polygon.h"

#include <QVector>


//! A city district (kvart).
/*!
 * This class represents an area contained within the major
 * road network.
 *
 * Subregions of districts are represented by the Block class.
 */
class core::District : public core::Region
{
	Q_OBJECT;
public:
	//! Constructs the object.
	/*!
	 * \param border district border
	 * \param parent parent object, or NULL if no parent
	 */
	District(QVector<Point> const & border, QObject * parent = NULL);

	//! Performs one step of tracing.
	/*!
	 * \param field tensor field to trace on
	 */
	bool traceStep(TensorField const & field);

	//! Tests whether this district encloses the specified point.
	/*!
	 * \param p point to test
	 */
	bool contains(Point const & p);

	//!  Removes all edges and trace points from this district.
	void clear();

	//! Returns the polygon that limits the district area.
	math::Polygon polygon() const { return m_polygon; }

	//! Returns the list of urban blocks.
	QList<Block *> blocks() const { return m_blocks; }

protected:
	//! Callback function called by base class when a subregion has been detected.
	/*!
	 * This implementation creates a District object in the enclosed area.
	 */
	void onSubregionFound(QVector<Edge *> const & edges, QVector<bool> const & order);

	//! Adds initial seed point and boundary edge.
	virtual void traceInit();

private:
	//! Polygon encapsulating the district's area.
	math::Polygon m_polygon;
	//! Detected urban blocks.
	QList<Block *> m_blocks;
};


#endif // ifndef CORE_DISTRICT_H
