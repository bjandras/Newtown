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
#ifndef BASE_NAMES_H
#define BASE_NAMES_H


/*! \mainpage Newtown
 *
 * \section About
 *
 * The Newtown program code is part of my diploma thesis at the
 * Faculty of Electrical Engineering and Computing at University of Zagreb.
 *
 * \section Licensing
 *
 * Copyright (C) 2010. Borko Jandras, all rights reserved.
 *
 * Permission for using code contained as part of this project is granted for
 * personal and classroom use provided the copyright notice is left intact.
 */


//! Platform-independent fundation classes.
/*!
 * This module should contain essentials such as basic containers
 * and algorithms not present in STL.  Please keep this mode free from
 * any platform-dependent code.
 */
namespace base {}

//! Newtown core functionality.
/*!
 * Contains the implementation of core functionality of the Newtown project.
 */
namespace core {}

//! Computational geometry, linear algebra, trigonometry, etc.
/*!
 * Mathematical stuff goes here.  The same rigor that goes in
 * writing mathematical theorems and proofs should be exercised
 * when writing code in this module.
 */
namespace math {}


#endif // ifndef BASE_NAMES_H
