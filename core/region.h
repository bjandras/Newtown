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
#ifndef CORE_REGION_H_
#define CORE_REGION_H_

#include "core/region.hh"
#include "core/tracer.hh"
#include "core/seeder.hh"
#include "core/grapher.hh"
#include "core/field.hh"
#include "core/point.hh"
#include "core/edge.hh"
#include "math/vector2f.hh"

#include <QObject>
#include <QVector>


//! Represents a region of space where road network is built.
/*!
 * Provides instances of Tracer, Grapher and Seeder classes that are
 * used during the road-construction process.
 */
class core::Region : public QObject
{
	Q_OBJECT
public:
	//! Construct the object.
	explicit Region(QObject * parent = NULL);

	//! Returns the associated tracer object.
	core::Tracer & tracer() const;
	//! Returns the associated seeder object.
	core::Seeder & seeder() const;
	//! Returns the associated graphing object.
	core::Grapher & grapher() const;

	//! Removes the specified edge from the street graph.
	/*!
	 * Edge information is removed from both the tracing and graphing subsystem.
	 * Emits the edgeRemoved signal.
	 *
	 * \param edge edge to be removed
	 */
	void removeEdge(Edge * edge);

	//! Adds the specified seed point.
	/*!
	 * Emits the seedAdded signal.
	 *
	 * \param seed seed point to be added
	 */
	void addSeed(Point const & seed);

	//! Removes the specified seed point.
	/*!
	 * Emits the seedRemoved signal.
	 *
	 * \param seed seed point to be removed
	 */
	void removeSeed(Point const & seed);

	//! Traces the specified field.
	/*!
	 * The starting point is taked from the seed-point with the highest priority.
	 *
	 * \param field tensor field to obtain the trace line from
	 * \retval true if the tracing process can be continued
	 * \retval false if the calling the function again will yield no new result
	 */
	bool traceField(TensorField const & field);

	//! Traces the specified field starting from the specified position.
	/*!
	 * \param fromPoint starting point of the tracing process
	 * \return number of new edges added
	 */
	int  traceField(TensorField const & field, Point const & fromPoint);

	//! Traces the specifield field.
	/*!
	 * \param fromPoint starting point of the tracing process
	 * \param inDirection direction the tracing process should take
	 * \return number of new edges added
	 */
	int  traceField(TensorField const & field, Point const & fromPoint, math::Vector2f const & inDirection, bool major);

	//! Traces the specified line segment.
	/*!
	 * \param fromPoint starting point of the tracing process
	 * \param toPoint point in which the tracing process should stop
	 * \return number of new edges added
	 */
	void traceLineSegment(Point const & fromPoint, Point const & toPoint);

	//! Simplifies the road network.
	void simplifyGraph();

	//! Locates closed regions withing the road network.
	void findSubregions();

signals:
	//! A signal emitted when an edge has been added to the road network.
	void edgeAdded(core::Edge * edge);
	//! A signal emitted when an edge has been removed from the road network.
	void edgeRemoved(core::Edge * edge);
	//! A signal emitted when a seed-point has been added to the seeder object.
	void seedAdded(core::Point const & p);
	//! A signal emitted when a seed-point has been removed from the seeder object.
	void seedRemoved(core::Point const & p);

protected:
	//! Flag indicating whether we're tracing a major road network (or not).
	bool m_traceMajorNetwork;

	//! Callback function called when a subregion has been detected.
	/*!
	 * This implementation constructs the border and calls foundSubregion(QVector<Point> const & border).
	 */
	virtual void onSubregionFound(QVector<Edge *> const & edges, QVector<bool> const & order);

	//! Callback function called when a subregion has been detected.
	/*!
	 * This implementation does nothing.
	 */
	virtual void onSubregionFound(QVector<Point> const & border);

private:
	//! Tracer object.
	core::Tracer * m_tracer;
	//! Seeder object.
	core::Seeder * m_seeder;
	//! Graphing object.
	core::Grapher * m_grapher;
	//! Flag indicating whether last trace step was in the direction of major eigenvector field (or not).
	bool m_lastTraceMajor;

	//! Creates the tracer object and assigns it to m_tracer.
	void createTracer();
	//! Creates the seeder object and assigns it to m_seeder.
	void createSeeder();
	//! Creates the graphing object and assigns it to m_grapher.
	void createGrapher();
};


#endif // ifndef CORE_REGION_H_
