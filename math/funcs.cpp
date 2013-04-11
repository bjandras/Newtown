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

#include "math/funcs.h"
#include "math/vector2f.h"

#include <cmath>

using namespace math;


float math::sqrt(float a)
{
	return zero(sqrtf(a));
}

float math::zero(float a)
{
	return (fabsf(a) < 0.00001f) ? 0.0f : a;
}

float math::sin(float a)
{
	return zero(sinf(a));
}

float math::cos(float a)
{
	return zero(cosf(a));
}

float math::atan2(float y, float x)
{
	float a = atan2f(y,x);
	if (a < 0) a = 2*M_PI + a;
	return a;
}


float math::rbf(Vector2f const & x, Vector2f const & c, float d)
{
	return zero(expf(-d * (x-c).normSquared()));
}


Vector2f math::operator-(Vector2f const & vect)
{
	return Vector2f(-vect(0), -vect(1));
}

Vector2f math::operator+(Vector2f const & left, Vector2f const & right)
{
	return Vector2f(left(0)+right(0), left(1)+right(1));
}

Vector2f math::operator-(Vector2f const & left, Vector2f const & right)
{
	return Vector2f(left(0)-right(0), left(1)-right(1));
}

Vector2f math::operator/(Vector2f const & vect, float s)
{
	return Vector2f(vect(0)/s, vect(1)/s);
}

Vector2f math::operator*(Vector2f const & vect, float s)
{
	return Vector2f(vect(0)*s, vect(1)*s);
}

Vector2f math::operator*(float s, Vector2f const & vect)
{
	return vect*s;
}

float math::operator*(Vector2f const & left, Vector2f const & right)
{
	return left(0)*right(0) + left(1)*right(1);
}


Vector2f & math::operator+=(Vector2f & vect, Vector2f const & other)
{
	vect(0) += other(0);
	vect(1) += other(1);
	return vect;
}

Vector2f & math::operator-=(Vector2f & vect, Vector2f const & other)
{
	vect(0) -= other(0);
	vect(1) -= other(1);
	return vect;
}

Vector2f & math::operator*=(Vector2f & vect, float s)
{
	vect(0) *= s;
	vect(1) *= s;
	return vect;
}

Vector2f & math::operator/=(Vector2f & vect, float s)
{
	vect(0) /= s;
	vect(1) /= s;
	return vect;
}


Vector2f math::orient(Vector2f const & v, Vector2f const & d)
{
	float f = acosf(v * d);
	return (fabs(f) > M_PI/2) ? -v : v;
}
