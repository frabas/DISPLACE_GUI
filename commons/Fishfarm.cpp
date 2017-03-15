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

#include<string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Fishfarm.h"
#include <helpers.h>

#include <iterator>


Fishfarm::Fishfarm(int _name, Node *_node, double _size)
    : name(_name), x(_node->get_x()), y(_node->get_y()),
      size(_size)
{
}

Fishfarm::~Fishfarm()
{
    //dtor
}

int Fishfarm::get_name() const
{
    return(name);
}

double Fishfarm::get_x() const
{
    return(x);
}

double Fishfarm::get_y() const
{
    return(y);
}

void Fishfarm::set_x(double _x)
{
    x= _x;
}

void Fishfarm::set_y(double _y)
{
    y= _y;
}
