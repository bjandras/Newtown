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
#include "math/tensor.h"
#include "math/vector2f.h"
#include "math/funcs.h"

using namespace math;


Tensor Tensor::fromValues(float x, float y)
{
	return Vector2f(x,y);
}

Tensor::Tensor()
	: Vector2f()
{
}

Tensor::Tensor(float value, float angle)
	: Vector2f(value * cos(2.0f*angle),  value * sin(2.0f*angle))
{
}

Tensor::Tensor(const Vector2f & v)
	: Vector2f(v)
{
	// a check whether v is a valid tensor is not performed
}

float Tensor::value() const
{
	return norm();
}

float Tensor::angle() const
{
	return atan2(m_val[1], m_val[0]) / 2.0f;
}

Vector2f Tensor::eigenVector(bool major) const
{
	float r = value();
	float f = angle();

	Vector2f v(r*cos(f), r*sin(f));

	return major ? v : Vector2f(-v(1), v(0));
}

Tensor & Tensor::operator+=(const Tensor & other)
{
	m_val[0] += other(0);
	m_val[1] += other(1);
	return *this;
}


Tensor math::operator+(const Tensor & left, const Tensor & right)
{
	return static_cast<const Vector2f &>(left) + static_cast<const Vector2f &>(right);
}

Tensor math::operator-(const Tensor & left, const Tensor & right)
{
	return static_cast<const Vector2f &>(left) - static_cast<const Vector2f &>(right);
}

Tensor math::operator*(float s, const Tensor & t)
{
	return s * static_cast<const Vector2f &>(t);
}

Tensor math::operator*(const Tensor & t, float s)
{
	return static_cast<const Vector2f &>(t) * s;
}

Tensor math::operator/(const Tensor & t, float s)
{
	return static_cast<const Vector2f &>(t) / s;
}
