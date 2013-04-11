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

#include "base/bitmatrix.h"
#include "base/config.h"

#include <QDebug>

#include <stdexcept>
#include <cstdlib>
#include <cstring>


using namespace base;


BitMatrix::BitMatrix(int rows, int cols)
	: m_array(NULL)
	, m_rows(rows), m_cols(cols)
{
	allocBlocks();
}

BitMatrix::BitMatrix(int rows, int cols, bool fillValue)
	: m_array(NULL)
	, m_rows(rows), m_cols(cols)
{
	allocBlocks();
	fill(fillValue);
}

BitMatrix::BitMatrix(BitMatrix const & other)
	: m_array(NULL)
	, m_rows(other.m_rows), m_cols(other.m_cols)
{
	allocBlocks();
	memcpy(m_array, other.m_array, numBlocks()*sizeof(Block));
}

BitMatrix::~BitMatrix()
{
	delete[] m_array;
	m_array = NULL;
}

BitMatrix & BitMatrix::operator=(BitMatrix const & other)
{
	delete[] m_array;

	m_rows = other.m_rows;
	m_cols = other.m_cols;
	allocBlocks();

	memcpy(m_array, other.m_array, numBlocks()*sizeof(Block));

	return *this;
}

void BitMatrix::allocBlocks()
{
	m_array = new Block[(m_rows*m_cols)/bitsPerBlock + 1];
}


BitMatrix::Reference BitMatrix::operator()(int row, int col)
{
	return Reference(*this, row, col);
}

bool BitMatrix::operator()(int row, int col) const
{
	return get(row, col);
}


BitMatrix::Block & BitMatrix::getBlock(int row, int col, int & shift) const
{
#if !defined(OMIT_BOUNDARY_CHECKS)
	if (row < 0 || row >= rows()) throw std::out_of_range("row index out of range");
	if (col < 0 || col >= cols()) throw std::out_of_range("column index out of range");
#endif
	int bitIndex = row*cols() + col;
	int blockIndex = bitIndex / bitsPerBlock;
	shift = bitIndex % bitsPerBlock;
	return m_array[blockIndex];
}

void BitMatrix::set(int row, int col, bool value)
{
	int shift;
	Block & block = getBlock(row, col, shift);

	Block mask = 0x1 << shift;
	if (value)
		block |= mask;
	else
		block &= ~mask;
}

bool BitMatrix::get(int row, int col) const
{
	int shift;
	Block const & block = getBlock(row, col, shift);

	Block mask = 0x1 << shift;
	return block & mask;
}

void BitMatrix::fill(bool value)
{
	memset(m_array, value?~0u:0u, numBlocks()*sizeof(Block));
}


BitMatrix::Reference::Reference(BitMatrix & matrix, int row, int col)
	: m_matrix(matrix)
	, m_row(row), m_col(col)
{
}

BitMatrix::Reference & BitMatrix::Reference::operator=(bool value)
{
	m_matrix.set(m_row, m_col, value);
	return *this;
}

BitMatrix::Reference & BitMatrix::Reference::operator=(Reference const & value)
{
	m_matrix.set(m_row, m_col, (bool)value);
	return *this;
}

BitMatrix::Reference::operator bool() const
{
	return m_matrix.get(m_row, m_col);
}


QDebug operator<<(QDebug dbg, base::BitMatrix & matrix)
{
	for (int row = 0; row < matrix.rows(); ++row)
	{
		for (int col = 0; col < matrix.cols(); ++col)
		{
			dbg.nospace() << (matrix.get(row, col)?1:0);
		}

		if (row < matrix.rows()-1)
			dbg.nospace() << "\n";
	}

	return dbg.space();
}
