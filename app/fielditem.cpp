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

#include "fielditem.h"
#include "scene.h"
#include "model.h"
#include "core/field.h"
#include "math/vector2f.h"
#include "math/funcs.h"

#include <QtGui>

using math::Vector2f;


static QColor const colorFocused(Qt::red);
static QColor const colorNormal(Qt::blue);


FieldItem::FieldItem(QGraphicsItem * parent)
	: QAbstractGraphicsShapeItem(parent)
	, m_basisField(NULL)
	, m_scaleArrow(NULL)
	, m_moveArrow(NULL)
	, m_basisScale(1.0f, 0.0f)
{
	setFlag(ItemIsFocusable);
	setFlag(ItemSendsGeometryChanges);
	setZValue(10);
}

FieldItem::~FieldItem()
{
	delete m_basisField;
}


qreal FieldItem::unitLength()
{
	return 10.0;
}

QRectF FieldItem::unitRect() const
{
	qreal u = unitLength();
	return QRectF(-u,-u, 2*u, 2*u);
}

QRectF FieldItem::boundingRect() const
{
	float r = (basisScale() * unitLength()).norm();
	return QRectF(-r,-r, 2*r,2*r).united(unitRect());
}

void FieldItem::paint(QPainter * painter, QStyleOptionGraphicsItem const * /*option*/, QWidget * /*widget*/)
{
	QPen pen;
	pen.setColor(hasFocus() ? colorFocused : colorNormal);
	
	float r = (basisScale() * unitLength()).norm();

	painter->save();
	painter->setPen(pen);
	painter->drawEllipse(QPointF(0,0), r,r);
	painter->restore();
}


void FieldItem::keyReleaseEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Backspace)
	{
		event->accept();
		scene()->removeItem(this);
	}
}

QVariant FieldItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change)
	{
	case ItemSceneChange:
		if (value.canConvert<QGraphicsScene*>())
		{
			handleItemSceneChange(scene(), value.value<QGraphicsScene*>());
		}
		break;
	case ItemPositionChange:
		if (value.canConvert<QPointF>())
		{
			handleItemPositionChange(value.value<QPointF>());
		}
		break;
			
	default:
		break;
	}
	
	return QAbstractGraphicsShapeItem::itemChange(change, value);
}

void FieldItem::handleItemSceneChange(QGraphicsScene * oldScene_, QGraphicsScene * newScene_)
{
	Scene * oldScene = dynamic_cast<Scene*>(oldScene_);
 	Scene * newScene = dynamic_cast<Scene*>(newScene_);
	
	if (oldScene != NULL)
	{
		if (m_basisField != NULL)
		{
			oldScene->model().removeBasisField(m_basisField);
		}
	}

	if (m_basisField == NULL)
		m_basisField = createBasisField(newScene->toFieldCoords(pos()), basisScale());

	if (newScene != NULL)
	{
		if (m_basisField != NULL)
		{
			newScene->model().addBasisField(m_basisField);
		}
	}
}

void FieldItem::handleItemPositionChange(QPointF const & newPos)
{
	Scene * theScene = dynamic_cast<Scene*>(scene());
	if (theScene == NULL) return;

	if (m_basisField != NULL)
	{
		theScene->model().removeBasisField(m_basisField);
		delete m_basisField; m_basisField = NULL;
	}

	if (m_basisField == NULL)
		m_basisField = createBasisField(theScene->toFieldCoords(newPos), basisScale());

	if (m_basisField != NULL)
	{
		theScene->model().addBasisField(m_basisField);
	}
}


math::Vector2f FieldItem::basisScale() const
{
	return Vector2f(m_basisScale.x(), m_basisScale.y());
}

core::BasisField * FieldItem::createBasisField(Vector2f const & /*p*/, math::Vector2f const & /*v*/) const
{
	return NULL;
}

void FieldItem::createScaleArrow(qreal vx, qreal vy)
{
	m_basisScale = QPointF(vx, vy);
	
	if (m_scaleArrow != NULL)
	{
		m_scaleArrow->setParentItem(NULL);
		delete m_scaleArrow;
	}
	
	QPointF ad = QPointF(vx, -vy) * unitLength();
	m_scaleArrow = new ArrowItem(ad, this);
}

void FieldItem::createMoveArrow()
{
	if (m_moveArrow != NULL)
	{
		m_moveArrow->setParentItem(NULL);
		delete m_moveArrow;
	}

	m_moveArrow = new ArrowItem(QPoint(), this);
}

void FieldItem::arrowMoved(ArrowItem * arrow)
{
	if (arrow == m_scaleArrow)
	{
		update(); // if bounding rect got larger
		
		m_basisScale = m_scaleArrow->direction() / unitLength();
		m_basisScale.ry() *= -1; // flip y axis direction

		update(); // if bounding rect got smaller
		
		handleItemPositionChange(pos()); // TODO: something to make the basis field remake
	}
	else if (arrow == m_moveArrow)
	{
		QPointF dp = m_moveArrow->direction();
		m_moveArrow->setDirection(QPoint(0,0));
		moveBy(dp.x(), dp.y());
	}
}


FieldItem::ArrowItem::ArrowItem(QPointF d, FieldItem * parent)
	: QGraphicsRectItem(parent)
	, m_parent(parent)
{
	setDirection(d);
}

QPointF FieldItem::ArrowItem::direction() const
{
	QPointF p0(0,0);
	QPointF p1 = rect().topLeft();
	QSizeF  sz = rect().size();

	p1 += QPointF(sz.width()/2, sz.height()/2);

	return p1 - p0;
}

void FieldItem::ArrowItem::setDirection(QPointF const & d)
{
	QPointF p0(0,0);
	QPointF p1 = p0 + d;
	QSizeF  sz(6,6);
	
	p1 -= QPointF(sz.width()/2, sz.height()/2);
	
	setRect(QRectF(p1,sz));
}

void FieldItem::ArrowItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	event->accept();
}

void FieldItem::ArrowItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	m_parent->arrowMoved(this);
	event->accept();
}

void FieldItem::ArrowItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	QRectF r = rect();
	r.translate(event->scenePos() - event->lastScenePos());
	setRect(r);

	event->accept();
}


//////


FieldItem_Cursor::FieldItem_Cursor(QGraphicsItem * parent)
	: FieldItem(parent)
{
	setFlag(ItemIsMovable);
	setFlag(ItemIsFocusable, false);
}

void FieldItem_Cursor::paint(QPainter * painter, QStyleOptionGraphicsItem const * option, QWidget * widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	Scene * theScene = dynamic_cast<Scene*>(scene());
	if (theScene == NULL) return;

	Vector2f v = theScene->field(theScene->toFieldCoords(pos()), /*major=*/true);

	qreal x = v(0) * unitLength();
	qreal y = v(1) * unitLength();

	QPointF p1(0,0);
	QPointF p2 = p1 + QPointF(x,-y);
	QPointF p3 = p1 - QPointF(x,-y);

	painter->save();
	painter->setPen(colorNormal);
	painter->drawRect(unitRect());
	painter->setPen(Qt::magenta);
	painter->drawLine(p1, p2);
	painter->setPen(Qt::green);
	painter->drawLine(p1, p3);
	painter->restore();
}


//////


FieldItem_Marker::FieldItem_Marker(QGraphicsItem * parent)
	: FieldItem(parent)
{
	setFlag(ItemIsMovable);
}

void FieldItem_Marker::handleItemSceneChange(QGraphicsScene * oldScene_, QGraphicsScene * newScene_)
{
	Scene * oldScene = dynamic_cast<Scene*>(oldScene_);
	Scene * newScene = dynamic_cast<Scene*>(newScene_);

	if (oldScene != NULL)
	{
		oldScene->model().removeSeedMarker(oldScene->toFieldCoords(pos()));
	}

	if (newScene != NULL)
	{
		newScene->model().addSeedMarker(newScene->toFieldCoords(pos()));
	}
}

void FieldItem_Marker::handleItemPositionChange(QPointF const & newPos)
{
	Scene * theScene = dynamic_cast<Scene*>(scene());
	if (theScene == NULL) return;

	theScene->model().removeSeedMarker(theScene->toFieldCoords(pos()));
	theScene->model().addSeedMarker(theScene->toFieldCoords(newPos));
}

void FieldItem_Marker::paint(QPainter * painter, QStyleOptionGraphicsItem const * option, QWidget * widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	static QPointF const polygonPoints[4] =
	{
		QPointF(-10.0f,   0.0f),
		QPointF(  0.0f, -10.0f),
		QPointF( 10.0f,   0.0f),
		QPointF(  0.0f,  10.0f),
	};

	QPen pen;
	pen.setColor(hasFocus() ? colorFocused : colorNormal);

	painter->save();
	painter->setPen(pen);
	painter->drawPolygon(polygonPoints, 4);
	painter->restore();
}


//////


FieldItem_Regular::FieldItem_Regular(qreal vx, qreal vy, QGraphicsItem * parent)
	: FieldItem(parent)
{
	createScaleArrow(vx, vy);
	createMoveArrow();
}

core::BasisField * FieldItem_Regular::createBasisField(Vector2f const & p, Vector2f const & v) const
{
	return new core::BasisField(p, v.norm(), v);
}

void FieldItem_Regular::paint(QPainter * painter, QStyleOptionGraphicsItem const * option, QWidget * widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	Vector2f bs = basisScale();
	
	qreal x = bs(0) * unitLength();
	qreal y = bs(1) * unitLength();

	QPointF p0(0,0);
	QPointF p1 = p0 + QPointF(x,-y);
	QPointF p2 = p0 - QPointF(x,-y);

	QPen pen;
	pen.setColor(hasFocus() ? colorFocused : colorNormal);
	
	painter->save();
	painter->setPen(pen);
	painter->drawLine(p0, p1);
	painter->drawLine(p0, p2);
	painter->restore();
}


//////


FieldItem_Center::FieldItem_Center(QGraphicsItem * parent)
	: FieldItem(parent)
{
	createScaleArrow();
	createMoveArrow();
}

core::BasisField * FieldItem_Center::createBasisField(Vector2f const & p, Vector2f const & v) const
{
	return new core::BasisField(p, v.norm(), core::BasisField::SingularityType_Center);
}


//////


FieldItem_Node::FieldItem_Node(QGraphicsItem * parent)
	: FieldItem(parent)
{
	createScaleArrow();
	createMoveArrow();
}

core::BasisField * FieldItem_Node::createBasisField(Vector2f const & p, Vector2f const & v) const
{
	return new core::BasisField(p, v.norm(), core::BasisField::SingularityType_Node);
}


//////


FieldItem_Wedge::FieldItem_Wedge(QGraphicsItem * parent)
	: FieldItem(parent)
{
	createScaleArrow();
	createMoveArrow();
}

core::BasisField * FieldItem_Wedge::createBasisField(Vector2f const & p, Vector2f const & v) const
{
	return new core::BasisField(p, v.norm(), core::BasisField::SingularityType_Wedge);
}


//////


FieldItem_Trisector::FieldItem_Trisector(QGraphicsItem * parent)
	: FieldItem(parent)
{
	createScaleArrow();
	createMoveArrow();
}

core::BasisField * FieldItem_Trisector::createBasisField(Vector2f const & p, Vector2f const & v) const
{
	return new core::BasisField(p, v.norm(), core::BasisField::SingularityType_Trisector);
}


//////


FieldItem_Saddle::FieldItem_Saddle(QGraphicsItem * parent)
	: FieldItem(parent)
{
	createScaleArrow();
	createMoveArrow();
}

core::BasisField * FieldItem_Saddle::createBasisField(Vector2f const & p, Vector2f const & v) const
{
	return new core::BasisField(p, v.norm(), core::BasisField::SingularityType_Saddle);
}


//////


FieldItem_Focus::FieldItem_Focus(QGraphicsItem * parent)
	: FieldItem(parent)
{
	createScaleArrow();
	createMoveArrow();
}

core::BasisField * FieldItem_Focus::createBasisField(Vector2f const & p, Vector2f const & v) const
{
	return new core::BasisField(p, v.norm(), core::BasisField::SingularityType_Focus);
}
