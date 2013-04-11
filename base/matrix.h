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
#ifndef BASE_MATRIX_H_
#define BASE_MATRIX_H_

#include "base/matrix.hh"
#include "base/config.h"

#include <cstring>
#include <stdexcept>


//! A two-dimenstional array.
/*!
 * Also known as a general matrix container.  The contained element must
 * support default consturctor and assignment operator.
 *
 * Matrix dimension is specified in the constructor.
 * Function-call operators provide read-write access to individual elements.
 * The fill() method allows one value to be assigned to all elements.
 */
template<typename T>
struct base::Matrix
{
//! \name Object constriction.
//@{
	//! Constructs the object with the specified number of rows and columns.
	/*!
	 * \param rows number of rows
	 * \param cols number of columns
	 */
	Matrix(int rows, int cols)
		: m_rows(rows), m_cols(cols)
		, m_matrix(0)
	{
		alloc();
	}
	
	//! Copy-constructor.
	Matrix(Matrix const & other)
		: m_rows(other.m_rows), m_cols(other.m_cols)
		, m_matrix(0)
	{
		alloc();

		for (int r = 0; r < m_rows; ++r)
		{
			for (int c = 0; c < m_cols; ++c)
			{
				m_matrix[r][c] = other.m_matrix[r][c];
			}
		}
	}
//@}

	~Matrix()
	{
		dealloc();
	}

	//! Assignment operator.
	Matrix & operator=(Matrix const & other)
	{
		dealloc();

		m_rows = other.m_rows;
		m_cols = other.m_cols;

		alloc();

		for (int r = 0; r < m_rows; ++r)
		{
			for (int c = 0; c < m_cols; ++c)
			{
				m_matrix[r][c] = other.m_matrix[r][c];
			}
		}

		return *this;
	}

	//! Returns a number of rows in this matrix.
	int rows() const { return m_rows; }
	//! Returns a number of cols in this matrix.
	int cols() const { return m_cols; }

	//! Element access operator.
	T & operator()(int row, int col)
	{
#if !defined(OMIT_BOUNDARY_CHECKS)
		if (row < 0 || row >= rows()) throw std::out_of_range("row index out of range");
		if (col < 0 || col >= cols()) throw std::out_of_range("column index out of range");
#endif
		return m_matrix[row][col];
	}

	//! Element access operator.
	T const & operator()(int row, int col) const
	{
#if !defined(OMIT_BOUNDARY_CHECKS)
		if (row < 0 || row >= rows()) throw std::out_of_range("row index out of range");
		if (col < 0 || col >= cols()) throw std::out_of_range("column index out of range");
#endif
		return m_matrix[row][col];
	}

	//! Sets all elements to the specified value.
	void fill(T value)
	{
		for (int i = 0; i < m_rows*m_cols; ++i)
		{
			m_matrix[0][i] = value;
		}
	}
	
private:
	//! Row count.
	int m_rows;
	//! Column count.
	int m_cols;
	//! Memory for holding values.
	T ** m_matrix;

	//! Allocates memory for elements.
	void alloc()
	{
		m_matrix = new T*[m_rows];
		m_matrix[0] = new T[m_rows*m_cols];
		for (int row = 1; row < m_rows; ++row)
		{
			m_matrix[row] = m_matrix[0] + row*m_cols;
		}
	}

	//! Frees memory.
	void dealloc()
	{
		delete[] m_matrix[0];
		delete[] m_matrix;
		m_matrix = 0;
	}
};


#endif // ifndef BASE_MATRIX_H_
