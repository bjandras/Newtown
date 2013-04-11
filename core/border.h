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
#ifndef CORE_BORDER_H
#define CORE_BORDER_H

#include <QList>
#include <QMap>
#include <QPoint>

class QImage;


namespace core
{
	//! Border detection algorithm.
	/*!
	 * This module contains the implementation of the "border" algorithm
	 * described in the book "Computer Vision" by Shapiro and Stockman.
	 */
	namespace border
	{
		//! Region label type.
		typedef int Label;
		//! Type of segment of region boundary.
		typedef QList<QPoint> BoundarySegment;
		//! Type of region boundary.
		typedef QList<BoundarySegment> Boundary;
		//! Region boundaries, keyed by label.
		typedef QMap<Label, Boundary> Regions;

		//! Located region boundaries in the specified image.
		/*!
		 * Runs the "border" algorithm on the previously labeled image.
		 *
		 * Currently the hue value of each pixel is used as label value.
		 * To use some other value as label modify the getLabel function in border.cpp.
		 *
		 * \param[out] regions where the found regions will be saved
		 * \param[in] image labeled input image
		 */
		void findRegions(Regions & regions, QImage const & image);
	};
};


#endif // ifndef CORE_BORDER_H
