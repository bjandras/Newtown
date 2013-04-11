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

#include "core/grapher.h"
#include "core/point.h"
#include "math/graph.h"
#include "math/polygon.h"

#include <QDebug>
#include <QTime>

using namespace core;


//! Comparison operator for points.
/*!
 * Specifies the total order of points needed by QMap.
 */
bool operator<(Point const & left, Point const & right)
{
	math::Vector2f l = left.pos();
	math::Vector2f r = right.pos();

	return (l(0) != r(0)) ? l(0) < r(0) : l(1) < r(1);
}


Grapher::Grapher(QObject * parent)
	: QObject(parent)
{
}


void Grapher::connect(Vertex v1, Vertex v2)
{
	m_vertices[v1].append(Adjacency(v2));
	m_vertices[v2].append(Adjacency(v1));
}

void Grapher::disconnect(Vertex v1, Vertex v2)
{
	AdjacencyList & a1 = m_vertices[v1];
	for (AdjacencyList::iterator it = a1.begin(); it != a1.end(); )
	{
		it = (it->v == v2) ? a1.erase(it) : it+1;
	}

	AdjacencyList & a2 = m_vertices[v2];
	for (AdjacencyList::iterator it = a2.begin(); it != a2.end(); )
	{
		it = (it->v == v1) ? a2.erase(it) : it+1;
	}

	// remove disconnected vertices
	//
	if (a1.empty())
	{
		m_vertices.remove(v1);
	}
	if (a2.empty())
	{
		m_vertices.remove(v2);
	}
}


Grapher::EdgeList Grapher::edges() const
{
	EdgeList result;

	for (VertexMap::const_iterator it = m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		Vertex const & v1 = it.key();
		AdjacencyList const & adjs = it.value();

		for (AdjacencyList::const_iterator jt = adjs.begin(); jt != adjs.end(); ++jt)
		{
			Vertex const & v2 = jt->v;

			if (v1 < v2) // treat it as undirectional graph
			{
				result.append(qMakePair(v1,v2));
			}
		}
	}

	return result;
}


Grapher::VertexList Grapher::bridges() const
{
	VertexList result;

	for (VertexMap::const_iterator it = m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		Vertex const & v = it.key();
		AdjacencyList const & adjs = it.value();

		if (adjs.size() == 2)
		{
			result += v;
		}
	}

	return result;
}

Grapher::VertexList Grapher::dongles() const
{
	VertexList result;

	for (VertexMap::const_iterator it = m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		Vertex const & v = it.key();
		AdjacencyList const & adjs = it.value();

		if (adjs.size() == 1)
		{
			result += v;
		}
	}

	return result;
}


//! Implements comparison of loops in street graph.
/*!
 * Criterion for comparation is are enclosed by the loop.
 */
struct CompareCycles
{
	//! Compares the object
	/*!
	 * \param lut vertex lookup table
	 */
	CompareCycles(QList<Grapher::Vertex> & lut)
		: m_lut(lut)
	{}

	//! Less-than comparison function.
	bool operator()(QList<math::Graph::Vertex> const & left, QList<math::Graph::Vertex> const & right)
	{
		QList<core::Point> lp, rp;
			
		foreach (math::Graph::Vertex gv, left)
			lp.append(m_lut[gv-1]);
		foreach (math::Graph::Vertex gv, right)
			rp.append(m_lut[gv-1]);

		return math::Polygon(lp).area() < math::Polygon(rp).area();
	}

private:
	//! Vertex lookup table.
	QList<Grapher::Vertex> & m_lut;
};
	
Grapher::CycleList Grapher::cycles() const
{
	// vertex indices
	QList<Vertex> indices = m_vertices.keys();

	QTime swatch;
	swatch.start();
	int numEdges = 0;

	// construct the graph using vertex indices as vertex identifiers
	//
	math::Graph graph(m_vertices.size());
	for (VertexMap::const_iterator it = m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		Vertex const & v1 = it.key();
		AdjacencyList const & adjs = it.value();

		for (AdjacencyList::const_iterator jt = adjs.begin(); jt != adjs.end(); ++jt)
		{
			Vertex const & v2 = jt->v;

			// vertex identifiers
			//
			int gv1 = indices.indexOf(v1) + 1;
			int gv2 = indices.indexOf(v2) + 1;

			graph.connect(gv1,gv2);

			++numEdges;
		}
	}

	numEdges /= 2; // undirected graph

	qDebug() << "graph contains" << m_vertices.size() << "vertices and" << numEdges << "edges";

	// obtain the graph's MCB (this could take a while)
	QList<math::Graph::VertexList> mcb = graph.minimumCycleBasis(CompareCycles(indices));

	// construct cycle list from the graph's MCB
	//
	CycleList result;
	foreach (math::Graph::VertexList gcycle, mcb)
	{
		Cycle cycle;
		foreach (math::Graph::Vertex gv, gcycle)
		{
			cycle.append(indices[gv-1]);
		}

		result.append(cycle);
	}

	qDebug() << "found" << result.size() << "cycles in" << swatch.elapsed() << "ms";

	return result;
}
