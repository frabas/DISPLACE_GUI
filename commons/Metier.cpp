// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "Metier.h"
#include <helpers.h>

Metier::Metier(int _name,
int _type,
vector<double> _selectivity,
vector<double> _discards,
vector<double> _betas_per_pop,
vector<int>    _mls_cat_per_pop,
double _fspeed,
double _gear_width_a,
double _gear_width_b,
string _gear_width_model,
multimap<int, double> _loss_after_1_passage,
multimap<int, int> _metier_target_stocks)
{
	name=_name;
	type=_type;
	selectivity=_selectivity;
	discards=_discards;
	betas_per_pop=_betas_per_pop;
    mls_cat_per_pop=_mls_cat_per_pop;
    fspeed= _fspeed;
    gear_width_a=_gear_width_a;
	gear_width_b=_gear_width_b;
	gear_width_model=_gear_width_model;
	loss_after_1_passage=_loss_after_1_passage;
    metier_target_stocks=_metier_target_stocks;
}


Metier::Metier()
{
}


Metier::~Metier()
{
	//dtor
}


int Metier::get_name()
{

	return(name);
}


int Metier::get_metier_type()
{

	return(type);
}


double Metier::get_target_factor()
{

	return(target_factor);
}


vector<double> Metier::get_selectivity_ogive()
{

	return(selectivity);
}


vector<double> Metier::get_discards_ogive()
{

	return(discards);
}


vector<double> Metier::get_betas_per_pop()
{

	return(betas_per_pop);
}

vector<int> Metier::get_mls_cat_per_pop()
{

    return(mls_cat_per_pop);
}


double Metier::get_fspeed()
{

    return(fspeed);
}

double Metier::get_gear_width_a()
{

	return(gear_width_a);
}


double Metier::get_gear_width_b()
{

	return(gear_width_b);
}


string Metier::get_gear_width_model()
{

	return(gear_width_model);
}


multimap<int, double> Metier::get_loss_after_1_passage()
{

	return(loss_after_1_passage);
}

multimap<int, int> Metier::get_metier_target_stocks()
{

    return(metier_target_stocks);
}


void Metier::set_betas_per_pop(vector<double> metier_betas_per_pop)
{

	betas_per_pop=metier_betas_per_pop;
}


void Metier::set_mls_cat_per_pop(vector<int> metier_mls_cat_per_pop)
{

    mls_cat_per_pop=metier_mls_cat_per_pop;
}

