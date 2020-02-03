// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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
#include "Tac.h"
#include <helpers.h>

Tac::Tac(double init_tac, double _percent_for_simulated_vessels, map<string,double> _relative_stability_key)
{

    dout(cout << "create tac "  << endl);

	percent_for_simulated_vessels=_percent_for_simulated_vessels;
								 // % share per string country
	relative_stability_key=_relative_stability_key;
	ts_tac.push_back(init_tac);
	current_tac=init_tac;

    is_tac_exhausted=0;
}


Tac::Tac()
{
	//ctor
}


Tac::~Tac()
{
	//dtor
}


Tac::Tac(const Tac& other)
{
    UNUSED(other);
	//copy ctor
}


double Tac::get_percent_for_simulated_vessels() const
{
	return(percent_for_simulated_vessels);
}


map<string,double> Tac::get_relative_stability_key() const
{
	return(relative_stability_key);
}


vector<double> Tac::get_ts_tac() const
{
	return(ts_tac);
}


double Tac:: get_current_tac() const
{
	return(current_tac);
}

int Tac::get_is_tac_exhausted() const
{
    return(is_tac_exhausted);
}


void Tac:: add_tac_to_ts(double a_tac, int current_is)
{
    ts_tac.push_back(a_tac);
    cout << "the TAC for y+1 is added to the TAC time series for this pop" << endl;
    for(unsigned int i=0; i<ts_tac.size(); i++)
	{
        cout << "tac i"<< i <<": " << ts_tac.at(i) << endl;
	}
    if(current_is==1) current_tac=a_tac;
    if(current_is==-1) current_tac=ts_tac.at(ts_tac.size()-2);
    cout << "current tac is " << current_tac << endl;

}

void Tac:: set_is_tac_exhausted(int val)
{
    is_tac_exhausted=val;

}
