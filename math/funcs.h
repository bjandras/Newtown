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
#ifndef MATH_FUNCS_H_
#define MATH_FUNCS_H_

#include "math/vector2f.hh"


namespace math
{
	//! Returns the square of the supplied value.
	inline
	float pow2(float a)
	{
		return a*a;
	}

	//! Square-root function.
	float sqrt(float a);

	//! Rounds the floating-point to zero if it is small enough.
	float zero(float a);

//! \name Trigonometry
//@{
	//! Sine function.
	float sin(float a);
	//! Cosine function.
	float cos(float a);
	//! Arcus-tanges function.
	float atan2(float y, float x);
//@}

	//! Radial-basis function.
	float rbf(Vector2f const & x, Vector2f const & c, float d);

//! \name Basic vector algebra
//@{
	//! Vector negation.
	Vector2f operator-(Vector2f const & left);
	//! Vector addition.
	Vector2f operator+(Vector2f const & left, Vector2f const & right);
	//! Vector subtraction.
	Vector2f operator-(Vector2f const & left, Vector2f const & right);
	//! Vector division by scalar.
	Vector2f operator/(Vector2f const & vect, float s);
	//! Vector multiplication by scalar.
	Vector2f operator*(Vector2f const & vect, float s);
	//! Vector multiplication by scalar.
	Vector2f operator*(float s, Vector2f const & vect);

	//! Vector inner product (dot-product).
	float    operator*(Vector2f const & left, Vector2f const & right);

	//! In-place vector addition.
	Vector2f & operator+=(math::Vector2f & vect, math::Vector2f const & other);
	//! In-place vector subtraction.
	Vector2f & operator-=(math::Vector2f & vect, math::Vector2f const & other);
	//! In-place vector multiplication by scalar.
	Vector2f & operator*=(math::Vector2f & vect, float s);
	//! In-place vector division by scalar.
	Vector2f & operator/=(math::Vector2f & vect, float s);
//@}

	//! Orients the vector in a general direction.
	Vector2f orient(Vector2f const & v, Vector2f const & d);
};


#endif // ifndef MATH_FUNCS_H_
