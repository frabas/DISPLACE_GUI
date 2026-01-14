// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2026 Francois Bastardie <fba@aqua.dtu.dk>

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

Tac::Tac(double init_tac, double _percent_for_simulated_vessels, 
	map<string,double> _relative_stability_key,
	map<int,double> _percent_tac_per_vessel_length_class,
	map<int, double> _percent_tac_cumul_over_months_key)
{

    dout(cout << "create tac "  << "\n");

	percent_for_simulated_vessels=_percent_for_simulated_vessels;
								 // % share per string country
	relative_stability_key=_relative_stability_key;
	ts_tac.push_back(init_tac);
	current_tac=init_tac;

	percent_tac_cumul_over_months_key = _percent_tac_cumul_over_months_key;
	map<string, double>::iterator it;
	for (it = _relative_stability_key.begin(); it != _relative_stability_key.end(); it++)
	{
		tac_per_nation.insert(std::make_pair(it->first, it->second / 100 * init_tac));
	}

	map<int, double>::iterator it2;
	for (it2 = _percent_tac_per_vessel_length_class.begin(); it2 != _percent_tac_per_vessel_length_class.end(); it2++)
	{
		tac_accessible_per_vessel_length_class.insert(std::make_pair(it2->first, it2->second / 100 * init_tac));
	}
	

		
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

map<int, double> Tac::get_percent_tac_cumul_over_months_key() const
{
	return(percent_tac_cumul_over_months_key);
}

vector<double> Tac::get_ts_tac() const
{
	return(ts_tac);
}


double Tac:: get_current_tac() const
{
	return(current_tac);
}

double Tac::get_tac_per_nation(string nation) 
{
	return(tac_per_nation[nation]);
}

double Tac::get_tac_accessible_per_vessel_length_class(int vessel_length_class)
{
	return(tac_accessible_per_vessel_length_class[vessel_length_class]);
}

void Tac::reset_tac_per_nation()
{
	map<string, double>::iterator it;
	for (it = relative_stability_key.begin(); it != relative_stability_key.end(); it++)
	{
		tac_per_nation[it->first] = (it->second) / 100 * this->get_current_tac();
	}
	
}

void Tac::reset_tac_accessible_per_vessel_length_class()
{
	map<int, double>::iterator it;
	for (it = percent_tac_per_vessel_length_class.begin(); it != percent_tac_per_vessel_length_class.end(); it++)
	{
		tac_accessible_per_vessel_length_class[it->first] = (it->second) / 100 * this->get_current_tac();
	}

}

int Tac::get_is_tac_exhausted() const
{
    return(is_tac_exhausted);
}


void Tac:: add_tac_to_ts(double a_tac, int current_is)
{
    ts_tac.push_back(a_tac);
    cout << "the TAC for y+1 is added to the TAC time series for this pop" << "\n";
    for(unsigned int i=0; i<ts_tac.size(); i++)
	{
        cout << "tac i"<< i <<": " << ts_tac.at(i) << "\n";
	}
    if(current_is==1) current_tac=a_tac;
    if(current_is==-1) current_tac=ts_tac.at(ts_tac.size()-2);
    cout << "current tac is " << current_tac << "\n";

	this->reset_tac_per_nation();

}

void Tac:: set_is_tac_exhausted(int val)
{
    is_tac_exhausted=val;

}
