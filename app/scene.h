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
#ifndef SCENE_H_
#define SCENE_H_

#include "math/vector2f.hh"
#include "core/point.hh"
#include "core/edge.hh"
#include "model.h"

#include <QGraphicsScene>


class Model;
class FieldItem;
class EdgeGraphItem;
class SeedGraphItem;
class DistrictGraphItem;


class Scene : public QGraphicsScene
{
	Q_OBJECT;
public:
	Scene(Model * model, QObject * parent);
	virtual ~Scene();

	Model       & model();
	Model const & model() const;

	math::Vector2f field(math::Vector2f const & p, bool major) const;
	
	math::Vector2f toFieldCoords(QPointF        const & sceneCoords);
	QPointF        toSceneCoords(math::Vector2f const & fieldCoords);

	EdgeGraphItem * findItem(core::Edge * edge) const;
	SeedGraphItem * findItem(core::Point const & p) const;
	DistrictGraphItem * findItem(core::District * district) const;

public slots:
	void clearField();
	void selectTool(QString const & toolName);
	
protected:
	void keyPressEvent(QKeyEvent * keyEvent);
	void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
	
private:
	Model * m_model;
	QPointF m_lastScenePos;
	int m_lastKeyPress;
	FieldItem * m_cursorItem;
	QString m_selectedToolName;

	void traceLine(math::Vector2f const & p0, math::Vector2f const & p1);
	void traceStream(math::Vector2f const & p0, math::Vector2f const & dir, bool major);

private slots:
	void on_model_edgeAdded(core::Edge * edge);
	void on_model_edgeRemoved(core::Edge * edge);
	void on_model_seedAdded(core::Point const & p);
	void on_model_seedRemoved(core::Point const & p);
	void on_model_districtAdded(core::District * district);
	void on_model_districtRemoved(core::District * district);
};


#endif // ifndef SCENE_H_
