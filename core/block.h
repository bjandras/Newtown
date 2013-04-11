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
#ifndef CORE_BLOCK_H
#define CORE_BLOCK_H

#include "core/block.hh"
#include "core/point.hh"
#include "math/polygon.h"

#include <QObject>
#include <QVector>


//! Represents a city block.
/*!
 * A city block is the smallest area of urban planning.
 *
 * Blocks contain zero or more parcels on which buildings are built.
 * The parcels() method returns a list of available parcels.
 */
class core::Block : public QObject
{
	Q_OBJECT;
public:
	//! Type of the parcel objects.
	typedef math::Polygon Parcel;

	//! Constructs the object.
	/*!
	 * \param base the base polygon on which the block is located
	 * \param border polygon enclosing the area of the block
	 * \param parent the parent object
	 */
	Block(QVector<Point> const & base, QVector<Point> const & border, QObject * parent = NULL);

	//! Returns the polygon that limits the block area.
	math::Polygon polygon() const;

	//! Returns parcels available on this block.
	QVector<Parcel> parcels() const;

private:
	//! Polygon encapsulating block's area.
	math::Polygon m_polygon;
	//! Polygon defined by region's vertices.
	math::Polygon m_parcel;
};


#endif // ifndef CORE_BLOCK_H
