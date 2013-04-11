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

#include "math/point2f.h"
#include "math/vector2f.h"

#include <qnumeric.h>
#include <cmath>


using namespace math;


Point2f const Point2f::origin(0.0f, 0.0f);
Point2f const Point2f::infinity(INFINITY, INFINITY);


Point2f::Point2f()
	: m_pos(infinity.m_pos)
{}

Point2f::Point2f(float x, float y)
	: m_pos(x, y)
{}

Point2f::Point2f(Vector2f const & pos)
	: m_pos(pos)
{}

Point2f::Point2f(Point2f const & other)
	: m_pos(other.m_pos)
{}

Point2f & Point2f::operator=(Point2f const & other)
{
	m_pos = other.m_pos;
	return *this;
}

bool Point2f::operator==(Point2f const & other) const
{
	return this->x() == other.x() && this->y() == other.y();
}

bool Point2f::finite() const
{
	return qIsFinite(x()) && qIsFinite(y());
}

Vector2f const Point2f::pos() const
{
	return m_pos;
}

float Point2f::x() const
{
	return m_pos(0);
}

float Point2f::y() const
{
	return m_pos(1);
}
