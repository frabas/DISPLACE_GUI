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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "Firm.h"
#include <helpers.h>
#include <Vessel.h>
#include <Node.h>
#include <Metier.h>
#include <comstructs.h>


#include <boost/make_shared.hpp>

#include <functional>
#include <stdexcept>


Firm::Firm()
{
    idx_firm = 0;
    x=0;
    y=0;
    init();
}


Firm::~Firm()
{

}



Firm::Firm(std::string a_name, int a_idx_firm, std::vector<Vessel*> &a_bunch_of_vessels)
{
    pthread_mutex_init(&mutex,0);

    idx_firm = a_idx_firm;
    bunch_of_vessels = a_bunch_of_vessels;
    name = a_name;
    x=0;
    y=0;
    dout(cout <<"firm creator...OK" << endl);
    init();
}


void Firm::init()
{


}



//------------------------------------------------------------//
//------------------------------------------------------------//
// getters...
//------------------------------------------------------------//
//------------------------------------------------------------//

int Firm::get_idx () const
{
    return(idx_firm);
}

double Firm::get_x () const
{
    return(x);
}

double Firm::get_y () const
{
    return(y);
}



string Firm::get_name () const
{
    return(name);
}


vector<Vessel*> Firm::get_bunch_of_vessels()
{
    return(bunch_of_vessels);
}


void Firm::set_x(double _x)
{
    x= _x;
}

void Firm::set_y(double _y)
{
    y= _y;
}

