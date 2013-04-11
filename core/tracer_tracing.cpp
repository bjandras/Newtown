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
#include "math/tensor.h"
#include "math/funcs.h"
#include "core/mapimage.h"
#include "core/border.h"
#include "core/field.h"

#include <cmath>

#include <QDebug>


using namespace core;
using math::Vector2f;


// numerical integrator step
#define RK4_STEP 0.0005f
// maximum number of steps of numerical integration before giving up
#define INSTEP_MAX 1000


Tracer::EdgeList Tracer::traceLineSegment(Point const & fromPoint, Point const & toPoint)
{
	// select the starting vertex
	//
	VertexList verticen = findVertex(fromPoint.pos(), distSep());
	Vertex startVertex  = verticen.empty() ? Vertex(fromPoint) : verticen[0];

	// line we're tracing
	//
	Vector2f line = toPoint.pos() - startVertex.pos();
	float lineLen = line.norm();

	Edge::Trace trace;
	Vertex existingVertex;
	SamplePoint existingSamplePoint, touchingSamplePoint;
	float existingDist = INFINITY;

	Vector2f sp = startVertex.pos(); // sample-point
	Vector2f td = line.normalized(); // tracing direction
	for (float segmentLength = 0; segmentLength < lineLen; )
	{
		Vector2f tp = sp + td*distSample(); // TODO: domain check

		segmentLength += distSample();
		sp = tp;

		// save this sample-point
		trace.append(sp);

		// look for an existing vertex to connect to
		//
		if (! existingVertex.finite())
		{
			verticen = findVertex(sp, td.normalized()*distConnect(), M_PI/2);
			verticen.removeOne(startVertex);
			if (! verticen.empty())
			{
				existingVertex = verticen[0];
			}
		}

		// find the closest existing sample-point
		//
		SamplePoint nearestSamplePoint = findSamplePoint(sp, td.normalized()*distTouch(), M_PI/3).value(0);
		if (nearestSamplePoint.finite())
		{
			float dist = (sp - nearestSamplePoint.pos()).norm();

			if (dist < existingDist)
			{
				touchingSamplePoint = trace.last();
				existingSamplePoint = nearestSamplePoint;
				existingDist = dist;
			}

			if (dist < distTouch()) break;
		}
	}

	// complete the edge
	//
	if (existingVertex.finite())
	{
		return completeEdge(startVertex, trace, existingVertex);
	}
	else if (existingSamplePoint.finite())
	{
		return completeEdge(startVertex, trace, existingSamplePoint, touchingSamplePoint);
	}
	else
	{
		return completeEdge(startVertex, trace, toPoint);
	}
}


Tracer::EdgeList Tracer::traceDomainBounds()
{
	EdgeList result =
		traceLineSegment(Point(0,0), Point(1,0)) +
		traceLineSegment(Point(1,0), Point(1,1)) +
		traceLineSegment(Point(1,1), Point(0,1)) +
		traceLineSegment(Point(0,1), Point(0,0));

	foreach (Edge * edge, result)
	{
		edge->setType(Edge::TypeBoundary);
	}

	return result;
}


Tracer::EdgeList Tracer::traceBoundaries(core::MapImage const & boundaryImage)
{
	EdgeList result;

	border::Regions regions;
	border::findRegions(regions, boundaryImage);

	foreach (border::Boundary const & boundary, regions)
	{
		foreach (border::BoundarySegment const & segment, boundary)
		{
			QVector<Point> segv(segment.size());

			for (border::BoundarySegment::const_iterator it = segment.begin(); it != segment.end(); ++it)
			{
				int i = it - segment.begin();
				segv[i] = boundaryImage.toFieldCoords(*it);
			}


			result += traceBoundary(segv);
		}
	}

	return result;
}

Tracer::EdgeList Tracer::traceBoundary(QVector<Point> const & boundary)
{
	EdgeList result;

	Point v1 = boundary.first();
	Point v2 = boundary.last();

	// TODO: why is distTouch not enough?
	SamplePoint spx = findSamplePoint(v1.pos(), 1.2*distTouch()).value(0);
	if (spx.finite())
	{
		result << splitEdge(spx.edge(), spx);
		v1 = spx;
	}

	// trace the boundary line
	//
	Edge::Trace trace;
	Point a = v1;
	for (QVector<Point>::const_iterator it = boundary.begin() + 1; it != boundary.end(); ++it)
	{
		Point b = *it;

		Vector2f d = b.pos() - a.pos();
		float dist = d.norm();

		if (dist > distSample())
		{
			trace.append(b);
			a = b;
		}
	}

	if (trace.empty())
	{
		return result;
	}

	v2 = trace.takeLast();	
	spx = findSamplePoint(v2.pos(), 1.2*distTouch()).value(0);
	if (spx.finite())
	{
		result << splitEdge(spx.edge(), spx);
		v2 = spx;
	}

	// record the edge object
	//
	Edge * edge = new Edge(v1, v2, trace, Edge::TypeBoundary);
	addEdge(edge);

	return result << edge;
}


// Returns the eigenvector of the specified tensor.
inline
Vector2f eigenv(math::Tensor const & t, bool major, Vector2f const & d)
{
	return math::orient(t.eigenVector(major), d);
}

float traceField(core::TensorField const & field, bool major, math::Vector2f & p, math::Vector2f & d, float distMax)
{
	static const float h = RK4_STEP; // integration interval

	float dist = 0;

	for (int instep = 0; instep < INSTEP_MAX; ++instep)
	{
		// trace the hiperstreamline using RK-4 numerical scheme
		//
		Vector2f m1 = eigenv(field(p            ), major, d);
		Vector2f m2 = eigenv(field(p + 0.5f*h*m1), major, d);
		Vector2f m3 = eigenv(field(p + 0.5f*h*m2), major, d);
		Vector2f m4 = eigenv(field(p + 1.0f*h*m3), major, d);
		Vector2f dp = (h / 6.0f) * (m1 + m2 + m3 + m4);

		float dpNorm = dp.norm();

		if (math::zero(dpNorm) == 0)
		{
			// reached some sort of singularity
			break;
		}

		Vector2f np = p + dp;
		float ndist = dist + dpNorm;

		if (ndist <= distMax && QRectF(0,0, 1,1).contains(QPointF(np(0), np(1))))
		{
			dist = ndist;
			p = np;
			d = dp;
		}
		else
		{
			break;
		}
	}

	return dist;
}

Tracer::EdgeList Tracer::traceField(core::TensorField const & field, bool major, math::Vector2f const & fromPosition, math::Vector2f const & inDirection)
{
	// select the starting vertex
	//
	VertexList verticen = findVertex(fromPosition, distSep());
	Vertex startVertex  = verticen.empty() ? Vertex(fromPosition) : verticen[0];

	Edge::Trace trace;
	Vertex existingVertex;
	SamplePoint existingSamplePoint, touchingSamplePoint;
	float existingDist = INFINITY;

	// trace the field's streamline
	//
	Vector2f sp = startVertex.pos(); // sample-point
	Vector2f td = inDirection;       // tracing direction
	for (float segmentLength = 0; segmentLength < distSegment(sp)+distLookahead(sp); )
	{
		Vector2f tp = sp; // tracing point
		float traceDist = ::traceField(field, major, tp, td, distSample());

		if (math::zero(traceDist) == 0.0f)
		{
			// could not find a new sample-point
			// (reached domain boundary or a degenerate point)
			break;
		}

		segmentLength += traceDist;
		sp = tp;

		// save this sample-point
		trace.append(sp);

		// look for an existing vertex to connect to
		//
		if (! existingVertex.finite())
		{
			verticen = findVertex(sp, td.normalized()*distConnect(), M_PI/2);
			verticen.removeOne(startVertex);
			if (! verticen.empty())
			{
				existingVertex = verticen[0];
			}
		}

		// find the closest existing sample-point
		//
		SamplePoint nearestSamplePoint = findSamplePoint(sp, td.normalized()*distTest(sp), M_PI/3).value(0);
		if (nearestSamplePoint.finite())
		{
			float dist = (sp - nearestSamplePoint.pos()).norm();

			// if there is a sample-point (from the same edge) that is nearer than the existing one,
			// or we don't have an existing sample-point yet and we're not looking ahead
			//
			if ((existingSamplePoint.finite() && (dist < existingDist) && (existingSamplePoint.edge() == nearestSamplePoint.edge()))
			|| (!existingSamplePoint.finite() && (segmentLength <= distSegment(sp))))
			{
				touchingSamplePoint = trace.last();
				existingSamplePoint = nearestSamplePoint;
				existingDist = dist;
			}

			if (dist < distTouch()) break;
		}
	}

	// complete the edge
	//
	if (existingVertex.finite())
	{
		return completeEdge(startVertex, trace, existingVertex);
	}
	else if (existingSamplePoint.finite())
	{
		return completeEdge(startVertex, trace, existingSamplePoint, touchingSamplePoint);
	}
	else
	{
		return completeEdge(startVertex, trace);
	}
}


Tracer::EdgeList Tracer::completeEdge(Vertex const & startVertex, Edge::Trace const & trace, Vertex const & existingVertex)
{
	EdgeList result;

	// see if this would be an existing edge
	//
	if (containsEdge(startVertex, existingVertex))
	{
		// this is an existing edge
		return result;
	}

	// find sample-point closest to the existing vertex
	//
	int spcount = 0;
	float lastDist = INFINITY;
	for (Edge::Trace::const_iterator it = trace.begin(); it != trace.end(); ++it, ++spcount)
	{
		float dist = (existingVertex.pos() - it->pos()).norm();
		if (dist > lastDist) break;
		lastDist = dist;
	}

	if (spcount == 0)
	{
		// no samples in this edge
		return result;
	}

	// create the new edge
	//
	Edge * edge = new Edge(startVertex, existingVertex, trace.mid(0, spcount), edgeType());
	addEdge(edge);

	return result << edge;
}

Tracer::EdgeList Tracer::completeEdge(Vertex const & startVertex, Edge::Trace const & trace, SamplePoint const & existingSamplePoint, SamplePoint const & touchingSamplePoint)
{
	EdgeList result;

	float dist = (touchingSamplePoint.pos() - existingSamplePoint.pos()).norm();
	if (dist > distTouch())
	{
		return result;
	}

	Edge * existingEdge = existingSamplePoint.edge();

	// check angle
	//
	Vector2f vx = (existingEdge->v1().pos() - existingEdge->v2().pos()).normalized();
	Vector2f vn = (touchingSamplePoint.pos() - startVertex.pos()).normalized();
	if (fabsf(acosf(vx * vn)) < M_PI/4 || fabsf(acosf(-vx * vn)) < M_PI/4)
	{
		return result;
	}

	// don't split too close to existing vertex
	//
	if (! findVertex(existingSamplePoint.pos(), 0.5*distTest(existingSamplePoint)).empty())
	{
		return result;
	}

	// new edge's trace-line is trace broken at touch-point
	//
	Edge::Trace newTrace = trace.mid(0, qFind(trace, touchingSamplePoint) - trace.begin());
	if (newTrace.empty()) return result;

	// split the existing edge
	//
	EdgeList split = splitEdge(existingEdge, existingSamplePoint);
	if (split.empty()) return result;

	// create the new edge
	//
	Edge * newEdge = new Edge(startVertex, existingSamplePoint.pos(), newTrace, edgeType());
	addEdge(newEdge);

	return result << split << newEdge;
}

Tracer::EdgeList Tracer::completeEdge(Vertex const & startVertex, Edge::Trace const & trace)
{
	EdgeList result;

	if (trace.empty())
	{
		return result;
	}

	Edge::Trace newTrace;
	float dist = 0;
	for (Edge::Trace::const_iterator it = trace.begin()+1; it != trace.end(); ++it)
	{
		Point samp = *it;
		Point prev = *(it-1);

		dist += (prev.pos() - samp.pos()).norm();

		if (dist <= distSegment(samp))
		{
			newTrace.append(samp);
		}
		else
		{
			break;
		}
	}

	if (newTrace.empty())
	{
		return result;
	}

	if (dist < distSep(newTrace.last()))
	{
		return result;
	}

	Vertex endVertex(newTrace.takeLast().pos());

	Edge * edge = new Edge(startVertex, endVertex, newTrace, edgeType());
	addEdge(edge);

	return result << edge;
}


Tracer::EdgeList Tracer::splitEdge(Edge * existingEdge, SamplePoint const & splitPoint)
{
	EdgeList result;

	// break existing trace-line into left and right halves (without the existing sample-point as center)
	//
	Edge::Trace existingTrace = existingEdge->trace();
	int midIndex = existingTrace.indexOf(splitPoint); Q_ASSERT(midIndex >= 0);
	Edge::Trace leftTrace  = existingTrace.mid(0, midIndex);
	Edge::Trace rightTrace = existingTrace.mid(midIndex+1);

	if (leftTrace.empty() || rightTrace.empty())
	{
		return result;
	}

	Vertex leftVertex = existingEdge->v1();
	Vertex rightVertex = existingEdge->v2();
	Vertex centerVertex = Vertex(splitPoint.pos());

	Edge * leftEdge  = new Edge(leftVertex,   centerVertex, leftTrace,  existingEdge->type());
	Edge * rightEdge = new Edge(centerVertex, rightVertex,  rightTrace, existingEdge->type());

	removeEdge(existingEdge);
	addEdge(leftEdge);
	addEdge(rightEdge);

	return result << leftEdge << rightEdge << existingEdge;
}


Edge::Types Tracer::edgeType() const
{
	return (roadType() == RoadTypeMajor) ? Edge::TypeMajorRoad : Edge::TypeMinorRoad;
}
