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

#include "math/rect.h"
#include "math/funcs.h"


using namespace math;


Rect::Rect()
	: m_corner()
	, m_width(0.0f), m_height(0.0f)
{
}

Rect::Rect(Point2f corner, float width, float height)
	: m_corner(corner)
	, m_width(width), m_height(height)
{
}


float Rect::width() const
{
	return m_width;
}

float Rect::height() const
{
	return m_height;
}

float Rect::area() const
{
	return width() * height();
}

Point2f Rect::centroid() const
{
	return m_corner.pos() + Vector2f(width()/2, height()/2);
}

bool Rect::contains(Point2f const & p) const
{
	float left = m_corner.x();
	float right = left + width();
	float bottom = m_corner.y();
	float top = bottom + height();

	return p.x() >= left && p.x() <= right && p.y() >= bottom && p.y() <= top;
}


void Rect::scale(float f)
{
	Vector2f c = centroid().pos();
	Vector2f d = Vector2f(width(), height()); // diagonal

	Vector2f bl = m_corner.pos(); // bottom-left
	Vector2f tr = bl + d;         // top-right

	// scale corner points
	//
	bl = (bl - c)*f + c;
	tr = (tr - c)*f + c;

	// assign scaled values
	//
	m_corner = bl;
	d = tr - bl;
	m_width  = d(0);
	m_height = d(1);
}
