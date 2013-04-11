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

#include "core/edge.h"

#include <QVector>


using namespace core;


Edge::Edge(Point const & v1, Point const & v2, Trace const & trace, Types type)
	: QObject()
	, m_v1(v1), m_v2(v2)
	, m_trace(trace)
	, m_type(type)
{
}


void Edge::setType(Types type)
{
	m_type = type;
}

Edge::Types Edge::type() const
{
	return m_type;
}

bool Edge::isRoad() const
{
	return type() == TypeMajorRoad || type() == TypeMinorRoad;
}


Point const Edge::v1() const
{
	return m_v1;
}

Point const Edge::v2() const
{
	return m_v2;
}

Edge::Trace const Edge::trace() const
{
	return m_trace;
}

Edge::Trace const Edge::traceReversed() const
{
	int n = m_trace.size();

	QVector<Point> result(n);
	for (int i = 0; i < n; ++i)
	{
		result[i] = m_trace[n-1-i];
	}

	return result.toList();
}


Edge * Edge::join(Edge * e1, Edge * e2)
{
	Types type = e1->type(); // TODO

	if (e1->v2() == e2->v1())
		return new Edge(e1->v1(), e2->v2(), e1->trace() + e2->trace(), type);
	else if (e1->v2() == e2->v2())
		return new Edge(e1->v1(), e2->v1(), e1->trace() + e2->traceReversed(), type);
	else if (e1->v1() == e2->v1())
		return new Edge(e1->v2(), e2->v2(), e1->traceReversed() + e2->trace(), type);
	else if (e1->v1() == e2->v2())
		return new Edge(e2->v2(), e2->v1(), e2->traceReversed() + e1->traceReversed(), type);
	else
		return NULL;
}
