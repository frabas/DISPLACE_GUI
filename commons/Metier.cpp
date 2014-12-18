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
double _gear_width_a,
double _gear_width_b,
string _gear_width_model,
multimap<int, double> _loss_after_1_passage)
{
	name=_name;
	type=_type;
	selectivity=_selectivity;
	discards=_discards;
	betas_per_pop=_betas_per_pop;
	gear_width_a=_gear_width_a;
	gear_width_b=_gear_width_b;
	gear_width_model=_gear_width_model;
	loss_after_1_passage=_loss_after_1_passage;
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


void Metier::set_betas_per_pop(vector<double> metier_betas_per_pop)
{

	betas_per_pop=metier_betas_per_pop;
}
