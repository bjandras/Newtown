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

#include "core/city.h"
#include "core/edge.h"
#include "core/tracer.h"
#include "core/seeder.h"
#include "core/grapher.h"
#include "core/district.h"

#include <QTime>
#include <QDebug>
#include <QVector>


using namespace core;


City::City(QObject * parent)
	: Region(parent)
	, m_selectedDistrict(NULL)
{
}


void City::clear()
{
	removeDistricts();

	while (! seeder().empty())
	{
		emit seedRemoved(seeder().pop());
	}

	foreach (Edge * edge, tracer().edges())
	{
		removeEdge(edge);
	}
}

void City::removeDistricts()
{
	while (! m_districts.empty())
	{
		removeDistrict(m_districts.last());
	}
}


void City::traceInit()
{
	// this method intentionally left blank
}

bool City::traceStep(TensorField const & field)
{
	if (m_selectedDistrict == NULL && !m_districtsForTrace.empty())
	{
		selectDistrict(m_districtsForTrace.takeFirst());
	}

	if (m_selectedDistrict != NULL)
	{
		if (traceDistrict(field))
		{
			return true;
		}
		else
		{
			// tracing is done for this district
			m_districtsForSubs.append(m_selectedDistrict);

			selectDistrict(NULL);
			return traceStep(field);
		}
	}
	else
	{
		if (seeder().empty() && tracer().edgesCount() == 0)
		{
			traceInit();
		}

		 bool more = Region::traceField(field);

		 if (!more)
		 {
			 simplifyGraph();
		 }

		 return more;
	}
}

void City::simplifyGraph()
{
	if (m_selectedDistrict != NULL)
	{
		m_selectedDistrict->simplifyGraph();
	}
	else
	{
		core::Region::simplifyGraph();
	}
}

void City::findSubregions()
{
	if (m_selectedDistrict != NULL)
	{
		m_selectedDistrict->findSubregions();
	}
	else if (! m_districts.empty())
	{
		// we already have our subregions (districts), look for city blocks
		findBlocks();
	}
	else
	{
		core::Region::findSubregions();
	}
}

void City::findBlocks()
{
	QTime swatch;
	swatch.start();

	qDebug() << "running block detection";

	while (! m_districtsForSubs.empty())
	{
		District * district = m_districtsForSubs.takeFirst();
		district->findSubregions();
	}

	qDebug() << "block detection done in" << swatch.elapsed() << "ms";
}

bool City::traceDistrict(TensorField const & field)
{
	if (m_selectedDistrict != NULL)
	{
		return m_selectedDistrict->traceStep(field);
	}
	else
	{
		return false;
	}
}

void City::on_district_edgeAdded(core::Edge * edge)
{
	emit edgeAdded(edge);
}

void City::on_district_edgeRemoved(core::Edge * edge)
{
	emit edgeRemoved(edge);
}

void City::on_district_seedAdded(core::Point const & p)
{
	emit seedAdded(p);
}

void City::on_district_seedRemoved(core::Point const & p)
{
	emit seedRemoved(p);
}


void City::addDistrict(core::District * district)
{
	if (! m_districts.contains(district))
	{
		district->setParent(this);
		m_districts.append(district);

		connect(district, SIGNAL(edgeAdded(core::Edge*)), this, SLOT(on_district_edgeAdded(core::Edge*)));
		connect(district, SIGNAL(edgeRemoved(core::Edge*)), this, SLOT(on_district_edgeRemoved(core::Edge*)));
		connect(district, SIGNAL(seedAdded(core::Point)), this, SLOT(on_district_seedAdded(core::Point)));
		connect(district, SIGNAL(seedRemoved(core::Point)), this, SLOT(on_district_seedRemoved(core::Point)));

		emit districtAdded(district);
	}
	else
	{
		qWarning() << "attempting to add a duplicate district";
	}
}

void City::removeDistrict(core::District * district)
{	
	m_districtsForTrace.removeOne(district);
	m_districtsForSubs.removeOne(district);

	if (district == m_selectedDistrict)
	{
		selectDistrict(NULL);
	}

	// clear it out
	district->clear();

	// remove from records
	//
	if (m_districts.contains(district))
	{
		m_districts.removeOne(district);
		emit districtRemoved(district);

		district->disconnect(this);

		if (district->parent() == this)
		{
			district->setParent(NULL);
			delete district;
		}
	}
}

QList<core::District *> City::districts() const
{
	return m_districts;
}

void City::selectDistrict(core::District * district)
{
	m_selectedDistrict = district;
}

District * City::selectedDistrict() const
{
	return m_selectedDistrict;
}


void City::onSubregionFound(QVector<core::Point> const & border)
{
	core::District * district = new core::District(border, this);
	addDistrict(district);

	// tracing candidate
	m_districtsForTrace.append(district);
}
