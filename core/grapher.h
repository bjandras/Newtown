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
#ifndef CORE_GRAPH_H_
#define CORE_GRAPH_H_

#include "core/grapher.hh"
#include "core/tracer.hh"
#include "core/point.h"

#include <QObject>
#include <QPair>
#include <QList>
#include <QMap>


//! Encapsulates road-network's connectivity information.
/*!
 * This information is later used to find closed regions within
 * the road network.
 */
class core::Grapher : public QObject
{
	Q_OBJECT;
public:
	//! Vertex type.
	typedef Point Vertex;
	//! Edge type.
	typedef QPair<Vertex,Vertex> Edge;
	//! List of vertices.
	typedef QList<Vertex> VertexList;
	//! List of edges.
	typedef QList<Edge> EdgeList;
	//! A cycle in the graph.
	typedef QList<Vertex> Cycle;
	//! List of cycles.
	typedef QList<Cycle> CycleList;

	//! Constructs the object.
	Grapher(QObject * parent = NULL);

	//! Established a connection between the specified vertex pair.
	/*!
	 * \param v1 first vertex
	 * \param v2 second vertex
	 */
	void connect(Vertex v1, Vertex v2);

	//! Removes a connection between the specified vertex pair.
	/*!
	 * \param v1 first vertex
	 * \param v2 second vertex
	 */
	void disconnect(Vertex v1, Vertex v2);

	//! Lists all edges.
	/*!
	 * \return a list of edges
	 */
	EdgeList edges() const;

	//! Returns a list of vertices that form bridges.
	/*!
	 * \return a list of vertices that have two adjacent edges
	 */
	VertexList bridges() const;

	//! Returns a list of vertices that have only one adjacency.
	/*!
	 * \return a list of vertices that have one adjacent edge
	 */
	VertexList dongles() const;

	//! Returns a list of cycles present in the graph.
	CycleList cycles() const;

private:
	//! Encodes vertex adjacency.
	struct Adjacency
	{
		//! Adjacent vertex.
		Vertex v;

		//! Constructs the object.
		Adjacency(Vertex v_) : v(v_) {}
	};

	//! List of adjacencies.
	typedef QList<Adjacency> AdjacencyList;
	//! Adjacencies keyed by vertex.
	typedef QMap<Vertex, AdjacencyList> VertexMap;

	//! Graph's vertices.
	VertexMap m_vertices;
};


#endif // ifndef CORE_GRAPH_H_
