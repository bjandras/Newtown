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

#include "model.h"
#include "math/tensor.h"
#include "math/vector2f.h"
#include "math/funcs.h"
#include "core/field.h"
#include "core/tracer.h"
#include "core/edge.h"
#include "core/seeder.h"
#include "core/grapher.h"
#include "core/district.h"
#include "core/block.h"
#include "core/volumebox.h"

#include <QtCore>
#include <QColor>
#include <QPainter>


using math::Tensor;
using math::Vector2f;


Model::Model()
	: core::City(NULL)
	, m_heightField()
	, m_discreteHeightField(20)
	, m_boundaryField()
	, m_discreteBoundaryField(256)
	, m_basisSumField(DEFAULT_DECAY_USEREDIT)
	, m_normalize(true)
{
	m_boundaryField.setDecay(DEFAULT_DECAY_BOUNDARY);
	
	m_weights[0] = DEFAULT_WEIGHT_HEIGHT;
	m_weights[1] = DEFAULT_WEIGHT_BOUNDARY;
	m_weights[2] = DEFAULT_WEIGHT_USEREDIT;
	
	m_discreteHeightField.loadValues(m_heightField);
	m_discreteBoundaryField.loadValues(m_boundaryField);
}

Tensor Model::operator()(math::Vector2f const & p) const
{
	Tensor t;

	if (selectedDistrict() != NULL)
	{
		if (! selectedDistrict()->contains(p))
		{
			return t;
		}
	}

	if (! boundaryImage().isNull())
	{
		QPointF ip = boundaryImage().toImageCoords(p);
		QRgb pixel = boundaryImage().pixel(ip.toPoint());

		if (QColor::fromRgba(pixel).hue() > 0)
		{
			return t;
		}
	}

	t += m_weights[0] * m_discreteHeightField(p);
	t += m_weights[1] * m_discreteBoundaryField(p);
	t += m_weights[2] * m_basisSumField(p);

	float n = t.value();
	if (normalizingEnabled() || n > 1)
	{
		if (n > 0) t = t / n;
	}
	
	return t;
}

void Model::addBasisField(core::BasisField * basisField)
{
	m_basisSumField += basisField;

	if (basisField->isSingularity())
	{
		seeder().addSingularity(basisField->p0);
	}

	emit basisFieldAdded(basisField);
	emit fieldChanged();
}

void Model::removeBasisField(core::BasisField * basisField)
{
	m_basisSumField -= basisField;

	if (basisField->isSingularity())
	{
		seeder().removeSingularity(basisField->p0);
	}

	emit basisFieldRemoved(basisField);
	emit fieldChanged();
}

void Model::setBoundaryImage(QImage const & image)
{
	m_boundaryField.setImage(image);
	m_discreteBoundaryField.loadValues(m_boundaryField);
	emit fieldChanged();
	
	seeder().setBoundaries(image);
	m_boundaryImage = image;
}

void Model::setHeightMapImage(QImage const & image)
{
	m_heightField.setImage(image);
	m_discreteHeightField.loadValues(m_heightField);
	emit fieldChanged();
}

void Model::setPopulationMapImage(QImage const & image)
{
	tracer().setPopulationMapImage(image);
}

void Model::setDecay(QString const & fieldName, float value)
{
	if (fieldName == "height")
	{
		// TODO
	}
	else if (fieldName == "boundary")
	{
		m_boundaryField.setDecay(value);
		m_discreteBoundaryField.loadValues(m_boundaryField);
	}
	else if (fieldName == "userEdit")
	{
		m_basisSumField.setDecay(value);
	}
	else
	{
		qDebug() << "unknown field:" << fieldName;
	}

	emit fieldChanged();
}

void Model::setWeight(QString const & fieldName, float value)
{
	if (fieldName == "height")
	{
		m_weights[0] = value;
	}
	else if (fieldName == "boundary")
	{
		m_weights[1] = value;
	}
	else if (fieldName == "userEdit")
	{
		m_weights[2] = value;
	}
	else
	{
		qDebug() << "unknown field:" << fieldName;
	}

	emit fieldChanged();
}


void Model::addSeedMarker(core::Point const & seedMarker)
{
	m_seedMarkers.append(seedMarker);
}

void Model::removeSeedMarker(core::Point const & seedMarker)
{
	m_seedMarkers.removeOne(seedMarker);
}


void Model::removeEdge(core::Edge * edge)
{
	// NOTE: functioning of this method relies on parent/child relation in edge/tracer/region objects.
	// This relation is not explicit.

	core::Tracer * tracer = dynamic_cast<core::Tracer*>(edge->parent());

	if (tracer != NULL)
	{
		core::Region * region = dynamic_cast<Region*>(tracer->parent());

		if (region != NULL)
		{
			region->removeEdge(edge);
		}
		else
		{
			qWarning() << "removeEdge: region is not a parent of the tracer of the edge being removed";
		}
	}
	else
	{
		qWarning() << "removeEdge: tracer is not a parent of the edge being removed";
	}
}


void Model::traceInit()
{
	// seeds from markers
	//
	foreach (core::Point seed, m_seedMarkers)
	{
		addSeed(seed);
	}

	// starting edges
	//
	core::Tracer::EdgeList edges;
	edges += tracer().traceDomainBounds();
	edges += tracer().traceBoundaries(m_boundaryImage);
	foreach (core::Edge * edge, edges)
	{
		if (edge->parent() != NULL)
		{
			grapher().connect(edge->v1(), edge->v2());
			emit edgeAdded(edge);
		}
	}
}

bool Model::traceStep()
{
	return core::City::traceStep(*this);
}

void Model::traceComplete()
{
	core::Tracer::EdgeList edges;

	// signal all existing edges non-existant
	//
	edges = tracer().edges();
	foreach (core::District * district, districts())
	{
		edges += district->tracer().edges();
	}
	foreach (core::Edge * edge, edges)
	{
		emit edgeRemoved(edge);
	}

	QTime swatch;
	swatch.start();

	blockSignals(true);

	bool more;
	do
	{
		more = traceStep();
	}
	while (more);

 	blockSignals(false);

	qDebug() << "traced" << edges.size() << "edges in" << swatch.elapsed() << "ms";

	// signal all edges as newly created
	//
	edges = tracer().edges();
	foreach (core::District * district, districts())
	{
		edges += district->tracer().edges();
	}
 	foreach (core::Edge * edge, edges)
 	{
 		emit edgeAdded(edge);
 	}
}


QPainterPath makePath(core::Edge * edge, int width, int height)
{
	core::Point p1 = edge->v1();
	core::Point p2 = edge->v2();

	QPointF qp1(width * p1.x(), height * (1 - p1.y()));
	QPointF qp2(width * p2.x(), height * (1 - p2.y()));

	QPainterPath path;

	path.moveTo(qp1);
	foreach (core::Point p, edge->trace())
	{
		QPointF qp(width * p.x(), height * (1 - p.y()));

		path.lineTo(qp);
		path.moveTo(qp);
	}
	path.lineTo(qp2);

	return path;
}

QImage Model::renderStreetMap()
{
	int width = 2048, height = 2048;

	QImage image(width, height, QImage::Format_ARGB32);
	QPainter painter(&image);

	// background color
	//
	painter.setBrush(QBrush(Qt::white));
	painter.drawRect(0,0, width, height);

	// boundary image
	//
	QImage img = boundaryImage();
	painter.drawImage(QRectF(0,0, width,height), img, img.rect());

	QPen penMajor = painter.pen();
	penMajor.setColor(Qt::black);
	penMajor.setWidth(3);

	QPen penMinor = painter.pen();
	penMinor.setColor(Qt::darkGray);
	penMinor.setWidth(2);

	QPen penBridge = painter.pen();
	penBridge.setColor(Qt::gray);
	penBridge.setWidth(6);

	core::Tracer::EdgeList edges;
	foreach (core::District * district, districts())
	{
		edges += district->tracer().edges();
	}
	edges += tracer().edges();

	painter.setPen(penMajor);
	foreach (core::Edge * edge, edges)
	{
		switch (edge->type())
		{
		case core::Edge::TypeMajorRoad:
		case core::Edge::TypeMinorRoad:
			{
				QPainterPath path = makePath(edge, width, height);
				painter.setPen(edge->type() == core::Edge::TypeMajorRoad ? penMajor : penMinor);
				painter.drawPath(path);
			}
			break;

		case core::Edge::TypeBridge:
			{
				QPainterPath path = makePath(edge, width, height);
				painter.setPen(penBridge);
				painter.drawPath(path);
				painter.setPen(penMajor);
				painter.drawPath(path);
			}
			break;

		default:
			;
		}
	}

	painter.end();

	return image;
}

QImage Model::renderPreviewTexture(QImage const & backgroundImage)
{
	int width = 2048, height = 2048;

	QImage image(width, height, QImage::Format_ARGB32);
	QPainter painter(&image);

	// background color
	//
	painter.setBrush(QBrush(QColor(186,195,152)));
	painter.drawRect(0,0, width, height);

	// boundary image
	//
	QImage img = backgroundImage;
	painter.drawImage(QRectF(0,0, width,height), img, img.rect());

	QPen penMajor = painter.pen();
	penMajor.setColor(Qt::white);
	penMajor.setWidth(14);

	QPen penMinor = painter.pen();
	penMinor.setColor(Qt::white);
	penMinor.setWidth(7);

	QPen penBridge = painter.pen();
	penBridge.setColor(Qt::gray);
	penBridge.setWidth(6);

	// draw blocks
	//
	painter.setBrush(QBrush(Qt::gray));
	foreach (core::District * district, districts())
	{
		foreach (core::Block * block, district->blocks())
		{
			QVector<core::Point> polygon = block->polygon().points();

			QPolygonF qpolygon(polygon.size());
			for (int i = 0; i < polygon.size(); ++i)
			{
				core::Point p =  polygon[i];
				QPointF qp(width * p.x(), height * (1 - p.y()));
				qpolygon[i] = qp;
			}

			painter.drawPolygon(qpolygon);
		}
	}

	core::Tracer::EdgeList edges;
	foreach (core::District * district, districts())
	{
		edges += district->tracer().edges();
	}
	edges += tracer().edges();

	// draw street network
	//
	painter.setPen(penMajor);
	foreach (core::Edge * edge, edges)
	{
		switch (edge->type())
		{
		case core::Edge::TypeMajorRoad:
		case core::Edge::TypeMinorRoad:
			{
				QPainterPath path = makePath(edge, width, height);
				painter.setPen(edge->type() == core::Edge::TypeMajorRoad ? penMajor : penMinor);
				painter.drawPath(path);
			}
			break;

		case core::Edge::TypeBridge:
			{
				QPainterPath path = makePath(edge, width, height);
				painter.setPen(penBridge);
				painter.drawPath(path);
				painter.setPen(penMajor);
				painter.drawPath(path);
			}
			break;

		default:
			;
		}
	}

	painter.end();

	return image;
}

QVector<core::VolumeBox> Model::buildings() const
{
	QList<core::VolumeBox> result;

	foreach (core::District * district, districts())
	{
		foreach (core::Block * block, district->blocks())
		{
			foreach (core::Block::Parcel parcel, block->parcels())
			{
				result << core::VolumeBox(parcel, 0.02f);
			}
		}
	}

	return result.toVector();
}
