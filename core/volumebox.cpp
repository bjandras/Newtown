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

#include "core/volumebox.h"


using namespace core;


VolumeBox::VolumeBox()
	: m_base()
	, m_height(0.0f)
{
}

VolumeBox::VolumeBox(math::Polygon const & base, float height)
	: m_base(base)
	, m_height(height)
{
}


math::Polygon VolumeBox::base() const
{
	return m_base;
}

float VolumeBox::height() const
{
	return m_height;
}


QList<VolumeBox::Quad> VolumeBox::walls() const
{
	QList<Quad> result;

	QVector<math::Point2f> points = m_base.points();
	for (int i = 0, j = 1; i < points.size(); ++i, ++j)
	{
		if (j == points.size())
		{
			j = 0;
		}

		math::Point2f pi = points[i];
		math::Point2f pj = points[j];

		Quad quad(4);
		quad[0] = QVector3D(pi.x(), pi.y(), 0.0f);
		quad[1] = QVector3D(pj.x(), pj.y(), 0.0f);
		quad[2] = QVector3D(pj.x(), pj.y(), height());
		quad[3] = QVector3D(pi.x(), pi.y(), height());

		result << quad;
	}

	return result;
}

VolumeBox::Polygon VolumeBox::roof() const
{
	Polygon result;

	QVector<math::Point2f> points = m_base.points();
	foreach (math::Point2f p, points)
	{
		result << QVector3D(p.x(), p.y(), height());
	}

	math::Point2f p0 = points.first();
	result << QVector3D(p0.x(), p0.y(), height());

	return result;
}
