// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef FISHFARM_H
#define FISHFARM_H

#include <commons_global.h>

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "myutils.h"
#include "Node.h"

class  Fishfarm
{
    public:
        Fishfarm(Node *node, int id, string name, int farmtype);

        Fishfarm(int _name, string _stringname, Node *_node, int farmtype, int _is_active, double _size, double _farm_original_long, double _farm_original_lat,
                 double _mean_SST, double _mean_salinity, double _mean_windspeed, double _mean_currentspeed, double _max_depth, double _diss_O2_mg_per_l,
                 double _Linf_mm, double _K_y, double _t0_y, double _fulton_condition_factor, string _meanw_growth_model_type,
                 int _start_day_growing, int _end_day_harvest, int _nbyears_for_growth, int _nb_days_fallowing_period,
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
                 double _market_price_sold_fish, double _operating_cost_per_day, double _annual_profit
                 );

        Fishfarm();
        virtual ~Fishfarm();
                                 // Copy constructor
        int get_name() const;
        string get_stringname() const;
        Node* get_loc_ff() const;

        int get_farmtype() const;

        int get_is_active() const;
        int get_is_running() const;

        double get_size() const;
        double get_farm_original_long() const;
        double get_farm_original_lat() const;
        double get_x() const;
        double get_y() const;

        double get_Linf_mm() const; // variables for growth modelling
        double get_K_y() const;
        double get_t0_y() const;
        double get_fulton_condition_factor() const;
        string get_meanw_growth_model_type() const;
        double get_meanw_at_start() const;

        double get_sim_individual_mean_kg() const; // for tracking
        double get_sim_previous_individual_mean_kg() const; // for tracking
        double get_sim_kg_harvested() const;
        double get_sim_kg_eggs_harvested() const;
        double get_sim_annual_profit() const;
        double get_sim_net_discharge_N() const;
        double get_sim_net_discharge_P() const;
        double get_sim_net_discharge_C() const;
        double get_sim_net_discharge_medecine() const;
        double get_sim_cumul_net_discharge_N() const;
        double get_sim_cumul_net_discharge_P() const;
        double get_sim_cumul_net_discharge_C() const;
        double get_sim_cumul_net_discharge_medecine() const;

        double get_start_day_growing() const; // for main-loop
        double get_end_day_harvest() const;


        double get_prop_harvest_kg_sold() const; // for economic model
        double get_nb_fish_at_harvest() const;
        double get_market_price_sold_fish() const;
        double get_nb_fish_at_start() const;
        double get_price_per_kg_at_start() const;
        double get_operating_cost_per_day() const;
        double get_kg_eggs_per_kg() const;
        double get_price_eggs_per_kg() const;
        double get_feed_price_per_kg() const;
        double get_total_feed_kg() const;
        double get_feed_vet_price_per_kg() const;
        double get_total_feed_vet_kg() const;
        int get_nbyears_for_growth() const;

        double get_total_feed_N_kg() const;
        double get_total_feed_vet_N_kg() const;
        double get_total_feed_P_kg() const;
        double get_total_feed_vet_P_kg() const;
        double get_N_in_fish_kg_3per() const;
        double get_P_in_fish_kg_0_5per() const;
        double get_prop_N_in_feed() const;
        double get_prop_P_in_feed() const;
        double get_prop_N_in_feed_vet() const;
        double get_prop_P_in_feed_vet() const;

        void set_x(double value);
        void set_y(double value);

        void set_is_active(int value);
        void set_is_running(int value);

        void set_sim_individual_mean_kg(double value);
        void set_sim_kg_harvested(double value);
        void set_sim_kg_eggs_harvested(double value);
        void set_sim_annual_profit(double value);
        void set_sim_net_discharge_N(double value);
        void set_sim_net_discharge_P(double value);
        void set_sim_net_discharge_C(double value);
        void set_sim_net_discharge_medecine(double value);
        void set_sim_cumul_net_discharge_N(double value);
        void set_sim_cumul_net_discharge_P(double value);
        void set_sim_cumul_net_discharge_C(double value);
        void set_sim_cumul_net_discharge_medecine(double value);


        void compute_current_sim_individual_mean_kg_in_farm(int tstep, double a_year);  // methods for growth modelling
        void compute_profit_in_farm();  // methods for fishfarm economy
        void compute_discharge_on_farm(int tstep);  // methods for envirnmental impact on node

        void export_fishfarms_indicators(ofstream& fishfarmlogs, int tstep);


    protected:
    private:
        int name;
        string stringname;
        Node* p_location_ff;

        int farmtype;

        int is_active;
        int is_running;

        double farm_original_long, farm_original_lat;
        double x, y;
        double size;
        double mean_SST, mean_salinity, mean_windspeed, mean_currentspeed, max_depth, diss_O2_mg_per_l;
        double Linf_mm,  K_y, t0_y,  fulton_condition_factor;
        string meanw_growth_model_type;
        int start_day_growing, end_day_harvest, nbyears_for_growth, nb_days_fallowing_period, nb_fish_at_start, nb_fish_at_harvest;
        double meanw_at_start, price_per_kg_at_start, target_meanw_at_harvest, meanw_at_harvest;
        double prop_harvest_kg_sold, kg_eggs_per_kg, price_eggs_per_kg;
        double N_in_fish_kg_3per, P_in_fish_kg_0_5per;
        string feed_types;
        double feed_price_per_kg, total_feed_kg, prop_N_in_feed, prop_P_in_feed;
        double total_feed_N_kg, total_feed_P_kg;
        string feed_type_vet;
        double feed_vet_price_per_kg, total_feed_vet_kg, prop_N_in_feed_vet, prop_P_in_feed_vet;
        double total_feed_vet_N_kg, total_feed_vet_P_kg;
        double annual_discharge_N_kg, annual_discharge_P_kg;
        double annual_discharge_C_kg, annual_discharge_heavymetals_kg;
        double annual_discharge_medecine_kg, net_harvest_kg_per_sqkm_y;
        double market_price_sold_fish, operating_cost_per_day, annual_profit;

        // tracked data
        double sim_previous_individual_mean_kg, sim_individual_mean_kg;
        double sim_kg_harvested;
        double sim_kg_eggs_harvested;
        double sim_annual_profit;
        double sim_net_discharge_N, sim_net_discharge_P, sim_net_discharge_C, sim_net_discharge_medecine;
        double sim_cumul_net_discharge_N, sim_cumul_net_discharge_P, sim_cumul_net_discharge_C, sim_cumul_net_discharge_medecine;



};
#endif							 // FISHFARM_H
