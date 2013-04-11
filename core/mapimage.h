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
#ifndef CORE_MAPIMAGE_H_
#define CORE_MAPIMAGE_H_

#include "core/mapimage.hh"
#include "math/vector2f.hh"

#include <QImage>
#include <QPointF>


//! Encapsulation of input map image.
/*!
 * Map images are used for loading natural boundary maps and heightmaps.
 */
class core::MapImage : public QImage
{
public:
//! \name Object construction.
//@{
	//! Constructs a null-image.
	MapImage();

	//! Constructs a map image using data from a file on disk.
	MapImage(const char * path);

	//! Constructs a map image using data from the specified image.
	MapImage(QImage const & image);
//@}

//! \name Coordinate conversion.
//@{
	//! Converts field coordinates to image coordinates.
	QPointF toImageCoords(math::Vector2f const & fieldCoords) const;

	//! Converts image coordinates to field coordinates.
	math::Vector2f toFieldCoords(QPointF const & imageCoords) const;
//@}
};


#endif // ifndef CORE_MAPIMAGE_H_
