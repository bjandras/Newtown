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

#include "core/border.h"

#include <QImage>
#include <QColor>
#include <QtCore>

using namespace core::border;


//! Returns the label of the specified pixel.
/*!
 * Currently the pixel's hue value is used as label value.
 *
 * \param image image containing the pixel
 * \param p position of the pixel inside the image
 */
inline
Label getLabel(QImage const & image, QPoint const & p)
{
	static Label background = -1;
	
	if (image.rect().contains(p))
	{
		return QColor::fromRgba(image.pixel(p)).hue();
	}
	else
	{
		return background;
	}
}

//! Tests whether the supplied label is the special value for background pixels.
/*!
 * \retval true the label is a backgound label
 * \retval true not a background label
 */
inline
bool isBackground(Label l)
{
	return l < 0;
}

//! Returns a list of neighborhood pixels.
/*!
 * \param image image containing the specified pixel
 * \param p position of the pixel inside the image
 * \param label label of the specified pixel
 * \return list of neighboring pixels
 */
inline
QVector<QPoint> getNeighbours(QImage const & image, QPoint  const & p, Label label)
{
	QVector<QPoint> n;

	for (int x = -1; x < 2; ++x)
	{
		for (int y = -1; y < 2; ++y)
		{
			if (x == 0 && y == 0) continue;

			QPoint q = p + QPoint(x,y);

			if (label == getLabel(image, q))
			{
				n.append(q);
			}
		}
	}
	
	return n;
}

//! Tests whether two pixels are neighbors.
/*!
 * \param p1 position of the first pixel within the image
 * \param p2 position of the second pixel within the image
 * \retval true pixels are neighbors
 * \retval false pixels are not neighbors
 */
inline
bool neighbours(QPoint const & p1, QPoint const & p2)
{
	QPoint d = p1 - p2;

	return (qAbs(d.x()) <= 1 && qAbs(d.y()) <= 1);
}

//! Tests whether the specified pixel is a border pixel.
/*!
 * \param p position of the pixel within the image
 * \param neigbours list of neighborhood pixels
 */
inline
bool isBorderPixel(QPoint const & /*p*/, QVector<QPoint> const & neighbours)
{
	return neighbours.size() < 8;
}

//! Tests whether the specified pixel is an edge pixel.
/*!
 * \param p position of the pixel within the image
 * \param image image containing the pixel
 */
inline
bool isEdgePixel(QPoint const & p, QImage const & image)
{
	return p.x() == 0 || p.x() == image.width()-1 || p.y() == 0 || p.y() == image.height()-1;
}

//! Reverses pixel withing the boundary segment.
void reverse(BoundarySegment & b)
{
	if (b.isEmpty()) return;
	
	BoundarySegment::iterator fwd, bck;
	for (fwd = b.begin(), bck = b.end()-1; fwd < bck; fwd++, bck--)
	{
		qSwap(*fwd, *bck);
	}
}

//! Tries to connect individual segments of the region boundary.
/*!
 * If two segments share end-point pixels they will be replaced by a single segment.
 */
void connectBoundarySegments(Boundary & boundary)
{
	// make sure segments go from left to right
	//
	for (Boundary::iterator i = boundary.begin(); i != boundary.end(); ++i)
	{
		BoundarySegment & b = *i;

		if (b.first().x() > b.last().x())
		{
			reverse(b);
		}
	}

	for (Boundary::iterator i = boundary.begin(); i != boundary.end(); ++i)
	{
		BoundarySegment & b1 = *i;
		if (b1.isEmpty()) continue;

		for (Boundary::iterator j = i+1; j != boundary.end(); ++j)
		{
			BoundarySegment & b2 = *j;
			if (b2.isEmpty()) continue;
			
			if (neighbours(b1.last(), b2.first()))
			{
				b1 += b2;
				b2.clear();
			}
			else if (neighbours(b1.first(), b2.last()))
			{
				b2 += b1;
				b1.clear();
				break;
			}
			else if (neighbours(b1.first(), b2.first()))
			{
				reverse(b1);
				Q_ASSERT(neighbours(b1.last(), b2.first()));
				b1 += b2;
				b2.clear();
			}
			else if (neighbours(b1.last(), b2.last()))
			{
				reverse(b2);
				Q_ASSERT(neighbours(b1.last(), b2.first()));
				b1 += b2;
				b2.clear();
			}
			else if (neighbours(b1.first(), b2.first()) && neighbours(b1.last(), b2.last()))
			{
				reverse(b2);
				b1 += b2;
				b2.clear();
			}
		}
	}

	for (Boundary::iterator i = boundary.begin(); i != boundary.end(); )
	{
		BoundarySegment & b = *i;

		if (b.isEmpty())
		{
			i = boundary.erase(i);
		}
		else
		{
			++i;
		}
	}
}

//! Tries to connect individual segments of all boundaries.
void connectBoundarySegments(Regions & regions)
{
	for (Regions::iterator it = regions.begin(); it != regions.end(); ++it)
	{
		Boundary & boundary = *it;
		
		connectBoundarySegments(boundary);
	}
}

void core::border::findRegions(Regions & regions, QImage const & image)
{
	for (int y = 0; y < image.height(); ++y)
	{
		for (int x = 0; x < image.width(); ++x)
		{
			QPoint p(x,y);
			
			Label label = getLabel(image, p);
			if (isBackground(label)) continue;
			if (isEdgePixel(p, image)) continue;

			QVector<QPoint> neighbours = getNeighbours(image, p, label);
			if (isBorderPixel(p, neighbours))
			{
				Boundary & boundary = regions[label];
				
				bool appended = false;
				foreach (QPoint const & n, neighbours)
				{
					for (Boundary::iterator it = boundary.begin(); it != boundary.end(); ++it)
					{
						BoundarySegment & segment = *it;
						if (n == segment.last())
						{
							segment.append(p);
							appended = true;
							break;
						}
					}
				}
				
				if (!appended)
				{
					// create new boundary segment
					
					BoundarySegment segment;
					segment.append(p);

					boundary.append(segment);
				}
			}
		}

		connectBoundarySegments(regions);
	}
}
