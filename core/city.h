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
#ifndef CORE_CITY_H_
#define CORE_CITY_H_

#include "core/city.hh"
#include "core/region.h"
#include "core/field.hh"
#include "core/point.hh"
#include "core/edge.hh"
#include "core/district.hh"

#include <QVector>
#include <QList>


//! Represents the entire city area.
/*!
 * The entire city area is covered by one object of this class.
 *
 * Subregions are represented by the District class, and are created
 * by Region::findSubregions() method.
 */
class core::City : public Region
{
	Q_OBJECT;
public:
	//! Constructs the object.
	explicit City(QObject * parent = NULL);

	//! Clears the city's road network.
	void clear();
	//! Removes all districts.
	void removeDistricts();

//! \name Street graph.
//@{
	//! Performs one step of tracing.
	/*!
	 * \param field tensor field object to trace on
	 * \return indication whether there is more to trace
	 */
	bool traceStep(TensorField const & field);

	//! Simplifies the road network.
	void simplifyGraph();

	//! Runs the city district detection.
	void findSubregions();

	//! Adds the specified district.
	void addDistrict(core::District * district);
	//! Removes the specified district.
	void removeDistrict(core::District * district);
	//! Returns the list of districts contained in this city.
	QList<District *> districts() const;

	//! Marks the specified district as selected.
	void selectDistrict(core::District * district);
	//! Returns the currently selected district.
	/*!
	 * \return currently selected district, or NULL if none is selected
	 */
	District * selectedDistrict() const;
//@}

signals:
	//! Signal emitted when a district object has been added to the city.
	void districtAdded(core::District * district);
	//! Signal emitted when a district object has been removed from the city.
	void districtRemoved(core::District * district);

protected:
	//! Callback function called by base class when a subregion has been detected.
	/*!
	 * This implementation creates a District object in the specified area.
	 */
	void onSubregionFound(QVector<core::Point> const & border);

	//! Called when traceStep() is invoked on a pristine object.
	virtual void traceInit();

private:
	//! District objects contained in this city.
	QList<core::District *> m_districts;
	//! Currently selected district.
	core::District * m_selectedDistrict;
	//! District object that need road network tracing.
	QList<core::District *> m_districtsForTrace;
	//! District object that have complete road networks.
	QList<core::District *> m_districtsForSubs;

	//! Runs subregion detection in all districts.
	void findBlocks();

	//! Traces a field in the currently selected district.
	bool traceDistrict(TensorField const & field);

private slots:
	//! Callback function called when a district has added an edge.
	void on_district_edgeAdded(core::Edge * edge);
	//! Callback function called when a district has removed an edge.
	void on_district_edgeRemoved(core::Edge * edge);
	//! Callback function called when a district has added a seed.
	void on_district_seedAdded(core::Point const & p);
	//! Callback function called when a district has removed a seed.
	void on_district_seedRemoved(core::Point const & p);
};


#endif // ifndef CORE_CITY_H_
