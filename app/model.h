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
#ifndef MODEL_H_
#define MODEL_H_

#include "core/city.h"
#include "core/field.h"
#include "core/point.h"
#include "core/edge.hh"
#include "core/district.hh"
#include "core/mapimage.hh"
#include "core/volumebox.hh"


#define DEFAULT_DECAY_BOUNDARY    4.0f
#define DEFAULT_DECAY_USEREDIT  130.0f
#define DEFAULT_WEIGHT_HEIGHT     0.3f
#define DEFAULT_WEIGHT_BOUNDARY   0.3f
#define DEFAULT_WEIGHT_USEREDIT   0.7f


class Model : public core::City, public virtual core::TensorField
{
	Q_OBJECT;
public:
	Model();
	
	math::Tensor operator()(math::Vector2f const & p) const;

//! \name Tensor field.
//@{	
	void addBasisField(core::BasisField * basisField);
	void removeBasisField(core::BasisField * basisField);

	core::MapImage boundaryImage() const { return m_boundaryField.image(); }
	void setBoundaryImage(QImage const & image);

	core::MapImage heightMapImage() const { return m_heightField.image(); }
	void setHeightMapImage(QImage const & image);

	void setPopulationMapImage(QImage const & image);

	bool normalizingEnabled() const { return m_normalize; }
	void normalizingEnable(bool value) { m_normalize = value; }
	
	void setDecay(QString const & fieldName, float value);
	void setWeight(QString const & fieldName, float value);
//@}

//! \name Street graph.
//@{
	void addSeedMarker(core::Point const & seedMarker);
	void removeSeedMarker(core::Point const & seedMarker);

	//! Removes the specified edge from the street graph.
	/*!
	 * Overrided so the specified edge is removed from the proper city/district object.
	 */
	void removeEdge(core::Edge * edge);

	void traceInit();
	bool traceStep();
	void traceComplete();
//@}

	QImage renderStreetMap();
	QImage renderPreviewTexture(QImage const & backgroundImage);
	QVector<core::VolumeBox> buildings() const;

signals:
	void basisFieldAdded(core::BasisField * field);
	void basisFieldRemoved(core::BasisField * field);
	void fieldChanged();

private:
	core::HeightField m_heightField;
	core::DiscreteField m_discreteHeightField;
	core::BoundaryField m_boundaryField;
	core::DiscreteField m_discreteBoundaryField;
	core::BasisSumField m_basisSumField;
	bool m_normalize;
	float m_weights[3];
	QImage m_boundaryImage;
	QList<core::Point> m_seedMarkers;
};


#endif // ifndef MODEL_H_
