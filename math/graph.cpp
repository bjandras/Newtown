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

#include "math/graph.h"
#include "base/bitmatrix.h"

#include <QSet>
#include <QDebug>
#include <QtAlgorithms>

#include <stdexcept>
#include <vector>
#include <set>
#include <climits>

#define DIST_INF (INT_MAX/2) // infinite distance


using namespace math;


// Returns matrix index for the specified vertex.
//
inline
int IDX(Graph::Vertex v)
{
	return v - 1;
}

inline
Graph::Edge makeEdge(Graph::Vertex v1, Graph::Vertex v2)
{
	return qMakePair(qMin(v1,v2), qMax(v1,v2));
}


Graph::Graph(int n)
	: m_matrix(n, n, false)
{
}


unsigned int Graph::numVertices() const
{
	return m_matrix.rows(); // or cols
}


void Graph::connect(Vertex v1, Vertex v2)
{
	if (v1 != NullVertex && v2 != NullVertex)
	{
		m_matrix(IDX(v1), IDX(v2)) = m_matrix(IDX(v2), IDX(v1)) = true;
	}
	else
	{
		throw std::runtime_error("using null vertex");
	}
}

void Graph::disconnect(Vertex v1, Vertex v2)
{
	if (v1 != NullVertex && v2 != NullVertex)
	{
		m_matrix(IDX(v1), IDX(v2)) = m_matrix(IDX(v2), IDX(v1)) = false;
	}
	else
	{
		throw std::runtime_error("using null vertex");
	}
}

bool Graph::connected(Vertex v1, Vertex v2) const
{
	if (v1 != NullVertex && v2 != NullVertex)
	{
		return m_matrix(IDX(v1), IDX(v2));
	}
	else
	{
		return false;
	}
}


Graph::EdgeList Graph::edges() const
{
	QSet<Edge> set;

	for (Vertex x = 1; x <= numVertices(); ++x)
	{
		for (Vertex y = x+1; y <= numVertices(); ++y)
		{
			if (connected(x,y))
			{
				set << makeEdge(x,y);
			}
		}
	}

	return set.toList();
}

Graph::VertexList Graph::adjacents(Vertex v) const
{
	VertexList result;

	for (Vertex u = 1; u <= numVertices(); ++u)
	{
		if (connected(v,u))
		{
			result.append(u);
		}
	}

	return result;
}


Graph::Paths Graph::allPairsShortestPaths() const
{
	unsigned int const n = numVertices();

	base::Matrix<int> path(n,n);
	base::Matrix<Vertex> next(n,n);

	path.fill(DIST_INF);
	next.fill(NullVertex);

	for (Vertex v1 = 1; v1 <= n; ++v1)
	{
		for (Vertex v2 = 1; v2 <= n; ++v2)
		{
			if (v1 == v2)
			{
				path(IDX(v1), IDX(v2)) = 0;
			}
			else if (connected(v1,v2))
			{
				path(IDX(v1), IDX(v2)) = 1;
			}
			else
			{
				path(IDX(v1), IDX(v2)) = DIST_INF;
			}
		}
	}

	for (Vertex vk = 1; vk <= n; ++vk)
	{
		for (Vertex vi = 1; vi <= n; ++vi)
		{
			for (Vertex vj = 1; vj <= n; ++vj)
			{
				// matrix indices
				int i = IDX(vi), j = IDX(vj), k = IDX(vk);

				if (path(i,k) + path(k,j) < path(i,j))
				{
					path(i,j) = path(i,k) + path(k,j);
					next(i,j) = vk; // NOTE: vk, not k
				}
			}
		}
	}

	return Paths(path, next);
}

Graph::VertexList Graph::Paths::getPath(Vertex start, Vertex end) const
{
	VertexList empty;

	if (m_path(IDX(start), IDX(end)) == DIST_INF)
	{
		return empty;
	}
	else
	{
		return empty << start << findPath(start, end) << end;
	}
}

Graph::VertexList Graph::Paths::findPath(Vertex start, Vertex end) const
{
	VertexList empty;

	Vertex intermediate = m_next(IDX(start), IDX(end));

	if (intermediate == NullVertex)
	{
		return empty;
	}
	else
	{
		return findPath(start, intermediate) + (empty << intermediate) + findPath(intermediate, end);
	}
}


//! Computes the standard inner (dot) product of two row-vectors.
/*!
 * The operation is done in the field F_2.
 *
 * \param matA matrix containing the first row-vector
 * \param rowA row index of the first row-vector
 * \param matB matrix containing the second row-vector
 * \param rowB row index of the second row-vector
 */
int innerProduct(base::BitMatrix & matA, int rowA, base::BitMatrix matB, int rowB)
{
	int s = 0;

	for (int c = 0; c < matA.cols(); ++c)
	{
		s += matA(rowA,c) & matB(rowB,c) ? 1 : 0;
	}

	return s % 2;
}

//! Returns the row index of the cycle C_i such that <C_i,S_j> = 1.
/*!
 * \param matHS Horton set matrix
 * \param matS matrix containing S_j vectors
 * \param rowS row index of the S_j vector
 * \return the index i of the C_i vector
 */
int findCycle(base::BitMatrix & matHS, base::BitMatrix & matS, int rowS)
{
	for (int r = 0; r < matHS.rows(); ++r)
	{
		if (innerProduct(matHS, r, matS, rowS) == 1)
		{
			return r;
		}
	}

	return -1;
}

//! Adds edges from S_i to S_j, i.e. S_j = S_j + S_i.
void addEdges(base::BitMatrix & matS, int i, int j)
{
	for (int c = 0; c < matS.cols(); ++c)
	{
		matS(j,c) = matS(j,c) || matS(i,c);
	}
}

//! Returns whether the first parameter has size less than the second one.
bool sizeLessThan(QList<math::Graph::Vertex> const & left, QList<math::Graph::Vertex> const & right)
{
	return left.size() < right.size();
}

QList<Graph::VertexList> Graph::minimumCycleBasis() const
{
	return minimumCycleBasis(&sizeLessThan);
}

QList<Graph::VertexList> Graph::minimumCycleBasis(LessThan lessThan) const
{
	typedef std::vector<bool> BitArray;

	EdgeList edges = this->edges();
	Paths paths = allPairsShortestPaths();

	QList<VertexList> cycles;
	std::set<BitArray> cycleSet; // incidence matrix

	// find all candidate cycles
	// (this set is a superset of the MCB -- let's call it the Horton set)
	//
	for (Vertex v = 1; v <= numVertices(); ++v)
	{
		foreach (Edge e, edges)
		{
			Vertex x = e.first;
			Vertex y = e.second;

			if (v == x || v == y) continue;

			VertexList p1 = paths.getPath(x,v);
			VertexList p2 = paths.getPath(v,y);

			if (!p1.empty() && !p2.empty())
			{
				p1.takeLast();

				QSet<Vertex> intersection = p1.toSet().intersect(p2.toSet());
				if (intersection.empty())
				{
					VertexList cycle = p1 + p2;

					// create an incidence vector for this cycle
					//
					std::vector<bool> incidenceVector(edges.size(), false);
					for (VertexList::iterator i = cycle.begin(); i != cycle.end(); ++i)
					{
						VertexList::iterator j = i+1;

						Vertex v1 = *i;
						Vertex v2 = (j != cycle.end()) ? *j : cycle.first();

						incidenceVector[edges.indexOf(makeEdge(v1,v2))] = true;
					}

					if (cycleSet.insert(incidenceVector).second)
					{
						cycles.append(p1 + p2);
					}
				}
			}
		}
	}

	// order by length
	qSort(cycles.begin(), cycles.end(), lessThan);

	// create the incidence matrix for the Horton set
	//
	base::BitMatrix matHS(cycles.size(), edges.size(), 0);
	for (int cycleIndex = 0; cycleIndex < cycles.size(); ++cycleIndex)
	{
		VertexList & cycle = cycles[cycleIndex];

		for (VertexList::iterator i = cycle.begin(); i != cycle.end(); ++i)
		{
			VertexList::iterator j = i+1;

			Vertex v1 = *i;
			Vertex v2 = (j != cycle.end()) ? *j : cycle.first();

			matHS(cycleIndex, edges.indexOf(makeEdge(v1,v2))) = 1;
		}
	}

	// construct the initial S matrix: S_i = {e_i}
	//
	base::BitMatrix matS(edges.size(), edges.size(), 0);
	for (int i = 0; i < edges.size(); ++i) matS(i,i) = 1;

	QSet<int> selectedCycles;
	QList<VertexList> result;

	// select MCB from the Horton set
	//
	for (int i = 0; i < edges.size(); ++i)
	{
		int ci = findCycle(matHS, matS, i);
		if (ci == -1) continue;

		selectedCycles.insert(ci);

		for (int j = i+1; j < edges.size(); ++j)
		{
			if (innerProduct(matS, j, matHS, ci))
			{
				addEdges(matS, i, j);
			}
		}
	}

	foreach (int ci, selectedCycles)
	{
		result.append(cycles[ci]);
	}

	return result;
}
