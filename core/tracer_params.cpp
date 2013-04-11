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

#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QColor>

#include <cmath>


using namespace core;


void Tracer::loadParameters()
{
	QString thisSection = "tracer";
	QString thisSubsection = (roadType() == RoadTypeMajor) ? "major" : "local";
	QString key = QString("%1/%2/%3").arg(thisSection).arg(thisSubsection);

	Parameters * params = Parameters::instance();
	Q_ASSERT(params);

	m_distSep       = params->get(key.arg("distSep"),        (roadType() == RoadTypeMajor) ? 0.06f :  0.015f).toFloat();
	m_koefTest      = params->get(key.arg("koefTest"),       0.6f).toFloat();
	m_koefSegment   = params->get(key.arg("koefSegment"),    1.5f).toFloat();
	m_koefLookahead = params->get(key.arg("koefLookahead"),  0.5f).toFloat();
	m_koefConnect   = params->get(key.arg("koefConnect"),   0.15f).toFloat();
	m_distSample    = params->get(key.arg("distSample"),   0.002f).toFloat();
}

void Tracer::onParameterValueChanged(QString const & key, QVariant const & /*value*/)
{
	QString thisSection = "tracer";
	QString thisSubsection = (roadType() == RoadTypeMajor) ? "major" : "local";

	if (key.startsWith(thisSection + "/" + thisSubsection))
	{
		loadParameters();
	}
}


float Tracer::distSep() const
{
	return m_distSep;
}

float Tracer::distSep(Point const & p) const
{
	if (m_populationMapImage.isNull())
	{
		return distSep();
	}

	QPoint pxp = m_populationMapImage.toImageCoords(p.pos()).toPoint();
	QRgb rgb = m_populationMapImage.pixel(pxp);
	QColor c = QColor::fromRgb(rgb);
	float  k = c.valueF();

	return distSep() * (1.5f - k);
}

float Tracer::distTest(Point const & p) const
{
	return distSep(p) * m_koefTest;
}

float Tracer::distSegment(Point const & p) const
{
	return distSep(p) * m_koefSegment;
}

float Tracer::distLookahead(Point const & p) const
{
	return distSegment(p) * m_koefLookahead;
}

float Tracer::distSample() const
{
	return m_distSample;
}

float Tracer::distConnect() const
{
	return distSep() * m_koefConnect;
}

float Tracer::distTouch() const
{
	return 2 * math::sqrt(math::pow2(distSample()/2) + math::pow2(distSample()));
}
