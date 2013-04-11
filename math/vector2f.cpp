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

#include "math/vector2f.h"
#include "math/funcs.h"
#include "base/config.h"

#include <stdexcept>


using namespace math;


Vector2f::Vector2f()
{
	m_val[0] = 0;
	m_val[1] = 0;
}

Vector2f::Vector2f(float v0, float v1)
{
	m_val[0] = v0;
	m_val[1] = v1;
}

Vector2f const & Vector2f::operator=(Vector2f const & other)
{
	m_val[0] = other(0);
	m_val[1] = other(1);
	return *this;
}


float Vector2f::operator()(int row) const
{
#if !defined(OMIT_BOUNDARY_CHECKS)
	if (row < 0 || row >= rows()) throw std::out_of_range("row index out of range");
#endif
	return m_val[row];
}

float & Vector2f::operator()(int row)
{
#if !defined(OMIT_BOUNDARY_CHECKS)
	if (row < 0 || row >= rows()) throw std::out_of_range("row index out of range");
#endif
	return m_val[row];
}

Vector2f::operator float const * () const
{
	return m_val;
}

Vector2f::operator float * ()
{
	return m_val;
}


float Vector2f::normSquared() const
{
	return pow2((*this)(0)) + pow2((*this)(1));
}

float Vector2f::norm() const
{
	return zero(sqrt(normSquared()));
}

Vector2f const Vector2f::normalized() const
{
	float n = norm();

	if (n > 0)
	{
		return *this / n;
	}
	else
	{
		return *this;
	}
}

void Vector2f::normalize()
{
	float n = norm();

	if (n > 0)
	{
		m_val[0] /= n;
		m_val[1] /= n;
	}
}


bool Vector2f::operator==(Vector2f const & other) const
{
	return m_val[0] == other.m_val[0] && m_val[1] == other.m_val[1];
}
