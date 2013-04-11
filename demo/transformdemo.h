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
#ifndef TRANSFORMDEMO_H
#define TRANSFORMDEMO_H

#include "math/vector2f.h"

#include <QWidget>


class TransformDemo : public QWidget
{
	Q_OBJECT;
public:
	explicit TransformDemo(QWidget * parent = NULL);

protected:
	virtual void paintEvent(QPaintEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);

private:
	float m_f, m_r;
	math::Vector2f m_p;
	math::Vector2f m_t;
};


#endif // ifndef TRANSFORMDEMO_H
