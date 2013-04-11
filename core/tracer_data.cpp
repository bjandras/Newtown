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
#include "math/funcs.h"

#include <QtGlobal>

#include <cmath>


using namespace core;
using math::Vector2f;


// forward declarations
//@{

template<class T, class E>
static
void addGridElement(T & grid, E const & element);

template<class T, class E>
static
void removeGridElement(T & grid, E const & element);

template<class T, class E>
static
QList<E> findGridElement(T const & grid, math::Vector2f const & atPosition, math::Vector2f const & sweepDirection, float sweepAngle);

template<class T, class E>
static
QPair<int,int> getGridCell(T const & grid, math::Vector2f const & pos);

//
//@}


// class methods
//@{

Tracer::EdgeList Tracer::edges() const
{
	return m_edges;
}

int Tracer::edgesCount() const
{
	return m_edges.size();
}

void Tracer::addEdge(Edge * edge)
{
	if (m_edges.count(edge) == 0)
	{
		foreach (Point p, edge->trace())
		{
			addSamplePoint(SamplePoint(p, edge));
		}

		addVertex(edge->v1());
		addVertex(edge->v2());

		m_edges.append(edge);

		edge->setParent(this);
	}
}

void Tracer::removeEdge(Edge * edge)
{
	if (m_edges.count(edge) > 0)
	{
		m_edges.removeOne(edge);
		Q_ASSERT(m_edges.count(edge) == 0);

		foreach (Point p, edge->trace())
		{
			removeSamplePoint(SamplePoint(p, edge));
		}

		// remove vertices if no other edge is using them
		//
		if (! containsEdge(edge->v1()))
		{
			removeVertex(edge->v1());
		}
		if (! containsEdge(edge->v2()))
		{
			removeVertex(edge->v2());
		}

		if (edge->parent() == this)
		{
			edge->setParent(NULL);
		}
	}
}

void Tracer::addVertex(Vertex const & v)
{
	addGridElement(m_vertices, v);
}

void Tracer::removeVertex(Vertex const & v)
{
	removeGridElement(m_vertices, v);
}

void Tracer::addSamplePoint(SamplePoint const & sp)
{
	addGridElement(m_samplePoints, sp);
}

void Tracer::removeSamplePoint(SamplePoint const & sp)
{
	removeGridElement(m_samplePoints, sp);
}


bool Tracer::containsEdge(Vertex const & v) const
{
	foreach (Edge * edge, m_edges)
	{
		if (edge->v1() == v || edge->v2() == v)
		{
			return true;
		}
	}

	return false;
}

bool Tracer::containsEdge(Vertex const & v1, Vertex const & v2) const
{
	foreach (Edge * edge, m_edges)
	{
		if ((edge->v1() == v1 && edge->v2() == v2) || (edge->v1() == v2 && edge->v2() == v1))
		{
			return true;
		}
	}

	return false;
}

Tracer::EdgeList Tracer::findEdge(Vertex const & v) const
{
	EdgeList result;

	foreach (Edge * edge, m_edges)
	{
		if (edge->v1() == v || edge->v2() == v)
		{
			result.append(edge);
		}
	}

	return result;
}

Tracer::EdgeList Tracer::findEdge(Vertex const & v1, Vertex const & v2, bool ignoreOrder) const
{
	EdgeList result;

	foreach (Edge * edge, m_edges)
	{
		if ((edge->v1() == v1 && edge->v2() == v2) || (ignoreOrder && edge->v1() == v2 && edge->v2() == v1))
		{
			result.append(edge);
		}
	}

	return result;
}

bool Tracer::containsVertex(Vertex const & v) const
{
	return findVertex(v.pos(), 0.0f).contains(v);
}

Tracer::VertexList Tracer::findVertex(math::Vector2f const & atPosition, float radius) const
{
	return findVertex(atPosition, Vector2f(radius,0), M_PI);
}

Tracer::VertexList Tracer::findVertex(math::Vector2f const & atPosition, math::Vector2f const & sweepDirection, float sweepAngle) const
{
	return findGridElement<VertexGrid, Vertex>(m_vertices, atPosition, sweepDirection, sweepAngle);
}

Tracer::SamplePointList Tracer::findSamplePoint(math::Vector2f const & atPosition, float radius) const
{
	return findSamplePoint(atPosition, Vector2f(radius,0), M_PI);
}

Tracer::SamplePointList Tracer::findSamplePoint(math::Vector2f const & atPosition, math::Vector2f const & sweepDirection, float sweepAngle) const
{
	return findGridElement<SamplePointGrid, SamplePoint>(m_samplePoints, atPosition, sweepDirection, sweepAngle);
}

//
//@}


// helper functions
//@{

//! Calculates the cell in which the specified point is in.
template<class T, class E>
static
QPair<int,int> getGridCell(T const & grid, math::Vector2f const & pos)
{
	float x = fmax(0, fmin(pos(0), 1));
	float y = fmax(0, fmin(pos(1), 1));
	int row = fmin(roundf(x*grid.rows()), grid.rows()-1);
	int col = fmin(roundf(y*grid.cols()), grid.cols()-1);

	return qMakePair<int,int>(row, col);
}

//! Adds the specified element to the spatial grid.
template<class T, class E>
static
void addGridElement(T & grid, E const & element)
{
	QPair<int,int> cell = getGridCell<T,E>(grid, element.pos());

	if (grid(cell.first, cell.second).count(element) == 0)
	{
		grid(cell.first, cell.second).append(element);
	}
}

//! Removes the specified element from the spatial grid.
template<class T, class E>
static
void removeGridElement(T & grid, E const & element)
{
	QPair<int,int> cell = getGridCell<T,E>(grid, element.pos());
	grid(cell.first, cell.second).removeAll(element);
}

//! Comparator for distances from the reference point.
template<class E>
struct CloserToPosition
{
	//! Constructs the object.
	CloserToPosition(Vector2f const & pos) : m_pos(pos) {}

	//! Less-than comparison function.
	bool operator()(E const & left, E const & right)
	{
		return (left.pos() - m_pos).norm() < (right.pos() - m_pos).norm();
	}

	//! Reference point.
	Vector2f m_pos;
};

//! Finds elements that fall in the search area.
template<class T, class E>
static
QList<E> findGridElement(T const & grid, math::Vector2f const & atPosition, math::Vector2f const & sweepDirection, float sweepAngle)
{
	// cell that contains the specified position
	//
	QPair<int,int> cell = getGridCell<T,E>(grid, atPosition);
	int row = cell.first;
	int col = cell.second;

	float sweepRadius = sweepDirection.norm();
	Vector2f sweepDirectionN = sweepDirection / sweepRadius; // normalized sweep direction

	// number of adjacent rows/cols we should be looking in
	//
	int adjRows = ceilf(sweepRadius * grid.rows());
	int adjCols = ceilf(sweepRadius * grid.cols());

	QList<E> result;

	for (int dr = -adjRows; dr <= adjRows; ++dr)
	{
		int r = row + dr;
		if (r < 0 || r >= grid.rows()) continue;

		for (int dc = -adjCols; dc <= adjCols; ++dc)
		{
			int c = col + dc;
			if (c < 0 || c >= grid.cols()) continue;

			foreach (E const & element, grid(r,c))
			{
				Vector2f direction = element.pos() - atPosition;
				float distance = direction.norm();

				if (distance <= sweepRadius)
				{
					if (math::zero(distance) == 0)
					{
						result.append(element);
					}
					else
					{
						float angle = acosf((direction / distance) * sweepDirectionN);

						if (angle <= sweepAngle)
						{
							result.append(element);
						}
					}
				}
			}
		}
	}

	// sort elements with respect to their distance to the specified position
	qSort(result.begin(), result.end(), CloserToPosition<E>(atPosition));

	return result;
}

//
//@}
