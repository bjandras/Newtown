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

#include "core/parameters.h"

using namespace core;


Parameters * Parameters::s_instance = NULL;

Parameters * Parameters::instance()
{
	if (s_instance == NULL)
	{
		s_instance = new Parameters();
	}

	return s_instance;
}


Parameters::Parameters(QObject *parent)
	: QObject(parent)
{
}

void Parameters::set(KeyType const & key, ValueType const & value)
{
	m_map[key] = value;
	emit valueChanged(key, value);
}

Parameters::ValueType Parameters::get(KeyType const & key, ValueType const & def) const
{
	if (m_map.contains(key))
	{
		return m_map[key];
	}
	else
	{
		return def;
	}
}

Parameters::ValueType Parameters::get(KeyType const & key) const
{
	return m_map[key];
}
