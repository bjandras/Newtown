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
#ifndef MATH_GRAPH_H
#define MATH_GRAPH_H

#include "math/graph.hh"
#include "base/bitmatrix.h"
#include "base/matrix.h"

#include <QList>
#include <QPair>

#include <tr1/functional>


//! Representation of a undirected weightless graph.
/*!
 * Newly created graph is disconnected. Edges are established by calling the connect()
 * function and removed with the disconnect() function.
 */
struct math::Graph
{
	//! Vertex handle.
	typedef unsigned int Vertex;
	//! List of vertices.
	typedef QList<Vertex> VertexList;
	//! An edge between two vertices.
	typedef QPair<Vertex,Vertex> Edge;
	//! List of edges.
	typedef QList<Edge> EdgeList;

	//! A null-vertex value.
	static Vertex const NullVertex = 0;

	//! Constructs the disconnected graph with the specified number of vertices.
	/*!
	 * \param n a number of vertices in the graph
	 */
	explicit Graph(int n);

	//! Returns a number of vertices in the graph.
	unsigned int numVertices() const;

	//! Establishes an edge between the specified vertex pair.
	void connect(Vertex v1, Vertex v2);
	//! Removes the edge between the specified vertex pair.
	void disconnect(Vertex v1, Vertex v2);
	//! Tests whether two vertices are adjacent.
	bool connected(Vertex v1, Vertex v2) const;

	//! Returns all edges present in the graph;
	EdgeList edges() const;

	//! Returns vertices adjacent to the specified one.
	VertexList adjacents(Vertex v) const;

	//! Structure encapsulating the result of Floyd-Warshall all-pairs-shortest-paths algorithm.
	/*!
	 * Used for path reconstruction -- the getPath() method returns the shortest
	 * path between any two vertices.
	 */
	struct Paths
	{
		//! Constructor.
		Paths(base::Matrix<int> const & path, base::Matrix<Vertex> const & next)
			: m_path(path), m_next(next)
		{}

		//! Reconstructs the path between two vertices.
		/*!
		  * Returns the empty list if the path does not exist.
		  */
		VertexList getPath(Vertex start, Vertex end) const;

	private:
		base::Matrix<int>    m_path;
		base::Matrix<Vertex> m_next;

		//! Finds the path between two vertices.
		VertexList findPath(Vertex start, Vertex end) const;
	};

	//! Returns shortest paths between all pairs of vertices.
	/*!
	 * This method implements the Floyd-Warshall algorithm for finding
	 * shortest paths between all pairs of vertices.
	 */
	Paths allPairsShortestPaths() const;

	//! Returns the Minimum Cycle Basis of the graph.
	QList<VertexList> minimumCycleBasis() const;

	//! Cycle comparation function signature.
	typedef std::tr1::function<bool (VertexList const & left, VertexList const & right)> LessThan;
	
	//! Returns the Minimum Cycle Basis of the graph.
	/*!
	 * The cycle comparation function is specified by user.
	 *
	 * \param compareFunc LessThan comparator for sorting cycles
	 */
	QList<VertexList> minimumCycleBasis(LessThan lessThan) const;

private:
	//! Adjacency matrix.
	base::BitMatrix m_matrix;
};


#endif // ifndef MATH_GRAPH_H
