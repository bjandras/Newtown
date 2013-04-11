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

#include "core/region.h"
#include "core/tracer.h"
#include "core/seeder.h"
#include "core/grapher.h"
#include "core/field.h"
#include "math/vector2f.h"
#include "math/tensor.h"
#include "math/funcs.h"

#include <QDebug>


using namespace core;
using math::Vector2f;


core::Region::Region(QObject * parent)
	: QObject(parent)
	, m_traceMajorNetwork(true)
	, m_tracer(NULL)
	, m_seeder(NULL)
	, m_grapher(NULL)
	, m_lastTraceMajor(false)
{
}


void core::Region::createTracer()
{
	delete m_tracer;
	m_tracer = new Tracer(m_traceMajorNetwork ? Tracer::RoadTypeMajor : Tracer::RoadTypeLocal, this);
}

void core::Region::createSeeder()
{
	delete m_seeder;
	m_seeder = new Seeder(this);
}

void core::Region::createGrapher()
{
	delete m_grapher;
	m_grapher = new Grapher(this);
}


Tracer & core::Region::tracer() const
{
	if (m_tracer == NULL)
	{
		const_cast<Region*>(this)->createTracer();
	}

	Q_ASSERT(m_tracer != NULL);
	return *m_tracer;
}

Seeder & core::Region::seeder() const
{
	if (m_seeder == NULL)
	{
		const_cast<Region*>(this)->createSeeder();
	}

	Q_ASSERT(m_seeder != NULL);
	return *m_seeder;
}

Grapher & core::Region::grapher() const
{
	if (m_grapher == NULL)
	{
		const_cast<Region*>(this)->createGrapher();
	}

	Q_ASSERT(m_grapher != NULL);
	return *m_grapher;
}


void core::Region::removeEdge(Edge * edge)
{
	Q_ASSERT(edge->parent() == &tracer());
	tracer().removeEdge(edge);

	if (edge->parent() == NULL)
	{
		grapher().disconnect(edge->v1(), edge->v2());
		emit edgeRemoved(edge);
		delete edge;
	}
}

void core::Region::addSeed(Point const & seed)
{
	if (seeder().insert(seed))
	{
		emit seedAdded(seed);
	}
}

void core::Region::removeSeed(Point const & seed)
{
	if (seeder().remove(seed))
	{
		emit seedRemoved(seed);
	}
}


bool core::Region::traceField(TensorField const & field)
{
	Point seed = seeder().pop();

	if (seed.finite())
	{
		emit seedRemoved(seed);

		Region::traceField(field, seed);

		return true;
	}
	else
	{
		// try re-seeding

		foreach (Point p, grapher().dongles() + grapher().bridges())
		{
			if (seeder().insert(p))
			{
				emit seedAdded(p);
			}
		}

		while (! seeder().empty())
		{
			seed = seeder().pop();
			emit seedRemoved(seed);

			if (core::Region::traceField(field, seed) > 0)
			{
				return true;
			}
		}
	}

	return false;
}

int core::Region::traceField(TensorField const & field, Point const & fromPoint)
{
	bool major = !m_lastTraceMajor;
	Vector2f pos = fromPoint.pos();
	Vector2f direction = field(pos).eigenVector(major);

	int numAdded =
			traceField(field, pos,  direction, major) +
			traceField(field, pos, -direction, major);

	if (numAdded == 0)
	{
		numAdded +=
			traceField(field, pos,  direction, !major) +
			traceField(field, pos, -direction, !major);
	}

	m_lastTraceMajor = major;

	return numAdded;
}

int core::Region::traceField(TensorField const & field, Point const & fromPoint, math::Vector2f const & inDirection, bool major)
{
	Tracer::EdgeList edges = tracer().traceField(field, major, fromPoint.pos(), inDirection);

	int numAdded = 0;
	foreach (core::Edge * edge, edges)
	{
		if (edge->parent() != NULL)
		{
			++numAdded;

			grapher().connect(edge->v1(), edge->v2());
			emit edgeAdded(edge);

			if (seeder().insert(edge->v2()))
			{
				emit seedAdded(edge->v2());
			}
		}
		else
		{
			grapher().disconnect(edge->v1(), edge->v2());
			emit edgeRemoved(edge);

			// remove seed-points from deleted vertices
			//
			if (!tracer().containsVertex(edge->v1()))
			{
				if (seeder().remove(edge->v1()))
				{
					emit seedRemoved(edge->v1());
				}
			}
			if (!tracer().containsVertex(edge->v2()))
			{
				if (seeder().remove(edge->v2()))
				{
					emit seedRemoved(edge->v2());
				}
			}

			delete edge;
		}
	}

	return numAdded;
}

void core::Region::traceLineSegment(Point const & fromPoint, Point const & toPoint)
{
	Tracer::EdgeList edges = tracer().traceLineSegment(fromPoint, toPoint);

	foreach (core::Edge * edge, edges)
	{
		if (edge->parent() != NULL)
		{
			grapher().connect(edge->v1(), edge->v2());
			emit edgeAdded(edge);
		}
		else
		{
			grapher().disconnect(edge->v1(), edge->v2());
			emit edgeRemoved(edge);

			delete edge;
		}
	}
}


void core::Region::simplifyGraph()
{
	Tracer::EdgeList edges;
	int numAdded = 0, numRemoved = 0;

	do
	{
		edges = tracer().simplify(grapher().bridges() + grapher().dongles());

		foreach (core::Edge * edge, edges)
		{
			if (edge->parent() != NULL)
			{
				++numAdded;

				grapher().connect(edge->v1(), edge->v2());
				emit edgeAdded(edge);
			}
			else
			{
				++numRemoved;

				grapher().disconnect(edge->v1(), edge->v2());
				emit edgeRemoved(edge);

				delete edge;
			}
		}
	}
	while (! edges.empty());

//	qDebug() << "removed" << numRemoved << "edge(s), added" << numAdded << "edge(s)";
}


void core::Region::findSubregions()
{
	Grapher::CycleList cycles = grapher().cycles();

	foreach (Grapher::Cycle cycle, cycles)
	{
		QList<Edge *> edges;
		QList<bool>   order;
		bool road = false;

		QList<Tracer::Vertex> & verts = cycle; // same thing
		for (QList<Tracer::Vertex>::iterator it = verts.begin(); it != verts.end(); ++it)
		{
			QList<Tracer::Vertex>::iterator jt = it + 1;
			if (jt == verts.end()) jt = verts.begin();

			Tracer::Vertex v1 = *it;
			Tracer::Vertex v2 = *jt;

			Edge * edge = tracer().findEdge(v1, v2, true).value(0);
			if (edge == NULL)
			{
				qCritical() << "the edge for specified vertices is missing!";
				continue;
			}

			edges << edge;
			order << (v1 == edge->v1());
			road  |= edge->isRoad();
		}

		if (road)
		{
			// the region must be surrounded by at least one road segment to be worth building on
			onSubregionFound(edges.toVector(), order.toVector());
		}
	}
}

void core::Region::onSubregionFound(QVector<Edge *> const & edges, QVector<bool> const & order)
{
	QList<Point> border;

	for (int i = 0; i < edges.size(); ++i)
	{
		Edge * edge = edges[i];
		bool orderp = order[i];

		if (orderp)
		{
			border << edge->v1() << edge->trace() << edge->v2();
		}
		else
		{
			border << edge->v2() << edge->traceReversed() << edge->v1();
		}
	}

	onSubregionFound(border.toVector());
}

void core::Region::onSubregionFound(QVector<Point> const & border)
{
	Q_UNUSED(border);
}
