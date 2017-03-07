// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#ifndef HELPERS_H
#define HELPERS_H

#include <commons_global.h>

/* Hint: in Qt Creator you can fix the dout << "blablah"; with dout(cout<<"blablah"); using find/replace regexp
 * find: cout(.*);  replace: dout(cout \1);
 * */

extern int COMMONSSHARED_EXPORT verbosity;

#define outv(l,x) { if (verbosity >= l) { x; } }

#define dout(x) outv(3,x)
#define outc(x) outv(2,x)
#define tout(x) outv(1,x)

#define UNUSED(expr) do { (void)(expr); } while (0)

#endif // HELPERS_H
