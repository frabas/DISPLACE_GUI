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

#ifndef POPULATION_H
#define POPULATION_H

#include <commons_global.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "myutils.h"
#include "Node.h"
#include "Tac.h"
#include <options.h>

class  Population
{
	public:
        Population(int name,
                   string pop_name,
                   double _avai0_beta,
                   double _avai2_beta,
                   double _avai3_beta,
                   double _avai5_beta,
                   double _avai7_beta,
                   const vector<int> &selected_szgroups,
                   const vector<double> &init_tot_N_at_szgroup,
                   const vector<double> &init_prop_migrants_in_tot_N_at_szgroup,
                   const vector<double> &init_fecundity_at_szgroup,
                   vector<double> init_weight_at_szgroup,
                   const vector<int> &init_comcat_at_szgroup,
                   const vector<double> &init_maturity_at_szgroup,
                   vector<double> init_M_at_szgroup,
                   const vector<double> &init_proprecru_at_szgroup,
                   const vector<double> &param_sr,
                   const multimap<types::NodeId,double> &full_spatial_availability,
                   const multimap<types::NodeId, double> &field_of_coeff_diffusion_this_pop,
                   const map<types::NodeId,double> &oth_land,
                   const vector<map<types::NodeId, double> > &oth_land_map_per_met,
                   const multimap<int, double> &overall_migration_fluxes,
                   const map<string,double> &relative_stability_key,
                   const vector< vector<double> > &percent_szgroup_per_age_matrix,
                   const vector< vector<double> > &percent_age_perszgroup_matrix,
                   const vector< vector<double> > &growth_transition_matrix,
                   const vector<Node* > &nodes,
                   const vector<double> &fbar_ages_min_max,
                   const vector<double> &init_tac,
                   double tac_percent_simulated,
                   double hyperstability_param,
                   double a_calib_cpue_multiplier,
                   double a_calib_weight_at_szgroup);

    Population();

    virtual ~Population();

    // Copy constructor
    int get_name() const;

    string get_pop_name() const;

    void set_name(int nm);

    void set_pop_name(std::string nm);

    int get_nb_szgroups() const;

    int get_nb_ages() const;

    const vector<Node *> &get_list_nodes() const;

    const vector<int> &get_selected_szgroups() const;

    double get_avai0_beta() const;

    double get_avai2_beta() const;

    double get_avai3_beta() const;

    double get_avai5_beta() const;

    double get_avai7_beta() const;

    double get_cpue_multiplier() const;
        const vector<double>& get_tot_N_at_szgroup() const;
        const vector<double>& get_tot_C_at_szgroup() const;
        const vector<double>& get_tot_D_at_szgroup() const;
        const vector<double>& get_true_tot_N_at_szgroup() const;
        const vector<double>& get_prop_migrants_in_tot_N_at_szgroup() const;
        const vector<double>& get_tot_N_at_szgroup_just_after_redistribution() const;
        const vector<double>& get_tot_N_at_szgroup_month_minus_1() const;
        const vector<double>& get_tot_N_at_szgroup_year_minus_1() const;
        const vector<double>& get_a_tot_N_at_szgroup_before_applying_M() const;
        const vector<double>& get_tot_N_at_age() const;
        const vector<double>& get_perceived_tot_N_at_age() const;
        const vector<double>& get_tot_N_at_age_last_quarter() const;
        const vector<double>& get_tot_F_at_age() const;
        const vector<double>& get_tot_F_at_age_running_average() const;
        const vector<double>& get_tot_F_at_age_last_quarter() const;
        const vector<double>& get_perceived_tot_F_at_age() const;
        const vector<double>& get_tot_M_at_age() const;
        const vector<double>& get_tot_W_at_age() const;
        const vector<double>& get_tot_Mat_at_age() const;
        const vector<double>& get_fbar_ages_min_max() const;
        const vector<double>& get_maturity_at_szgroup() const;
        const vector<double>& get_fecundity_at_szgroup() const;
        const vector<double>& get_weight_at_szgroup() const;
		vector<int> get_comcat_at_szgroup() const;
        const vector<double>& get_M_at_szgroup() const;
        const vector<double>& get_proprecru_at_szgroup() const;
        const vector<double>& get_SSB_at_szgroup() const;
        double get_SSB() const;
        double get_fbar() const;
        double get_proportion_mature_fish() const;
        const vector<double> &get_FFmsy() const;
        const vector<double>& get_param_sr() const;
		vector< vector <double> >get_growth_transition_matrix() const;
		vector< vector <double> >get_percent_szgroup_per_age_matrix() const;
		vector< vector <double> >get_percent_age_per_szgroup_matrix() const;
        multimap<types::NodeId, double> get_full_spatial_availability() const;
        multimap<types::NodeId, double> get_field_of_coeff_diffusion_this_pop() const;
        multimap<int,double> get_overall_migration_fluxes() const;
        map<types::NodeId, double> get_oth_land() const;
        vector<map<types::NodeId, double> > get_oth_land_map_per_met() const;
        double get_hyperstability_param() const;
        double get_oth_land_multiplier() const;
        Tac* get_tac() const;
        double get_quota() const;
        double get_quota_uptake() const;
        int get_is_choking_fisheries() const;
        double get_landings_so_far() const;
        double get_tot_N_at_age0() const;
        const vector<double>& get_landings_at_end_of_years() const;
        void set_quota(double _quota);
        void set_quota_uptake(double _quota_uptake);
        void set_selected_szgroups(vector<int> selected_szgroups);
		void set_avai0_beta(double _avai0_beta);
		void set_avai2_beta(double _avai2_beta);
		void set_avai3_beta(double _avai3_beta);
		void set_avai5_beta(double _avai5_beta);
		void set_avai7_beta(double _avai7_beta);
		void set_cpue_multiplier(double _cpue_multiplier);
        void set_hyperstability_param(double _hyperstability_param);
        void set_tot_N_at_szgroup(const vector<double>& _N_at_szgroup);
        void set_tot_C_at_szgroup(const vector<double>& _C_at_szgroup);
        void set_tot_D_at_szgroup(const vector<double>& _D_at_szgroup);
        void set_true_tot_N_at_szgroup(const vector<double>& _true_tot_N_at_szgroup);
        void set_prop_migrants_in_tot_N_at_szgroup(const vector<double>& _prop_migrants_at_szgroup);
        void set_tot_N_at_szgroup_just_after_redistribution(const vector<double>& _N_at_szgroup_just_after_redistribution);
        void set_tot_N_at_szgroup_month_minus_1(const vector<double>& _N_at_szgroup_month_minus_1);
        void set_tot_N_at_szgroup_year_minus_1(const vector<double>& _N_at_szgroup_year_minus_1);
        void set_a_tot_N_at_szgroup_before_applying_M(const vector<double>& _a_tot_N_at_szgroup_before_applying_M);
        void set_tot_N_at_age(const vector<double>& _N_at_age);
        void set_perceived_tot_N_at_age(const vector<double>& _perceived_N_at_age);
        void set_tot_N_at_age_last_quarter(const vector<double>& _N_at_age_last_quarter);
        void set_tot_F_at_age(const vector<double>& _F_at_age);
        void set_tot_F_at_age_running_average(const vector<double>& _tot_F_at_age_running_average);
        void set_perceived_tot_F_at_age(const vector<double>& _perceived_F_at_age);
        void set_tot_F_at_age_last_quarter(const vector<double>& _tot_F_at_age_last_quarter);
        void set_tot_M_at_age(const vector<double>& _M_at_age);
        void set_tot_W_at_age(const vector<double>& _W_at_age);
        void set_tot_Mat_at_age(const vector<double>& _Mat_at_age);
        void set_adults_diet_preference_per_stock(const vector<double>& _adults_diet_preference_per_stock);
        void set_juveniles_diet_preference_per_stock(const vector<double>& _juveniles_diet_preference_per_stock);
        void set_maturity_at_szgroup(const vector<double>& _maturity_at_szgroup);
        void set_fecundity_at_szgroup(const vector<double>& _fecundity_at_szgroup);
        void set_weight_at_szgroup(const vector<double> &_weight_at_szgroup);
		void set_comcat_at_szgroup(vector <int> _comcat_at_szgroup);
        void set_M_at_szgroup(const vector<double>& _M_at_szgroup);
        void set_proprecru_at_szgroup(const vector<double>& _proprecru_at_szgroup);
        void set_SSB_at_szgroup(const vector<double>& _SSB_at_szgroup);
        void set_SSB(double _SSB);
        void set_fbar(double _fbar);
        void set_FFmsy(const vector<double>& _FFmsy);
        void set_is_choking_fisheries(int is_choking_fisheries);
        void set_proportion_mature_fish(double _proportion_mature_fish);
                                 // in weight...
		void set_landings_so_far(double _landings_so_far);
        void add_to_landings_at_end_of_years(double value);
        void set_param_sr(const vector<double>& _param_sr);
        void set_full_spatial_availability(multimap<types::NodeId, double> _full_spatial_availability);
        void set_field_of_coeff_diffusion_this_pop(multimap<types::NodeId, double> _field_of_coeff_diffusion_this_pop);
        void set_overall_migration_fluxes(multimap<int,double> _overall_migration_fluxes);
        void set_oth_land(map<types::NodeId,double> _oth_land);
        void set_oth_land_map_per_met (vector<map<types::NodeId, double> > _oth_land_map_per_met);
    	void set_oth_land_multiplier(double _a_multiplier);
		void set_list_nodes(vector<Node* > _list_nodes);
		void distribute_N();	 // split tot N among nodes
		void aggregate_N();		 // aggregate from nodes
								 // do a cumul for F
        void diffuse_N_from_field(adjacency_map_t& adjacency_map);
        void compute_tot_N_and_F_and_W_at_age(int a_month_i);
        void compute_tot_M_at_age();
		double compute_fbar();
        vector <double> compute_SSB();
        double compute_proportion_mature_fish();
        void clear_tot_F_at_age();
        void clear_tot_M_at_age();
        void clear_tot_C_at_szgroup();
        void clear_tot_D_at_szgroup();
        void do_growth();		 // apply the size transition matrix
								 // stock-recruitment relationship via fecundity_at_szgroup
        void apply_overall_migration_fluxes(vector<Population* >& populations);

        void add_recruits_from_eggs();
								 // stock-recruitment relationship via fecundity_at_szgroup
		void add_recruits_from_SR();
        void add_recruits_from_a_fixed_number(); // absolute nb of recruits taken from param[0]
        // monthly M on the entire pop (i.e. not spatially disaggregated)
		void apply_natural_mortality();
		void export_popdyn_N(ofstream& popdyn_N, int tstep);
		void export_popdyn_F(ofstream& popdyn_F, int tstep);
        void export_popdyn_SSB(ofstream& popdyn_SSB, int tstep);
        void export_popdyn_annual_indic(ofstream& popdyn_annual_indic, int tstep, const DynAllocOptions &dyn_alloc_sce);

	protected:
	private:
		int name;
        string pop_name;
        int idx_pop;
		int nb_szgroups;
        int nb_ages;
        vector<int> selected_szgroups;
		double avai0_beta;		 // actually, only one value here...
		double avai2_beta;		 //
		double avai3_beta;		 //
		double avai5_beta;		 //
		double avai7_beta;		 //
		double cpue_multiplier;	 //
								 //
		vector<double> tot_N_at_szgroup;
                                 //
        vector<double> tot_C_at_szgroup;
                                 //
        vector<double> tot_D_at_szgroup;

        vector<double> true_tot_N_at_szgroup; // same as tot_N_at_szgroup if pop is 100% of the same species (i.e. different if stock mixing occurs)
                                 //
        vector<double> prop_migrants_in_N_at_szgroup; //0 if pop is 100% of the same species (i.e. different if stock mixing occurs)
                                 //
        vector<double> tot_N_at_szgroup_just_after_redistribution;
								 //
		vector<double> tot_N_at_szgroup_month_minus_1;
								 //
		vector<double> tot_N_at_szgroup_year_minus_1;
								 //
        vector<double> a_tot_N_at_szgroup_before_applying_M;

		vector<double> tot_N_at_age;
        vector<double> perceived_tot_N_at_age;
                                 //
		vector<double> tot_F_at_age;
        vector<double> tot_F_at_age_running_average;
                                 //
        vector<double> perceived_tot_F_at_age;
        vector<double> tot_F_at_age_last_quarter;
								 //
		vector<double> tot_M_at_age;
								 //
		vector<double> tot_W_at_age;
								 //
        vector<double> tot_Mat_at_age;
                                 //
        vector<double> adults_diet_preference_per_stock;
                                 //
        vector<double> juveniles_diet_preference_per_stock;
                                 //
        vector<double> weight_at_szgroup;
								 //
		vector<int> comcat_at_szgroup;
								 //
		vector<double> maturity_at_szgroup;
								 //
		vector<double> fecundity_at_szgroup;
								 //
		vector<double> M_at_szgroup;
								 //
		vector<double> proprecru_at_szgroup;
        vector<double> SSB_at_szgroup;
        double SSB ,fbar;				 //
        vector<double>  FFmsy;				 //
        double proportion_mature_fish;				 //
        int is_choking_fisheries;
        vector<double> param_sr; //
								 // area distribution
		vector<Node* > list_nodes;
								 // for sharing the total N by node
        multimap<types::NodeId,double> full_spatial_availability;
        multimap<types::NodeId,double> field_of_coeff_diffusion_this_pop;
        multimap<int,double> overall_migration_fluxes;
        map<types::NodeId,double> oth_land;// for accounting for the depletion from non-described activities
        vector<map<types::NodeId, double> >oth_land_map_per_met; // other landings on this pop specified per met
		vector< vector<double> > percent_szgroup_per_age_matrix;
		vector< vector<double> > percent_age_per_szgroup_matrix;
		vector< vector<double> > growth_transition_matrix;
        double hyperstability_param;	 // power curve shape param for cpue~abundance relationship
        vector<double> fbar_ages_min_max;
		Tac *tac;
        double quota;
        double quota_uptake;
		double landings_so_far;	 // global landings, reinit each start of the year...
        vector <double> landings_at_end_of_years;
        double oth_land_multiplier; // init at 1. Will change according to the next TAC.
        double tot_N_at_age0;


};
#endif							 // POPULATION_H
