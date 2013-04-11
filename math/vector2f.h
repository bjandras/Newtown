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
#ifndef MATH_VECTOR2F_H_
#define MATH_VECTOR2F_H_

#include "math/vector2f.hh"


//! Representation of a second degree vector using single-precision floating point numbers.
struct math::Vector2f
{
	//! Constructs a null-vector.
	Vector2f();
	//! Constructs a vector using specified component values.
	Vector2f(float v0, float v1);

	//! Assignment operator.
	Vector2f const & operator=(Vector2f const & other);

	//! Returns the number of columns in this matrix.
	/*!
	 * Since this is a row-vector, the function always returns 2.
	 */
	int rows() const { return 2; }

	//! Returns the number of rows in this matrix.
	/*!
	 * Since this is a row-vector, the function always returns 1.
	 */
	int cols() const { return 1; }

	//! Element access operator.
	float operator()(int row) const;
	//! Element access operator.
	float & operator()(int row);

	//! Returns the pointer to the internal array of values.
	operator float const * () const;
	//! Returns the pointer to the internal array of values.
	operator float * ();

	//! Returns the square of the vector norm.
	float normSquared() const;
	//! Returns the vector norm.
	float norm() const;

	//! Returns the normalized version of this vector.
	Vector2f const normalized() const;
	//! Normalizes, in-place, this vector.
	void normalize();

	//! Tests equality with another vector.
	bool operator==(Vector2f const & other) const;

protected:	
	//! Values.
	float m_val[2];
};


#endif // ifndef MATH_VECTOR2F_H_
