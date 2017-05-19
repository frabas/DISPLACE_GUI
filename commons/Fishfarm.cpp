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


Fishfarm::Fishfarm(int _name, string _stringname, Node *_node, double _size, double _farm_original_long, double _farm_original_lat,
                   double _mean_SST, double _mean_salinity, double _mean_windspeed, double _mean_currentspeed, double _max_depth, double _diss_O2_mg_per_l,
                   double _Linf_mm, double _K_y, double _t0_y, double _fulton_condition_factor, string _meanw_growth_model_type,
                   int _start_day_growing, int _end_day_harvest, int _nb_days_fallowing_period,
                   int _nb_fish_at_start, double _meanw_at_start,
                   double _price_per_kg_at_start, double _target_meanw_at_harvest, int _nb_fish_at_harvest, double _meanw_at_harvest,
                   double _prop_harvest_kg_sold, double _kg_eggs_per_kg, double _price_eggs_per_kg,
                   double _N_in_fish_kg_3per, double _P_in_fish_kg_0_5per,
                   string _feed_types, double _feed_price_per_kg, double _total_feed_kg, double _prop_N_in_feed, double _prop_P_in_feed,
                   double _total_feed_N_kg, double _total_feed_P_kg,
                   string _feed_type_vet, double _feed_vet_price_per_kg, double _total_feed_vet_kg, double _prop_N_in_feed_vets, double _prop_P_in_feed_vet,
                   double _total_feed_vet_N_kg, double _total_feed_vet_P_kg,
                   double _annual_discharge_N_kg, double _annual_discharge_P_kg,
                   double _annual_discharge_C_kg, double _annual_discharge_heavymetals_kg,
                   double _annual_discharge_medecine_kg, double _net_harvest_kg_per_sqkm_y,
                   double _market_price_sold_fish, double _operating_cost_per_days, double _annual_profit
                   )
    : name(_name), stringname(_stringname), x(_node->get_x()), y(_node->get_y()),
      size(_size), farm_original_long(_farm_original_long), farm_original_lat(_farm_original_lat)
{
 p_location_ff=_node;
}

Fishfarm::~Fishfarm()
{
    //dtor
}

int Fishfarm::get_name() const
{
    return(name);
}

string Fishfarm::get_stringname() const
{
    return(stringname);
}

Node* Fishfarm::get_loc_ff() const
{
    return(p_location_ff);
}

double Fishfarm::get_size() const
{
    return(size);
}


double Fishfarm::get_farm_original_long() const
{
    return(farm_original_long);
}

double Fishfarm::get_farm_original_lat() const
{
    return(farm_original_lat);
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
