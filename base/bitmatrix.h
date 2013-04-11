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
#ifndef BASE_BITMATRIX_H
#define BASE_BITMATRIX_H

#include "base/bitmatrix.hh"


//! A boolean-valued matrix.
/*!
 * Internally, boolen values are packed as individual bits.
 */
struct base::BitMatrix
{
	//! A reference to the bit-matrix value.
	/*!
	 * Element access operators in BitMatrix construct and return
	 * objects of this class, which act as proxies for elements
	 * inside matrix.
	 */
	struct Reference
	{
		//! Assigns the specified value.
		Reference & operator=(bool value);
		//! Assigns the specified value.
		Reference & operator=(Reference const & value);

		//! Returns the value of the referenced element.
		operator bool () const;

	private:
		//! Associated matrix object.
		BitMatrix & m_matrix;
		//! Row number of the referenced matrix element.
		int m_row;
		//! Column number of the referenced matrix element.
		int m_col;

		//! Constructs the object.
		/*!
		 * \param matrix the matrix object holding the referenced element
		 * \param row row number of the referenced element
		 * \param col column number of the referenced element
		 */
		Reference(BitMatrix & matrix, int row, int col);

		friend struct BitMatrix;
	};

//! \name Object construction.
//@{
	//! Construct the matrix with specified number of rows and columns.
	/*!
	 * Initial values for elements are not specified.
	 */
	BitMatrix(int rows, int cols);

	//! Construct the matrix with specified number of rows and columns.
	/*!
	 * Element values are assigned to the specified values.
	 */
	BitMatrix(int rows, int cols, bool fillValue);

	//! Copy-constructor.
	BitMatrix(BitMatrix const & other);
//@}

	//! Destructor.
	~BitMatrix();

	//! Assignment-operator.
	BitMatrix & operator=(BitMatrix const & other);

	//! Returns the number of rows in this matrix.
	int rows() const { return m_rows; }
	//! Returns the number of columns in this matrix.
	int cols() const { return m_cols; }

//! \name Element access.
//@{
	//! Element access operator.
	Reference operator()(int row, int col);
	//! Element access operator.
	bool      operator()(int row, int col) const;

	//! Sets the value of the specified element.
	void set(int row, int col, bool value);
	//! Returns the value of the specified element.
	bool get(int row, int col) const;

	//! Sets all elements to the specified values.
	void fill(bool value);
//@}

private:
	typedef unsigned int Block;
	static const int bitsPerBlock = sizeof(Block)*8;

	//! Memory for elements.
	Block * m_array;
	//! Number of rows.
	int m_rows;
	//! Number of columns.
	int m_cols;

	//! Returns the number of blocks needed for storage.
	int numBlocks() const { return (m_rows*m_cols)/bitsPerBlock + 1; }

	//! Allocates enough blocks for storage.
	void allocBlocks();

	//! Returns the block/shift pair needed for accessing the specified bit.
	Block & getBlock(int row, int col, int & shift) const;
};


class QDebug;
QDebug operator<<(QDebug dbg, base::BitMatrix & matrix);


#endif // ifndef BITMATRIX_H
