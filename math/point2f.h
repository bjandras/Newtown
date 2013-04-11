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
#ifndef MATH_POINT2F_H_
#define MATH_POINT2F_H_

#include "math/point2f.hh"
#include "math/vector2f.h"


//! A point in 2D space.
/*!
 * A vector that spans from the the origin to this point
 * is returned by the pos() method.
 */
struct math::Point2f
{
	//! Absolute origin point.
	static Point2f const origin;
	//! A point at infinity.
	static Point2f const infinity;

//! \name Object construction.
//@{
	//! Constructs a point at infinity.
	Point2f();

	//! Constructs a point at the specified position;
	Point2f(float x, float y);

	//! Constructs a point at the specified position;
	Point2f(Vector2f const & pos);

	//! Copy-constructor.
	Point2f(Point2f const & other);
//@}

	//! Assignment operator.
	Point2f & operator=(Point2f const & other);

	//! Equality test.
	bool operator==(Point2f const & other) const;

	//! Returns whether the point is finite, i.e. not at infinity.
	bool finite() const;

	//! Returns the radi-vector.
	Vector2f const pos() const;

	//! Returns the X coordinate.
	float x() const;
	//! Returns the Y coordinate.
	float y() const;

private:
	//! Radi-vector.
	Vector2f m_pos;
};


#endif // ifndef MATH_POINT2F_H_
