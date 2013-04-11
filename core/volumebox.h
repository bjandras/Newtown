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
#ifndef CORE_VOLUMEBOX_H
#define CORE_VOLUMEBOX_H

#include "core/volumebox.hh"
#include "math/polygon.h"

#include <QVector>
#include <QVector3D>


//! Represents a boxed volume area.
/*!
 * Used for defining building outlines.
 */
struct core::VolumeBox
{
	//! Quadrilateral.
	typedef QVector<QVector3D> Quad;
	//! Polygon.
	typedef QVector<QVector3D> Polygon;

//! \name Object construction.
//@{
	//! Constructs a null volume box.
	VolumeBox();

	//! Construct a volume box.
	VolumeBox(math::Polygon const & base, float height);
//@}

	//! Returns the box base.
	math::Polygon base() const;

	//! Returns height of the box.
	float height() const;

	//! Returns a list of quadrilaterals that forms walls of this box.
	QList<Quad> walls() const;

	//! Returns the roof polygon.
	Polygon roof() const;

private:
	//! Base on the plane.
	math::Polygon m_base;
	//! Vertical size.
	float m_height;
};


#endif // ifndef CORE_VOLUMEBOX_H
