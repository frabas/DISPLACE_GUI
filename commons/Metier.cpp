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
double _percent_revenue_completeness,
//vector<double> _selectivity,
const vector< vector <double> > &_selectivity_per_stock,
//vector<double> _discards,
vector<double> _betas_per_pop,
vector<double> _discardratio_limits,
vector<int>    _is_avoided_stocks,
vector<int>    _mls_cat_per_pop,
double _fspeed,
double _gear_width_a,
double _gear_width_b,
string _gear_width_model,
multimap<int, double> _loss_after_1_passage,
vector<int> _metier_target_stocks)
{
	name=_name;
	type=_type;
    percent_revenue_completeness=_percent_revenue_completeness;
    selectivity_per_stock=_selectivity_per_stock;
	betas_per_pop=_betas_per_pop;
    discardratio_limits=_discardratio_limits;
    is_avoided_stocks=_is_avoided_stocks;
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

void Metier::set_name(int name)
{
    Metier::name = name;
}


int Metier::get_metier_type()
{

	return(type);
}

double Metier::get_percent_revenue_completeness()
{

    return(percent_revenue_completeness);
}


double Metier::get_target_factor()
{

	return(target_factor);
}


/*
 vector<double> Metier::get_selectivity_ogive()
{

	return(selectivity);
}
*/

vector< vector <double> > Metier::get_selectivity_per_stock_ogives() const
{
    return(selectivity_per_stock);
}


/*
vector<double> Metier::get_discards_ogive()
{

	return(discards);
}
*/


vector<double> Metier::get_betas_per_pop()
{

	return(betas_per_pop);
}

vector<double> Metier::get_discardratio_limits()
{

    return(discardratio_limits);
}

vector<int> Metier::get_is_avoided_stocks()
{

    return(is_avoided_stocks);
}


int Metier::get_is_avoided_stocks(int stk)
{

    return(is_avoided_stocks.at(stk));
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

vector<int> Metier::get_metier_target_stocks()
{

    return(metier_target_stocks);
}


void Metier::set_betas_per_pop(vector<double> metier_betas_per_pop)
{

	betas_per_pop=metier_betas_per_pop;
}

void Metier::set_discardratio_limits(vector<double> metier_discardratio_limits)
{

    discardratio_limits=metier_discardratio_limits;
}

void Metier::set_is_avoided_stocks(vector<int> metier_is_avoided_stocks)
{

    is_avoided_stocks=metier_is_avoided_stocks;
}

void Metier::set_mls_cat_per_pop(vector<int> metier_mls_cat_per_pop)
{

    mls_cat_per_pop=metier_mls_cat_per_pop;
}

