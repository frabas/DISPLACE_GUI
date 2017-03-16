// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef FISHFARM_H
#define FISHFARM_H

#include <commons_global.h>

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "myutils.h"
#include "Node.h"

class COMMONSSHARED_EXPORT Fishfarm
{
    public:
        Fishfarm(int _name, Node *_node, double size);

        Fishfarm();
        virtual ~Fishfarm();
                                 // Copy constructor
        int get_name() const;
        double get_x() const;
        double get_y() const;
        void set_x(double value);
        void set_y(double value);

    protected:
    private:
        int name;
        double x, y;
        double size;
};
#endif							 // FISHFARM_H
