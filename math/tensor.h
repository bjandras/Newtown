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
#ifndef MATH_TENSOR_H_
#define MATH_TENSOR_H_

#include "math/tensor.hh"
#include "math/vector2f.h"

#include <list>


//! Representation of a second-order symmetric tensor.
/*!
 * Since the tensor is always of the special form, it is sufficient
 * to store only the first column of the 2x2 matrix.
 */
class math::Tensor : public math::Vector2f
{
public:
	//! Creates a tensor from the specified component values.
	static Tensor fromValues(float x, float y);
	
	//! Constructs a zero tensor.
	Tensor();
	//! Constructs a tensor from supplied value/angle pair.
	Tensor(float value, float angle);

	//! Returns the tensor's value part.
	float value() const;
	//! Returns the tensor's angle part.
	float angle() const;

	//! Returns an eigenvector of this tensor.
	/*!
	 * The returned eigenvector has the vector norm equal to the eigenvalue.
	 */
	Vector2f eigenVector(bool major) const;
	
//! \name Vector algebra.
//@{
	//! In-place addition operator.
	Tensor & operator+=(const Tensor & other);

	// Reimplemented because vector->tensor conversion is private.
	//
	friend Tensor operator+(const Tensor & left, const Tensor & right);
	friend Tensor operator-(const Tensor & left, const Tensor & right);
	friend Tensor operator*(float s, const Tensor & t);
	friend Tensor operator*(const Tensor & t, float s);
	friend Tensor operator/(const Tensor & t, float s);
//@}

private:
	//! Constructs the tensor from a vector that is a valid tensor.
	/*!
	 * \param v a vector in the form scalar*[cos(2f); sin(2f)]
	 */
	Tensor(const Vector2f & v);
};


#endif // ifndef MATH_TENSOR_H_
