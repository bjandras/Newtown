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
#ifndef FIELDPAINTER_H_
#define FIELDPAINTER_H_

#include "core/fieldpainter.hh"
#include "core/field.hh"
#include "math/vector2f.hh"

#include <QObject>
#include <QImage>
#include <QVector>

class QSize;
class QGLPixelBuffer;


//! Visualizer of tensor fields.
/*!
 * Objects of this class are used to create images of tensor field visualization.
 * The tensor field to be visualized is supplied in the constructor.
 *
 * Animation images are obtained by calling the paintImages() function. This function
 * paints two distinct images that can be blended together using the blend() function.
 *
 * If the tensor field definition has hanged call the needsRemake() function.
 */
class core::FieldPainter : public QObject
{
	Q_OBJECT;
public:
	//! Constructs the object.
	/*!
	 * \param size size of the painted image
	 * \param field tensor field to visualize
	 * \param parent object's parent, or NULL if no parent
	 */
	FieldPainter(QSize const & size, TensorField * field, QObject * parent);

	~FieldPainter();

	//! Sets the flag on whether the distorted mesh should be remade.
	/*!
	 * \param value new value for the flag
	 */
	void needsRemake(bool value);
	
	//! Paints field visualization images.
	/*!
	 * \param[out] imgx where to save the X-axis positive image
	 * \param[out] imgy where to save the Y-axis positive image
	 */
	void paintImages(QImage & imgx, QImage & imgy);

	//! Returns the current blend mask image.
	QImage blendImage();

	//! Mixes two images together based on the supplied blend mask.
	/*!
	 * imgx = wx*imgx + (1-wx)imgy
	 *
	 * \param[in,out] imgx first image and where to save the result
	 * \param[in] imgy second image
	 * \param[in] wx blend mask
	 */
	static void blend(QImage & imgx, QImage const & imgy, QImage const & wx);

	//! Mixes two images together based on the supplied blend mask.
	/*!
	 * result = wx*imgx + (1-wx)imgy
	 *
	 * \param[out] into where to save the result
	 * \param[in] imgx first image
	 * \param[in] imgy second image
	 * \param[in] wx blend mask
	 */
	static void blend(QImage & into, QImage const & imgx, QImage const & imgy, QImage const & wx);
	
protected:
	//! Called when OpenGL context is being initialized.
	void initializeGL();

	//! Called when the OpenGL context is resized.
	/*!
	 * \param w viewport width
	 * \param h viewport height
	 */
	void resizeGL(int w, int h);

	//! Called when the OpenGL drawing calls should be issued.
	void paintGL(bool flag);

private:
	//! Array of vertices.
	typedef QVector<math::Vector2f> Verticen;

	//! Size of the painted images.
	QSize m_size;
	//! Assigned tensor field that is being visualized.
	TensorField * m_field;
	//! Pixel buffer for painting the X-axis positive image (Ix).
	QGLPixelBuffer * m_pbx;
	//! Pixel buffer for painting the Y-axis positive image (Iy).
	QGLPixelBuffer * m_pby;
	//! Animation frame number.
	unsigned int m_frame;
	//! Image used as blend mask (Wx).
	QImage m_blendImage;
	//! Mesh texture coordinates.
	Verticen m_meshTexCoords;
	//! Mesh vertex coordinates for X-axis positive image.
	Verticen m_meshVertexVx;
	//! Mesh vertex coordinates for Y-axis positive image.
	Verticen m_meshVertexVy;
	//! Flag indicating whether the mesh should be remade.
	bool m_needsRemake;

	//! Remakes the distorted mesh if there's a need to.
	void remakeIfNeeded();
	//! Makes and returns the blend mask image.
	QImage makeBlendImage();

	//! Paints the current texture image.
	/*!
	 * The painter texutre is assigned as OpenGL current texture.
	 *
	 * \param flag indication whether the painted image is X-axis positive image.
	 */
	void paintTexture(bool flag);

	//! Creates distorted mesh.
	/*!
	 * The distorted mesh information is saved in m_meshTexCoords, m_meshVertexVx and m_meshVertexVy.
	 */
	void makeMesh();

	//! Creates background noise images.
	void makePatterns();
	
	//! Returns indication whether the distorted mesh should be remade.
	bool needsRemake() const;

	//! Returns tensor values at the specified point.
	/*!
	 * \param[in] p point in the field
	 * \param[out] f tensor angle
	 * \paran[out] r tensor magnitute
	 */
	void getT(math::Vector2f const & p, float & f, float & r);

	//! Returns vectors at the specified point.
	/*!
	 * \param[in] p point in the field
	 * \param[out] vx x-positive vector
	 * \param[out] vy y-positive vector
	 */
	void getV(math::Vector2f const & p, math::Vector2f & vx, math::Vector2f & vy);
};


#endif // ifndef FIELDPAINTER_H_
