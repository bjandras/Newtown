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

#include "core/block.h"
#include "math/polygon.h"
#include "math/funcs.h"

#include <QDebug>


using namespace core;


Block::Block(QVector<Point> const & base, QVector<Point> const & border, QObject * parent)
	: QObject(parent)
	, m_polygon(border)
	, m_parcel(base)
{
}

math::Polygon Block::polygon() const
{
	return m_polygon;
}

QVector<Block::Parcel> Block::parcels() const
{
	QList<Parcel> result;

	float dif = qAbs(m_parcel.area() - m_polygon.area());
	if (dif <= 0.0001)
	{
		math::Polygon parcel = m_parcel;
		parcel.scale(0.8);
		result << parcel;
	}

	return result.toVector();
}
