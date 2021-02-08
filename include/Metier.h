// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef METIER_H
#define METIER_H

#include <commons_global.h>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <map>

using namespace std;

class  Metier
{
	private:
        int name = -1;
        int type = -1;
        double percent_revenue_completeness; // for completing the revenue from other species....
		double target_factor;	 //NOT IN USE...
        vector< vector <double> > selectivity_per_stock;
        vector<double> betas_per_pop;
        vector<double> discardratio_limits;
        vector<int> is_avoided_stocks;  // 0 or 1 over stocks
        vector<int> mls_cat_per_pop;
        double fspeed;
        double gear_width_a;
		double gear_width_b;
		string gear_width_model;
		multimap<int, double> loss_after_1_passage;
        vector<int> metier_target_stocks;
        vector<int> metier_suitable_seabottomtypes;
   

	public:
		Metier();
        Metier(int name,
            int type,
            double percent_revenue_completeness,
            const vector< vector <double> > &selectivity_per_stock_ogives,
            vector<double> metier_betas_per_pop,
            vector<double> discardratio_limits,
            vector<int> is_avoided_stocks,
            vector<int> metier_mls_cat_per_pop,
            double fspeed,
            double gear_width_a,
            double gear_width_b,
            string gear_width_model,
            multimap<int, double> loss_after_1_passage,
            vector<int> _metier_target_stocks,
            vector<int> _metier_suitable_seabottomtypes);
		~Metier();
		int get_name();
        void set_name(int name);
		int get_metier_type();
        double get_percent_revenue_completeness();
		double get_target_factor();
        //vector<double> get_selectivity_ogive();
        vector<vector <double> > get_selectivity_per_stock_ogives() const;
        //vector<double> get_discards_ogive();
        double  get_fspeed();
        double  get_gear_width_a();
		double  get_gear_width_b();
		string  get_gear_width_model();
		multimap<int, double>  get_loss_after_1_passage();
        vector<int> get_metier_target_stocks();
        vector<int> get_metier_suitable_seabottomtypes();
        vector<double> get_betas_per_pop();
        vector<double> get_discardratio_limits();
        vector<int> get_is_avoided_stocks();
        int get_is_avoided_stocks(int stk);
        vector<int> get_mls_cat_per_pop();
        void set_betas_per_pop(vector<double> metier_betas_per_pop);
        void set_discardratio_limits(vector<double> metier_discardratio_limits);
        void set_is_avoided_stocks(vector<int> metier_is_avoided_stocks);
        void set_mls_cat_per_pop(vector<int> metier_mls_cat_per_pop);
   
};
#endif							 // METIER_H
