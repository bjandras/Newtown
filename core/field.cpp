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

#include "core/field.h"
#include "core/border.h"
#include "math/funcs.h"
#include "math/vector2f.h"
#include "math/tensor.h"

#include <math.h>

#include <QtGui>


using namespace core;
using math::pow2;
using math::Tensor;
using math::Vector2f;


////////////////////////////////////////////////////////////////////////////////


BasisField::BasisField(const math::Vector2f & p0_, float scale_, const math::Vector2f & d)
	: p0(p0_), scale(scale_)
	, m_singularityType((SingularityType)0)
	, m_regularValue(1.0f, atan2(d(1), d(0)))
{
}

BasisField::BasisField(const math::Vector2f & p0_, float scale_, SingularityType type)
	: p0(p0_), scale(scale_)
	, m_singularityType(type)
	, m_regularValue()
{
}

bool BasisField::isSingularity() const
{
	return m_singularityType > 0 && m_singularityType < NumSingularityTypes;
}

math::Tensor BasisField::operator()(math::Vector2f const & p) const
{
	if (0 == m_singularityType)
	{
		return scale * m_regularValue;
	}
	else
	{
		Q_ASSERT(m_singularityType < NumSingularityTypes);

		Vector2f v = (p - p0).normalized();

		float x = v(0);
		float y = v(1);

		switch (m_singularityType)
		{
			case SingularityType_Center:
				return scale * math::Tensor::fromValues(pow2(y)-pow2(x), -2.0f*x*y);
			case SingularityType_Wedge:
				return scale * math::Tensor::fromValues(x, y);
			case SingularityType_Node:
				return scale * math::Tensor::fromValues(pow2(x)-pow2(y), 2.0f*x*y);
			case SingularityType_Trisector:
				return scale * math::Tensor::fromValues(x, -y);
			case SingularityType_Saddle:
				return scale * math::Tensor::fromValues(pow2(x)-pow2(y), -2.0f*x*y);
			case SingularityType_Focus:
				return scale * math::Tensor::fromValues(pow2(y)-pow2(x),  2.0f*x*y);
			default:
				// not reached, hopefully
				return math::Tensor();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////


BasisSumField::BasisSumField(float decay)
	: m_decay(decay)
{
}

BasisSumField::~BasisSumField()
{
	clear();
}

void BasisSumField::setDecay(float decay)
{
	m_decay = decay;
}

float BasisSumField::decay() const
{
	return m_decay;
}

void BasisSumField::operator+=(Element * element)
{
	m_elements.push_back(element);
}

void BasisSumField::operator-=(Element * element)
{
	m_elements.remove(element);
}

Tensor BasisSumField::operator()(Vector2f const & p) const
{
	ElementList::const_iterator it;
	unsigned int i;
	
	unsigned int numElements = m_elements.size();
	
	if (numElements == 0)
	{
		return Tensor();
	}
	
	typedef float real;
	typedef real realv[numElements];
	
	// calculate distances
	//
	realv dists;
	real  distsSum = 0.0f;
	for (i = 0, it = m_elements.begin(); i < numElements; ++i, ++it)
	{
		Element * bf = *it;
		
		real d = (p - bf->p0).norm();
		
		if (d == 0.0) d = 0.001;
		
		dists[i]  = d;
		distsSum += d;
	}
	
	// calculate "nearness" value
	//
	realv nears;
	real  nearsSum = 0.0f;
	for (i = 0; i < numElements; ++i)
	{
		real n = math::pow2(distsSum / dists[i]);
		
		nears[i]  = n;
		nearsSum += n;
	}
	
	// calculate vector sum
	//
	math::Tensor t;
	for (i = 0, it = m_elements.begin(); i < numElements; ++i, ++it)
	{
		Element * bf = *it;
		
		float w = nears[i] / nearsSum;
		
		t += w * rbf(p, bf->p0, decay()) * bf->scale * (*bf)(p);
	}
	
	return t;
}

void BasisSumField::clear()
{
	while (! m_elements.empty())
	{
		delete m_elements.front();
		m_elements.pop_front();
	}
}


////////////////////////////////////////////////////////////////////////////////


HeightField::HeightField(QImage const & image)
	: m_image(image)
{
}

HeightField::~HeightField()
{
}

void HeightField::setImage(QImage const & image)
{
	m_image = image;
}

Tensor HeightField::operator()(math::Vector2f const & p) const
{
	static const int dx = 2, dy = 2;
	
	if (m_image.isNull())
	{
		return Tensor();
	}
	
	QPoint  ip = m_image.toImageCoords(p).toPoint();

	if (! QRect(QPoint(0,0), m_image.size()-QSize(dx,dy)).contains(ip))
	{
		return Tensor();
	}
	
	QRgb pix0 = m_image.pixel(ip);
	float f0 = QColor::fromRgb(pix0).valueF();
	
	QRgb pix1 = m_image.pixel(ip + QPoint(dx,0));
	float f1 = QColor::fromRgb(pix1).valueF();

	QRgb pix2 = m_image.pixel(ip + QPoint(0,dy));
	float f2 = QColor::fromRgb(pix2).valueF();

	qreal dHx = 100.0f * (f1 - f0);
	qreal dHy = 100.0f * (f2 - f0);

	qreal f = atan2f(dHy, dHx) + M_PI/2.0f;
	qreal r = sqrtf(pow2(dHx) + pow2(dHy));

	return Tensor(r, f);
}


////////////////////////////////////////////////////////////////////////////////


BoundaryField::BoundaryField(QImage const & image)
{
	if (! image.isNull())
	{
		setImage(image);
	}
}

void BoundaryField::setImage(QImage const & image)
{
	m_image = image;
	m_sumField.clear();

	if (m_image.isNull())
	{
		return;
	}
	
	QPainter painter;
	painter.begin(&m_image);

	QPen pen;
	pen.setColor(QColor(100, 100, 255));
	painter.setPen(pen);
	
	border::Regions regions;
	border::findRegions(regions, image);

	foreach (border::Boundary const & boundary, regions)
	{
		foreach (border::BoundarySegment const & segment, boundary)
		{
			QPoint a = segment[0];
			Vector2f af = m_image.toFieldCoords(a);
			
			foreach (QPoint b, segment)
			{
				Vector2f bf = m_image.toFieldCoords(b);

				Vector2f d = bf - af;
				if (d.norm() > 0.02)
				{
					m_sumField += new BasisField(af, 1.0f, d);
					
					painter.drawLine(a,b);

					a = b;
					af = m_image.toFieldCoords(a);
				}
			}
		}
	}

	painter.end();
}

Tensor BoundaryField::operator()(math::Vector2f const & p) const
{
	return m_sumField(p);
}


////////////////////////////////////////////////////////////////////////////////


DiscreteField::DiscreteField(int dim)
	: m_dim(dim)
{
	int rows = m_dim+1, cols = m_dim+1;

	m_matrix = new Tensor*[rows];

	m_matrix[0] = new Tensor[rows*cols];
	for (int row = 1; row < rows; ++row)
	{
		m_matrix[row] = m_matrix[0] + row*cols;
	}

	memset(m_matrix[0], 0, sizeof(Tensor[rows*cols]));
}

DiscreteField::~DiscreteField()
{
	delete m_matrix[0];
	delete m_matrix;
	m_matrix = NULL;
}

void DiscreteField::loadValues(TensorField const & field)
{
	int rows = m_dim+1, cols = m_dim+1;

	float sx, sy;
	sx = sy = 1.0f / m_dim;

	QProgressDialog progress;
	progress.setLabelText("Loading values...");
	progress.setRange(0, rows);
	
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			Vector2f p(col*sx, row*sy);
			m_matrix[row][col] = field(p);
		}

		QCoreApplication::processEvents();
		
		progress.setValue(progress.value()+1);
		if (progress.wasCanceled())
		{
			// TODO: maybe revert?
			break;
		}
	}
}

Tensor DiscreteField::operator()(math::Vector2f const & p) const
{
	// clamp point to domain range
	//
	float x = fmax(0, fmin(p(0), 1));
	float y = fmax(0, fmin(p(1), 1));

	// we do the bilinear interpolation between lattice points here

	// distances between two lattice points
	//
	float sx, sy;
	sx = sy = 1.0f / m_dim;

	// lattice points indices
	//
	float r1 = floorf(y * m_dim);
	float c1 = floorf(x * m_dim);
	float r2 =  ceilf(y * m_dim);
	float c2 =  ceilf(x * m_dim);

	// lattice points coordinates
	//
	float x1 = c1 / m_dim;
	float y1 = r1 / m_dim;
	float x2 = c2 / m_dim;
	float y2 = r2 / m_dim;

	// integer indices
	//
	int r1i = (int)r1;
	int r2i = (int)r2;
	int c1i = (int)c1;
	int c2i = (int)c2;

	// values at lattice points
	//
	Tensor t11 = m_matrix[r1i][c1i];
	Tensor t21 = m_matrix[r1i][c2i];
	Tensor t12 = m_matrix[r2i][c1i];
	Tensor t22 = m_matrix[r2i][c2i];

	Tensor t1 = t11*(x2 - x)/sx + t21*(x - x1)/sx;
	Tensor t2 = t12*(x2 - x)/sx + t22*(x - x1)/sx;

	return t1*(y2 - y)/sy + t2*(y - y1)/sy;
}
