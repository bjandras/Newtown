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
#ifndef CORE_PARAMETERS_H
#define CORE_PARAMETERS_H

#include "core/parameters.hh"

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>


//! Global parameters map.
/*!
 * A global instance of this class keeps various parametes that control
 * many aspects of core functionality.
 */
class core::Parameters : public QObject
{
	Q_OBJECT
public:
	//! Parameter key type.
	typedef QString KeyType;
	//! Parameter value type.
	typedef QVariant ValueType;

	//! Returns the global instance.
	static Parameters * instance();

	//! Assigns the parameter value.
	/*!
	 * \param key parameter key
	 * \param value new parameter value
	 */
	void set(KeyType const & key, ValueType const & value);

	//! Fetches the parameter value.
	/*!
	 * \param key parameter key
	 * \param def default value if parameter has not been assigned previously
	 * \return parameter value
	 */
	ValueType get(KeyType const & key, ValueType const & def) const;

	//! Fetches the parameter value.
	/*!
	 * \param key parameter key
	 * \return parameter value
	 */
	ValueType get(KeyType const & key) const;

signals:
	//! Signal emmited when a parameter value has changed.
	void valueChanged(QString const & key, QVariant const & value);

private:
	//! Key/value map.
	QMap<KeyType, ValueType> m_map;

	//! Constructs the object.
	explicit Parameters(QObject * parent = NULL);

private:
	//! Global instance poiter.
	static Parameters * s_instance;
};


#endif // ifndef CORE_PARAMETERS_H
