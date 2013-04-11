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
#ifndef MATH_RECT_H_
#define MATH_RECT_H_

#include "math/rect.hh"
#include "math/point2f.h"

#include <QRectF>


//! Representation of a 2D rectangle.
struct math::Rect
{
//! \name Object construction.
//@{
	//! Constructs a null rectangle.
	Rect();

	//! Constructs a rectangle.
	Rect(Point2f corner, float width, float height);
//@}

	//! Returns rectangle's width.
	float width() const;
	//! Returns rectangle's height.
	float height() const;

	//! Returns the area enclosed inside the polygon.
	float area() const;

	//! Returns the polygon's centroid.
	Point2f centroid() const;

	//! Tests whether the specified point is inside the polygon.
	bool contains(Point2f const & p) const;

	//! Scales the polygon by the specified factor.
	/*!
	 * Scaling is done around centroid.
	 */
	void scale(float f);

private:
	//! Position of the bottom-left corner.
	Point2f m_corner;
	//! Rectangle width.
	float m_width;
	//! Rectangle height.
	float m_height;
};


#endif // ifndef MATH_RECT_H_
