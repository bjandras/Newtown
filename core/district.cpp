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

#include "core/district.h"
#include "core/block.h"
#include "core/grapher.h"
#include "core/seeder.h"
#include "core/tracer.h"
#include "core/point.h"

#include <QVector>
#include <QDebug>


using namespace core;
using math::Vector2f;


District::District(QVector<Point> const & border, QObject * parent)
	: Region(parent)
	, m_polygon(border)
{
	m_traceMajorNetwork = false;
}


bool District::traceStep(TensorField const & field)
{
	if (seeder().empty() && tracer().edgesCount() == 0)
	{
		traceInit();
	}

	bool more = traceField(field);

	if (!more)
	{
		simplifyGraph();
	}

	return more;
}

void District::traceInit()
{
	addSeed(m_polygon.centroid());

	foreach (Edge * edge, tracer().traceBoundary(m_polygon.points()))
	{
		if (edge->parent() != NULL)
		{
			grapher().connect(edge->v1(), edge->v2());
			emit edgeAdded(edge);
		}
	}
}


bool District::contains(Point const & p)
{
	return m_polygon.contains(p);
}


void District::clear()
{
	m_blocks.clear();

	while (! seeder().empty())
	{
		emit seedRemoved(seeder().pop());
	}

	foreach (Edge * edge, tracer().edges())
	{
		removeEdge(edge);
	}
}


void District::onSubregionFound(QVector<Edge *> const & edges, QVector<bool> const & order)
{
	QList<Point> border;
	QList<Point> base;

	for (int i = 0; i < edges.size(); ++i)
	{
		Edge * edge = edges[i];

		if (order[i])
		{
			border << edge->v1() << edge->trace() << edge->v2();
			base << edge->v1();
		}
		else
		{
			border << edge->v2() << edge->traceReversed() << edge->v1();
			base << edge->v2();
		}
	}

	m_blocks.append(new Block(base.toVector(), border.toVector(), this));
}
