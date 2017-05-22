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



//#include <exprtk.hpp> // too large for the use we want


Fishfarm::Fishfarm(int _name, string _stringname, Node *_node, double _size, double _farm_original_long, double _farm_original_lat,
                   double _mean_SST, double _mean_salinity, double _mean_windspeed, double _mean_currentspeed, double _max_depth, double _diss_O2_mg_per_l,
                   double _Linf_mm, double _K_y, double _t0_y, double _fulton_condition_factor,
                   string _meanw_growth_model_type,
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
      size(_size), farm_original_long(_farm_original_long), farm_original_lat(_farm_original_lat),
      mean_SST(_mean_SST), mean_salinity(_mean_salinity), mean_windspeed(_mean_windspeed), mean_currentspeed(_mean_currentspeed), max_depth(_max_depth), diss_O2_mg_per_l(_diss_O2_mg_per_l),
      Linf_mm(_Linf_mm), K_y(_K_y), t0_y(_t0_y), fulton_condition_factor(_fulton_condition_factor),
      meanw_growth_model_type(_meanw_growth_model_type),
      start_day_growing(_start_day_growing), end_day_harvest(_end_day_harvest), nb_days_fallowing_period(_nb_days_fallowing_period),
      nb_fish_at_start(_nb_fish_at_start), meanw_at_start(_meanw_at_start),
      price_per_kg_at_start(_price_per_kg_at_start), target_meanw_at_harvest(_target_meanw_at_harvest), nb_fish_at_harvest(_nb_fish_at_harvest), meanw_at_harvest(_meanw_at_harvest),
      prop_harvest_kg_sold(_prop_harvest_kg_sold), kg_eggs_per_kg(_kg_eggs_per_kg), price_eggs_per_kg(_price_eggs_per_kg),
      N_in_fish_kg_3per(_N_in_fish_kg_3per), P_in_fish_kg_0_5per(_P_in_fish_kg_0_5per),
      feed_types(_feed_types), feed_price_per_kg(_feed_price_per_kg), total_feed_kg(_total_feed_kg), prop_N_in_feed(_prop_N_in_feed), prop_P_in_feed(_prop_P_in_feed),
      total_feed_N_kg(_total_feed_N_kg), total_feed_P_kg(_total_feed_P_kg),
      feed_type_vet(_feed_type_vet), feed_vet_price_per_kg(_feed_vet_price_per_kg), total_feed_vet_kg(_total_feed_vet_kg),
      prop_N_in_feed_vets(_prop_N_in_feed_vets), prop_P_in_feed_vet(_prop_P_in_feed_vet),
      total_feed_vet_N_kg(_total_feed_vet_N_kg), total_feed_vet_P_kg(_total_feed_vet_P_kg),
      annual_discharge_N_kg(_annual_discharge_N_kg), annual_discharge_P_kg(_annual_discharge_P_kg),
      annual_discharge_C_kg (_annual_discharge_C_kg), annual_discharge_heavymetals_kg(_annual_discharge_heavymetals_kg),
      annual_discharge_medecine_kg (_annual_discharge_medecine_kg),  net_harvest_kg_per_sqkm_y(_net_harvest_kg_per_sqkm_y),
      market_price_sold_fish(_market_price_sold_fish), operating_cost_per_days(_operating_cost_per_days), annual_profit(_annual_profit)


{
 p_location_ff=_node;
}

Fishfarm::~Fishfarm()
{
    //dtor
}




/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * GETTERS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


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


double Fishfarm::get_Linf_mm() const
{
    return(Linf_mm);
}

double Fishfarm::get_K_y() const
{
    return(K_y);
}

double Fishfarm::get_t0_y() const
{
    return(t0_y);
}

double Fishfarm::get_fulton_condition_factor() const
{
    return(fulton_condition_factor);
}

string Fishfarm::get_meanw_growth_model_type() const
{
    return(meanw_growth_model_type);
}

double Fishfarm::get_sim_individual_mean_kg() const
{
    return(sim_individual_mean_kg);
}

double Fishfarm::get_sim_kg_harvested() const
{
    return(sim_kg_harvested);
}

double Fishfarm::get_sim_kg_eggs_harvested() const
{
    return(sim_kg_eggs_harvested);
}

double Fishfarm::get_sim_annual_profit() const
{
    return(sim_annual_profit);
}

double Fishfarm::get_sim_net_discharge_N() const
{
    return(sim_net_discharge_N);
}

double Fishfarm::get_sim_net_discharge_P() const
{
    return(sim_net_discharge_P);
}

double Fishfarm::get_sim_net_discharge_C() const
{
    return(sim_net_discharge_C);
}

double Fishfarm::get_sim_net_discharge_medecine() const
{
    return(sim_net_discharge_medecine);
}




/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * SETTERS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/



void Fishfarm::set_x(double _x)
{
    x= _x;
}

void Fishfarm::set_y(double _y)
{
    y= _y;
}



void Fishfarm::set_sim_individual_mean_kg(double _value)
{
    sim_individual_mean_kg= _value;
}
void Fishfarm::set_sim_kg_harvested(double _value)
{
    sim_kg_harvested= _value;
}

void Fishfarm::set_sim_kg_eggs_harvested(double _value)
{
    sim_kg_eggs_harvested= _value;
}

void Fishfarm::set_sim_annual_profit(double _value)
{
    sim_annual_profit= _value;
}

void Fishfarm::set_sim_net_discharge_N(double _value)
{
    sim_net_discharge_N= _value;
}

void Fishfarm::set_sim_net_discharge_P(double _value)
{
    sim_net_discharge_P= _value;
}

void Fishfarm::set_sim_net_discharge_C(double _value)
{
    sim_net_discharge_C= _value;
}

void Fishfarm::set_sim_net_discharge_medecine(double _value)
{
    sim_net_discharge_medecine= _value;
}


void Fishfarm::compute_current_sim_individual_mean_kg_in_farm(int tstep)
{

    double tstep_in_y=0.0;
    if(get_meanw_growth_model_type()=="((fulton_condition_factor/100000*Linf_mm^3)*(1-exp(-K_y*(tstep-t0_y)))^3)"){
        tstep_in_y= (tstep+1)/8764.0;
        sim_individual_mean_kg=((get_fulton_condition_factor()/100000*pow(get_Linf_mm(),3))*pow((1-exp(-get_K_y()*(tstep_in_y-get_t0_y()))),3));
        cout << "sim_individual_mean_kg on this farm is " << sim_individual_mean_kg << " at " << tstep_in_y << " given " << tstep <<  endl;
        dout(cout << "sim_individual_mean_kg on this farm is " << sim_individual_mean_kg << endl;)
    } else{
    cout << "sorry, not the expected growth model for fishfarm...." << endl;
    }




   // because input the model is not very flexible we wish to replace by a proper mathematical expression parser:



    /* QT?
     * string growth_model_expr=get_meanw_growth_model_type(); // we expect something like: "((fulton_condition_factor/100000*Linf_mm^3)*(1-exp(-K_y*(tstep-t0_y)))^3)"
    double Linf  =get_Linf_mm();
    double K     =get_K_y();
    double to    =get_t0_y();
    double q     =get_fulton_condition_factor();
    double t     =tstep/8764;

    QScriptEngine engine;
    engine.globalObject().setProperty("Linf_mm", Linf);
    engine.globalObject().setProperty("K_y", K);
    engine.globalObject().setProperty("t0_y", to);
    engine.globalObject().setProperty("fulton_condition_factor", q);
    engine.globalObject().setProperty("tstep", t);
    sim_individual_mean_kg= engine.evaluate(growth_model_expr).toNumber();
    */  // => QT not used for the standalone simulator!!



    // exprtk syntax?
/*
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>     expression_t;
    typedef exprtk::parser<double>             parser_t;

    double Linf  =get_Linf_mm();
    double K     =get_K_y();
    double to    =get_t0_y();
    double q     =get_fulton_condition_factor();
    double t     =tstep/8764;

    symbol_table_t symbol_table;
    symbol_table.add_variable("Linf_mm", Linf);
    symbol_table.add_variable("K_y", K);
    symbol_table.add_variable("t0_y", to);
    symbol_table.add_variable("fulton_condition_factor", q);
    symbol_table.add_variable("t0_y", t);
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(growth_model_expr, expression);

    sim_individual_mean_kg=expression.value();
   cout << "sim_individual_mean_kg on this farm is " << sim_individual_mean_kg << endl;

*/ // THIS LIBRARY IS TOO LARGE....too many sections error

}





void Fishfarm::compute_profit_in_farm()
{
    ;
}

void Fishfarm::compute_discharge_on_farm()
{
    ;
}







