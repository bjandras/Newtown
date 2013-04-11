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
#ifndef CORE_FIELD_H_
#define CORE_FIELD_H_

#include "core/field.hh"
#include "core/mapimage.h"
#include "math/tensor.h"
#include "math/vector2f.hh"

#include <QObject>
#include <list>


//! Tensor field interface.
/*!
 * This abstract class defines the interface that all tensor-field classes implement.
 *
 * The interface defines the function call operator that returns the tensor value at the
 * specified point.
 */
class core::TensorField
{
public:
	virtual ~TensorField() {}

	//! Returns the tensor value at the specified point.
	virtual math::Tensor operator()(math::Vector2f const & p) const = 0;
};


//! Basis tensor field.
/*!
 * Basis tensor field is created by expanding an user-specified constraint to the entire
 * field domain using the decaying radial basis function.
 */
class core::BasisField
{
public:
	//! Type of singularity.
	enum SingularityType
	{
		SingularityType_Center = 1,
		SingularityType_Wedge,
		SingularityType_Node,
		SingularityType_Trisector,
		SingularityType_Saddle,
		SingularityType_Focus,
		NumSingularityTypes
	};
	
	//! Location of tensor field element.
	const math::Vector2f p0;
	//! Scaling factor.
	const float scale;

	//! Constructs a regular element at the specified point.
	/*!
	 * \param p0_ location
	 * \param scale_ scaling factor
	 * \param direction direction of the regular element
	 */
	BasisField(const math::Vector2f & p0_, float scale_, const math::Vector2f & direction);

	//! Construct a singular element at the specified point.
	/*!
	 * \param p0_ location
	 * \param scale_ scaling factor
	 * \param type type of the singularity
	 */
	BasisField(const math::Vector2f & p0_, float scale_, SingularityType type);

	//! Returns whether this is a singularity tensor field element.
	bool isSingularity() const;

	//! Returns the tensor value at the specified point.
	math::Tensor operator()(math::Vector2f const & p) const;
	
private:
	//! Type of singularity, or 0 if regular element.
	SingularityType m_singularityType;
	//! Regular element value (not used for singularity elements).
	const math::Tensor m_regularValue;
};


//! Tensor field created by summing together basis fields.
/*!
 * The sum is performed using a radial-basis-sum funcition,
 * using the decay parameter assigned using the setDecay() function.
 */
class core::BasisSumField : public TensorField
{
public:
	//! Type of element.
	typedef BasisField Element;
	
	//! Constructs the object.
	/*!
	 * \param decay RBF decay parameter
	 */
	BasisSumField(float decay=4.0f);

	//! Destructor.
	~BasisSumField();

	//! Assigns the RBF decay parameter.
	void setDecay(float decay);
	//! Returns the RBF decay parameter.
	float decay() const;
	
	//! Adds a basis field to the sum.
	void operator+=(Element * element);
	//! Removes a basis field from the sum.
	void operator-=(Element * element);
	//! Removes all elements.
	void clear();
	
	//! Returns the tensor value at the specified point.
	math::Tensor operator()(math::Vector2f const & p) const;
	
private:
	//! Container type.
	typedef std::list<Element*> ElementList;
	
	//! Collection of elements.
	ElementList m_elements;
	//! RBF decay param.
	float m_decay;
};


//! Tensor field obtained from a heightmap image.
class core::HeightField : public virtual TensorField
{
public:
	//! Constructs the object.
	HeightField(QImage const & image = QImage());
	~HeightField();

	//! Returns the heightmap image.
	QImage image() const { return m_image; }
	//! Assigns the heightmap image.
	void setImage(QImage const & image);
	
	//! Returns the tensor value at the specified point.
	math::Tensor operator()(math::Vector2f const & p) const;

private:
	//! Heightmap image.
	MapImage m_image;
};


//! Tensor field that respects natural boundaries.
class core::BoundaryField : public virtual TensorField
{
public:
	//! Constructs the object.
	BoundaryField(QImage const & image = QImage());

	//! Returns the tensor value at the specified point.
	math::Tensor operator()(math::Vector2f const & p) const;

	//! Assigns the boundary map image.
	void setImage(QImage const & image);
	//! Returns the boundary map image.
	MapImage image() const { return m_image; }
	
	//! Assigns the RBF decay parameter.
	void setDecay(float value) { m_sumField.setDecay(value); }
	//! Returns the RBF decay parameter.
	float decay() const { return m_sumField.decay(); }

private:
	//! Boundary map image.
	MapImage m_image;
	//! Sum field.
	BasisSumField m_sumField;
};


//! Tensor field defined only at discrete points.
/*!
 * At other points the value is interpolated.
 */
class core::DiscreteField : public virtual TensorField
{
public:
	//! Constructs the object with specified number of lattice points.
	DiscreteField(int dim);

	~DiscreteField();

	//! Returns the tensor value at the specified point.
	math::Tensor operator()(math::Vector2f const & p) const;

	//! Loads values at lattice points from the specified tensor field.
	/*!
	 * \param field tensor field to load values from
	 */
	void loadValues(TensorField const & field);
	
private:
	//! Number of lattice points.
	int m_dim;
	//! Values at lattice points.
	math::Tensor ** m_matrix;
};


#endif // ifndef CORE_FIELD_H_
