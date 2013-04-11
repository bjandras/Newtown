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

#include "core/tracer.h"
#include "core/edge.h"

#include <QDebug>


// number of cells in the vertex grid
#define VERTEX_GRID_DIM 20
// number of cells in the sample-point grid
#define SAMPLEPOINT_GRID_DIM 20


using namespace core;


Tracer::Tracer(RoadType type, QObject * parent)
	: QObject(parent)
	, m_roadType(type)
	, m_vertices(VERTEX_GRID_DIM, VERTEX_GRID_DIM)
	, m_samplePoints(SAMPLEPOINT_GRID_DIM, SAMPLEPOINT_GRID_DIM)
{
	connect(Parameters::instance(), SIGNAL(valueChanged(QString,QVariant)),
		this,          SLOT(onParameterValueChanged(QString,QVariant)));

	loadParameters();
}


void Tracer::setPopulationMapImage(MapImage const & image)
{
	m_populationMapImage = image;
}


Tracer::EdgeList Tracer::simplify(VertexList const & verts)
{
	EdgeList removed;
	EdgeList added;

	foreach (Vertex v, verts)
	{
		EdgeList edges = findEdge(v);

		if (edges.size() == 1)
		{
			Edge * edge = edges[0];

			if (edge->isRoad()) // only simplify road segments
			{
				removeEdge(edge);
				removed << edges;
			}
		}
		else if (edges.size() == 2)
		{
			Edge * newEdge = Edge::join(edges[0], edges[1]);
			Q_ASSERT(newEdge);

			if (! containsEdge(newEdge->v1(), newEdge->v2()))
			{
				removeEdge(edges[0]);
				removeEdge(edges[1]);
				addEdge(newEdge);

				removed << edges;
				added << newEdge;

				break;
			}
			else
			{
				delete newEdge;
			}
		}
	}

	return added + removed;
}


// continues in:
//   tracer_data.cpp
//   tracer_tracing.cpp
