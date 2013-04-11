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

#include "core/mapimage.h"
#include "math/vector2f.h"

#include <cmath>

using namespace core;


MapImage::MapImage()
	: QImage()
{
}

MapImage::MapImage(const char * path)
	: QImage(path)
{
}

MapImage::MapImage(QImage const & image)
	: QImage(image)
{
}

QPointF MapImage::toImageCoords(math::Vector2f const & fieldCoords) const
{
	// border clamp
	//
	float fx = fmax(0, fmin(fieldCoords(0), 1));
	float fy = fmax(0, fmin(fieldCoords(1), 1));

	int maxrow = height() - 1;
	int maxcol = width() - 1;
	
	qreal sx = fx * maxcol;
	qreal sy = maxrow - fy * maxrow;

	return QPointF(sx,sy);
}

math::Vector2f MapImage::toFieldCoords(QPointF const & imageCoords) const
{
	int maxrow = height() - 1;
	int maxcol = width() - 1;
	
	qreal fx = imageCoords.x() / maxcol;
	qreal fy = (maxrow - imageCoords.y()) / maxrow;
	
	return math::Vector2f(fx,fy);
}
