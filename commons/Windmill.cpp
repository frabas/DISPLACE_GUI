// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2018 Francois Bastardie <fba@aqua.dtu.dk>

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
#include "Windmill.h"
#include <helpers.h>

#include <iterator>


Windmill::Windmill(int _idx, string _name, Node *_node, double _size, int _type, int _kWh, int _is_active)
    : name(_name), idx(_idx), type(_type), x(_node->get_x()), y(_node->get_y()),
      size(_size), kWh(_kWh), is_active(_is_active)
{
    kWProduction=0;
}

Windmill::~Windmill()
{
    //dtor
}

string Windmill::get_name() const
{
    return(name);
}

int Windmill::get_idx() const
{
    return(idx);
}

int Windmill::get_type() const
{
    return(type);
}


int Windmill::get_is_active() const
{
    return(is_active);
}

double Windmill::get_x() const
{
    return(x);
}

double Windmill::get_y() const
{
    return(y);
}

double Windmill::get_kWh() const
{
    return(kWh);
}

double Windmill::get_kW_production() const
{
    return(kWProduction);
}



void Windmill::set_x(double _x)
{
    x= _x;
}

void Windmill::set_y(double _y)
{
    y= _y;
}

void Windmill::set_kW_production(double _cumul)
{
    kWProduction= _cumul;
}


//------------------------------------------------------------//
//------------------------------------------------------------//
// methods
//------------------------------------------------------------//
//------------------------------------------------------------//


void Windmill::compute_kWproduction_in_farm()
{

    // just a cumul
    this->set_kW_production(this->get_kW_production() + this->get_kWh());

}


void Windmill::export_windmills_indicators(ofstream& windmillslogs, int tstep)
{

    dout(cout  << "export windmills indicators...." << endl);
    // note that this file will also be used by the ui for displaying the statistics in stat windows


    windmillslogs << setprecision(3) << fixed;

    // see parseWindfarmsStats():
    // tstep(0) / node(1) / long(2) / lat(3) / windfarmtype(4) / windfarmid(5) / kWh(6) / kW_production(7)

    windmillslogs <<  tstep << " " << 0 << " "<<  this->get_x() << " "<< this->get_y() << " "<<
        this->get_type() << " " << this->get_idx() << " " << this->get_kWh() << " " << this->get_kW_production() << " " << endl;


}


