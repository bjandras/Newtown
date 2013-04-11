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

#include "scene.h"
#include "model.h"
#include "app/fielditem.h"
#include "app/graphitem.h"
#include "core/point.h"
#include "core/tracer.h"
#include "core/seeder.h"
#include "core/district.h"
#include "math/vector2f.h"
#include "math/funcs.h"

#include <QtGui>

using math::Vector2f;


Scene::Scene(Model * model, QObject * parent)
	: QGraphicsScene(parent)
	, m_model(model)
	, m_lastKeyPress(Qt::Key_unknown)
	, m_selectedToolName("pointerTool")
{
	m_cursorItem = new FieldItem_Cursor;
	addItem(m_cursorItem);

	connect(model, SIGNAL(edgeAdded(core::Edge*)), this, SLOT(on_model_edgeAdded(core::Edge*)));
	connect(model, SIGNAL(edgeRemoved(core::Edge*)), this, SLOT(on_model_edgeRemoved(core::Edge*)));
	connect(model, SIGNAL(seedAdded(core::Point)), this, SLOT(on_model_seedAdded(core::Point)));
	connect(model, SIGNAL(seedRemoved(core::Point)), this, SLOT(on_model_seedRemoved(core::Point)));
	connect(model, SIGNAL(districtAdded(core::District*)), this, SLOT(on_model_districtAdded(core::District*)));
	connect(model, SIGNAL(districtRemoved(core::District*)), this, SLOT(on_model_districtRemoved(core::District*)));
}

Scene::~Scene()
{
}


Model & Scene::model()
{
	return *m_model;
}

Model const & Scene::model() const
{
	return *m_model;
}

math::Vector2f Scene::field(math::Vector2f const & p, bool major) const
{
	math::Tensor t = model()(p);

	float r = t.value();
	float f = t.angle();

	Vector2f v(r*cosf(f), r*sinf(f));

	return major ? v : Vector2f(-v(1), v(0));
}

math::Vector2f Scene::toFieldCoords(QPointF const & sceneCoords)
{
	qreal fx = sceneCoords.x() / width();
	qreal fy = (height() - sceneCoords.y()) / height();
	
	return Vector2f(fx,fy);
}

QPointF Scene::toSceneCoords(Vector2f const & fieldCoords)
{
	qreal sx = fieldCoords(0) * width();
	qreal sy = height() - fieldCoords(1) * height();
	
	return QPointF(sx,sy);
}


EdgeGraphItem * Scene::findItem(core::Edge * edge) const
{
	foreach (QGraphicsItem * item, items())
	{
		EdgeGraphItem * edgeItem = dynamic_cast<EdgeGraphItem*>(item);

		if (edgeItem)
		{
			if (edgeItem->edge() == edge)
			{
				return edgeItem;
			}
		}
	}

	return NULL;
}

SeedGraphItem * Scene::findItem(core::Point const & p) const
{
	foreach (QGraphicsItem * item, items())
	{
		SeedGraphItem * seedItem = dynamic_cast<SeedGraphItem*>(item);

		if (seedItem)
		{
			if (seedItem->location() == p)
			{
				return seedItem;
			}
		}
	}

	return NULL;
}

DistrictGraphItem * Scene::findItem(core::District * district) const
{
	foreach (QGraphicsItem * item, items())
	{
		DistrictGraphItem * districtItem = dynamic_cast<DistrictGraphItem*>(item);

		if (districtItem)
		{
			if (districtItem->district() == district)
			{
				return districtItem;
			}
		}
	}

	return NULL;
}


void Scene::clearField()
{
	// remove all non-toplevel items
	//
	foreach (QGraphicsItem * item, items())
	{
		if (item != m_cursorItem && item->parentItem() != NULL)
		{
			removeItem(item);
			// don't delete
		}
	}

	// remove and delete toplevel items
	foreach (QGraphicsItem * item, items())
	{
		if (item != m_cursorItem)
		{
			removeItem(item);
			delete item;
		}
	}
}

void Scene::selectTool(QString const & name)
{
	m_selectedToolName = name;
}


void Scene::keyPressEvent(QKeyEvent * keyEvent)
{
	m_lastKeyPress = keyEvent->key();

	switch (keyEvent->key())
	{
	case Qt::Key_L:
		model().clear();
		break;
	case Qt::Key_T:
		model().traceStep();
		break;
	case Qt::Key_P:
		model().findSubregions();
		break;
	case Qt::Key_S:
		model().simplifyGraph();
		break;
	case Qt::Key_D:
		{
			DistrictGraphItem * item = dynamic_cast<DistrictGraphItem *>(focusItem());
			if (item)
				model().selectDistrict(item->district());
			else
				model().selectDistrict(NULL);
		}
		break;
	}
	
	return QGraphicsScene::keyPressEvent(keyEvent);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	if (m_selectedToolName == "pointerTool")
	{
		return QGraphicsScene::mousePressEvent(mouseEvent);
	}
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	QPointF sp0 = mouseEvent->buttonDownScenePos(Qt::LeftButton);
	QPointF sp1 = mouseEvent->scenePos();

	Vector2f p0 = toFieldCoords(sp0);
	Vector2f p1 = toFieldCoords(sp1);
	Vector2f d = (p1 - p0) * width() / FieldItem::unitLength();

	FieldItem * item = NULL;

	if (m_selectedToolName == "seedTool")
	{
		model().addSeed(p0);
	}
	else if (m_selectedToolName == "markerTool")
	{
		item = new FieldItem_Marker;
	}
	else if (m_selectedToolName == "streamTool")
	{
		bool minor = mouseEvent->modifiers() & Qt::ControlModifier;
		traceStream(p0, p1-p0, !minor);
	}
	else if (m_selectedToolName == "lineTool")
	{
		traceLine(p0, p1);
	}
	else if (m_selectedToolName == "regularTool")
	{
		item = new FieldItem_Regular(d(0), d(1));
	}
	else if (m_selectedToolName == "centerTool")
	{
		item = new FieldItem_Center;
	}
	else if (m_selectedToolName == "nodeTool")
	{
		item = new FieldItem_Node;
	}
	else if (m_selectedToolName == "wedgeTool")
	{
		item = new FieldItem_Wedge;
	}
	else if (m_selectedToolName == "trisectorTool")
	{
		item = new FieldItem_Trisector;
	}
	else if (m_selectedToolName == "saddleTool")
	{
		item = new FieldItem_Saddle;
	}
	else if (m_selectedToolName == "focusTool")
	{
		item = new FieldItem_Focus;
	}

	if (item != NULL && sceneRect().contains(sp0))
	{
		item->setPos(sp0);

		if (item->scene() == NULL)
		{
			addItem(item);
		}
	}

	if (m_selectedToolName == "pointerTool")
	{
		return QGraphicsScene::mouseReleaseEvent(mouseEvent);
	}
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	m_lastScenePos = mouseEvent->scenePos();

	// manipulate with items (default action) only if proper tool is selected
	//
	if (m_selectedToolName == "pointerTool")
	{
		return QGraphicsScene::mouseMoveEvent(mouseEvent);
	}
}


void Scene::traceLine(math::Vector2f const & p0, math::Vector2f const & p1)
{
	model().traceLineSegment(p0, p1);
}

void Scene::traceStream(math::Vector2f const & p0, math::Vector2f const & dir, bool major)
{
	model().traceField(model(), p0, dir, major);
}

void Scene::on_model_edgeAdded(core::Edge * edge)
{
	new EdgeGraphItem(this, edge);
}

void Scene::on_model_edgeRemoved(core::Edge * edge)
{
	EdgeGraphItem * item = findItem(edge);

	if (item != NULL)
	{
		removeItem(item);
		delete item;
	}
	else
	{
		qDebug() << "no item associated with this edge";
	}
}

void Scene::on_model_seedAdded(core::Point const & p)
{
	new SeedGraphItem(this, p);
}

void Scene::on_model_seedRemoved(core::Point const & p)
{
	SeedGraphItem * item = findItem(p);

	if (item != NULL)
	{
		removeItem(item);
		delete item;
	}
	else
	{
		qDebug() << "no item associated with this seed point";
	}
}


void Scene::on_model_districtAdded(core::District * district)
{
	new DistrictGraphItem(this, district);
}

void Scene::on_model_districtRemoved(core::District * district)
{
	DistrictGraphItem * item = findItem(district);

	if (item != NULL)
	{
		removeItem(item);
		delete item;
	}
	else
	{
		qDebug() << "no item associated with this district";
	}
}
