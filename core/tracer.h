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
#ifndef CORE_TRACER_H_
#define CORE_TRACER_H_

#include "core/tracer.hh"
#include "core/edge.h"
#include "core/point.h"
#include "math/vector2f.h"
#include "base/matrix.h"
#include "core/field.hh"
#include "core/mapimage.h"
#include "core/parameters.h"

#include <QObject>
#include <QVector>
#include <QList>
#include <QPair>


//! Road-network tracer.
/*!
 * This class encapsulates all information and state necessary for tracing a road network.
 *
 * Each of the traceLineSegment(), traceDomainBounds(), traceBoundaries() and traceField()
 * methods return a list of edges.  This list contains both the newly created edges and those
 * removed when an intersection is established.  Edges that are contained in the tracer have
 * their parent object set to the tracer object, and those that are removed
 * are assigned NULL as parent.
 */
class core::Tracer : public QObject
{
	Q_OBJECT;
public:
	//! Road end-point or intersection.
	typedef Point Vertex;
	//! List of vertices.
	typedef QList<Vertex> VertexList;
	//! List of edges.
	typedef QList<Edge *> EdgeList;

	//! Type of road-network a tracer can be tracing.
	enum RoadType
	{
		RoadTypeMajor, //!< Major road network.
		RoadTypeLocal, //!< Local roads.
	};

public:
	//! Constructs the object.
	Tracer(RoadType type, QObject * parent = NULL);

//! \name Trace settings.
//@{
	//! Returns the road-type this tracer is tracing.
	RoadType roadType() const { return m_roadType; }

	//! Assigns the population map image.
	void setPopulationMapImage(MapImage const & image);
//@}

//! \name Element access.
//@{
	//! Returns all edges contained.
	EdgeList edges() const;

	//! Returns the number of edges contained.
	int edgesCount() const;

	//! Adds the specified edge.
	/*!
	 * Note: edge's trace is not checked in any way.
	 */
	void addEdge(Edge * edge);

	//! Removes the specified edge from edge list.
	void removeEdge(Edge * edge);

	//! Tests whether a vertex exists at the specified position.
	bool containsVertex(Vertex const & v) const;

	//! Tests whether an edge exists incident to the specified vertex.
	bool containsEdge(Vertex const & v) const;

	//! Tests whether an edge exists between the two specified vertices.
	bool containsEdge(Vertex const & v1, Vertex const & v2) const;

	//! Locates edges that are incident to the specified vertex.
	/*!
	 * \return list of edges that are attached to the specified vertex
	 */
	EdgeList findEdge(Vertex const & v) const;

	//! Locates edges that connect the specified two vertices.
	/*!
	 * \return list of edges that are spanned between specified pair of vertices
	 */
	EdgeList findEdge(Vertex const & v1, Vertex const & v2, bool ignoreOrder) const;
//@}
	
//! \name Tracing.
//@{
	//! Creates an edge for the specified line segment.
	/*!
	 * \param fromPoint segment starting point
	 * \param toPoint segment ending point
	 * \return list of created and removed edges
	 */
	EdgeList traceLineSegment(Point const & fromPoint, Point const & toPoint);

	//! Creates edges for domain bounds.
	/*!
	 * \return list of created and removed edges
	 */
	EdgeList traceDomainBounds();

	//! Creates edges for all boundaries found on the boundary image.
	/*!
	 * \param boundaryImage map image specifying boundaries to trace.
	 * \return list of created and removed edges
	 */
	EdgeList traceBoundaries(core::MapImage const & boundaryImage);

	//! Creates an edge for the specified boundary line.
	/*!
	 * \param boundary list of points defining the boundary to trace
	 * \return list of created and removed edges
	 */
	EdgeList traceBoundary(QVector<Point> const & boundary);

	//! Creates an edge by tracing the specified tensor field.
	/*!
	 * \return list of created and removed edges
	 */
	EdgeList traceField(core::TensorField const & field, bool major, math::Vector2f const & fromPosition, math::Vector2f const & inDirection);
//@}

	//! Simplifies the road network.
	/*!
	 * \param verts a list of vertices to use for simplification
	 * \return list of created and removed edges
	 */
	EdgeList simplify(VertexList const & verts);

private:
	//! A point within an edge trace.
	struct SamplePoint : public Point
	{
		//! Constructs a null sample-point.
		SamplePoint()
			: Point(), m_edge(NULL)
		{}

		//! Constructs a sample-point at the specified position and belonging to the specified edge.
		SamplePoint(Point const & p, Edge * edge = NULL)
			: Point(p), m_edge(edge)
		{}

		//! Equality operator.
		bool operator==(SamplePoint const & other) const
		{
			// edge member is just a placeholder, not to be used in comparison
			return Point::operator==(other);
		}

		//! Returns the edge object this sample-point is associated with.
		Edge * edge() const { return m_edge; }

	private:
		//! Edge this sample-point is a part of.
		Edge * m_edge;
	};

	//! List of sample points.
	typedef QList<SamplePoint> SamplePointList;
	//! Spatial grid for vertices.
	typedef base::Matrix<VertexList> VertexGrid;
	//! Spatial grid for sample-points.
	typedef base::Matrix<SamplePointList> SamplePointGrid;

	//! Assigned road type.
	RoadType m_roadType;
	//! Assigned population map image.
	MapImage m_populationMapImage;

//! \name Data elements.
//@{
	//! Spatial grid of vertices.
	VertexGrid m_vertices;
	//! Spatial grid of sample-points.
	SamplePointGrid m_samplePoints;
	//! List of edges.
	EdgeList m_edges;
//@}

//! \name Tracing parameters.
//@{
	//! Distance separating individual vertices.
	float m_distSep;
	//! Distance separating two trace lines.
	float m_koefTest;
	//! Minimum length of a road segment.
	float m_koefSegment;
	//! Extra distance to try traversing while looking for a way to complete a road segment.
	float m_koefLookahead;
	//! Maximum distance between two vertices that can be joined.
	float m_koefConnect;
	//! Distance between two consecutive sample-points.
	float m_distSample;
//@}

//! \name Tracing parameters.
//@{
	//! Returns the distance separating individual vertices.
	float distSep() const;
	//! Returns the distance separating individual vertices.
	float distSep(Point const & p) const;
	//! Returns the distance separating two trace lines.
	float distTest(Point const & p) const;
	//! Returns the minimum length of a road segment.
	float distSegment(Point const & p) const;
	//! Returns trhe extra distance to try traversing while looking for a way to complete a road segment.
	float distLookahead(Point const & p) const;
	//! Returns the distance between two consecutive sample-points.
	float distSample() const;
	//! Returns the maximum distance between two vertices that can be joined.
	float distConnect() const;
	//! Returns the maximum distance between two trace lines.
	float distTouch() const;

	//! Loads parameters from global parameter map.
	/*!
	 * The Parameters class is used for obtaining parameter values.
	 */
	void loadParameters();
//@}

private slots:
	//! Function called when a parameter value inside global map has changed.
	/*!
	 * \param key parameter key name
	 * \param value new parameter value
	 */
	void onParameterValueChanged(QString const & key, QVariant const & value);

private:
//! \name Element access.
//@{
	//! Adds the specified vertex to the spatial grid.
	void addVertex(Vertex const & v);
	//! Removes the specified vertex from the spatial grid.
	void removeVertex(Vertex const & v);
	//! Adds the specified sample-point to the spatial grid.
	void addSamplePoint(SamplePoint const & sp);
	//! Removes the specified sample-point from the spatial grid.
	void removeSamplePoint(SamplePoint const & sp);

	//! Finds vertices that satisfy the search condition.
	VertexList findVertex(math::Vector2f const & atPosition, float radius) const;
	//! Finds vertices that satisfy the search condition.
	VertexList findVertex(math::Vector2f const & atPosition, math::Vector2f const & sweepDirection, float sweepAngle) const;

	//! Finds sample-points that satisfy the search condition.
	SamplePointList findSamplePoint(math::Vector2f const & atPosition, float radius) const;
	//! Finds sample-points that satisfy the search condition.
	SamplePointList findSamplePoint(math::Vector2f const & atPosition, math::Vector2f const & sweepDirection, float sweepAngle) const;
//@}

//! \name Edge creation.
//@{
	//! Completes an edge that comes close to an existing vertex.
	/*!
	 * \param startVertex vertex from which the trace line starts
	 * \param trace trace line
	 * \param existingVertex vertex that the trace comes close to
	 * \return list of created and removed edges
	 */
	EdgeList completeEdge(Vertex const & startVertex, Edge::Trace const & trace, Vertex const & existingVertex);

	//! Completes an edge that touches an existing trace line.
	/*!
	 * \param startVertex vertex from which the trace line starts
	 * \param trace trace line
	 * \param existingSamplePoint point of contact on the existing trace
	 * \param touchingSamplePoint point of contact on the trace line
	 * \return list of created and removed edges
	 */
	EdgeList completeEdge(Vertex const & startVertex, Edge::Trace const & trace, SamplePoint const & existingSamplePoint,SamplePoint const & touchingSamplePoint);

	//! Completes an edge.
	/*!
	 * \param startVertex vertex from which the trace line starts
	 * \param trace trace line
	 * \return list of created and removed edges
	 */
	EdgeList completeEdge(Vertex const & startVertex, Edge::Trace const & trace);

	//! Type of road edge this tracer is laying.
	Edge::Types edgeType() const;
//@}

	//! Splits an edge at the specified split point.
	/*!
	 * The specified split point must be a member of existing edge's trace line.
	 *
	 * \param existingEdge edge to be split in two
	 * \param splitPoint point where the existing trace line is to be split
	 * \return a list of 3 edges -- two new ones added and the old one removed
	 */
	EdgeList splitEdge(Edge * existingEdge, SamplePoint const & splitPoint);
};


#endif // ifndef CORE_TRACER_H_
