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
#ifndef FIELDITEM_H_
#define FIELDITEM_H_

#include "core/field.hh"
#include "math/vector2f.hh"

#include <QAbstractGraphicsShapeItem>


class FieldItem : public QAbstractGraphicsShapeItem
{
public:
	FieldItem(QGraphicsItem * parent = NULL);
	~FieldItem();

	math::Vector2f basisScale() const;

	static qreal unitLength();
	QRectF unitRect() const;
	QRectF boundingRect() const;
	
	void paint(QPainter * painter, QStyleOptionGraphicsItem const * option, QWidget * widget);
	
protected:
	void keyReleaseEvent(QKeyEvent * event);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	
	virtual void handleItemSceneChange(QGraphicsScene * oldScene, QGraphicsScene * newScene);
	virtual void handleItemPositionChange(QPointF const & newPos);

	virtual core::BasisField * createBasisField(math::Vector2f const & p, math::Vector2f const & v) const;
	
	void createScaleArrow(qreal vx=1, qreal vy=0);
	void createMoveArrow();
	
private:
	class ArrowItem : public QGraphicsRectItem
	{
	public:
		ArrowItem(QPointF d, FieldItem * parent);

		QPointF direction() const;
		void setDirection(QPointF const & d);
		
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
		void mouseMoveEvent(QGraphicsSceneMouseEvent * event);

	private:
		FieldItem * m_parent;
	};

	core::BasisField * m_basisField;
	ArrowItem * m_scaleArrow;
	ArrowItem * m_moveArrow;
	QPointF m_basisScale;

	virtual void arrowMoved(ArrowItem * arrow);
};


class FieldItem_Cursor : public FieldItem
{
public:
	FieldItem_Cursor(QGraphicsItem * parent = NULL);

	void paint(QPainter * painter, QStyleOptionGraphicsItem const * option, QWidget * widget);
};


class FieldItem_Marker : public FieldItem
{
public:
	FieldItem_Marker(QGraphicsItem * parent = NULL);

	void paint(QPainter * painter, QStyleOptionGraphicsItem const * option, QWidget * widget);

protected:
	virtual void handleItemSceneChange(QGraphicsScene * oldScene, QGraphicsScene * newScene);
	virtual void handleItemPositionChange(QPointF const & newPos);
};


class FieldItem_Regular : public FieldItem
{
public:
	FieldItem_Regular(qreal vx, qreal vy, QGraphicsItem * parent = NULL);

	void paint(QPainter * painter, QStyleOptionGraphicsItem const * option, QWidget * widget);
	
protected:
	core::BasisField * createBasisField(math::Vector2f const & p, math::Vector2f const & v) const;
};


class FieldItem_Center : public FieldItem
{
public:
	FieldItem_Center(QGraphicsItem * parent = NULL);
	
protected:
	core::BasisField * createBasisField(math::Vector2f const & p, math::Vector2f const & v) const;
};


class FieldItem_Node : public FieldItem
{
public:
	FieldItem_Node(QGraphicsItem * parent = NULL);
	
protected:
	core::BasisField * createBasisField(math::Vector2f const & p, math::Vector2f const & v) const;
};


class FieldItem_Wedge : public FieldItem
{
public:
	FieldItem_Wedge(QGraphicsItem * parent = NULL);
	
protected:
	core::BasisField * createBasisField(math::Vector2f const & p, math::Vector2f const & v) const;
};


class FieldItem_Trisector : public FieldItem
{
public:
	FieldItem_Trisector(QGraphicsItem * parent = NULL);

protected:
	core::BasisField * createBasisField(math::Vector2f const & p, math::Vector2f const & v) const;
};


class FieldItem_Saddle : public FieldItem
{
public:
	FieldItem_Saddle(QGraphicsItem * parent = NULL);

protected:
	core::BasisField * createBasisField(math::Vector2f const & p, math::Vector2f const & v) const;
};


class FieldItem_Focus : public FieldItem
{
public:
	FieldItem_Focus(QGraphicsItem * parent = NULL);

protected:
	core::BasisField * createBasisField(math::Vector2f const & p, math::Vector2f const & v) const;
};


#endif // ifndef FIELDITEM_H_
