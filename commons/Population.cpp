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

#include <string>
#include <algorithm>
#include <random>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Population.h"
#include <helpers.h>

static std::random_device rd;
static std::mt19937 g(rd());



Population::Population(int a_name,
                       string a_pop_name,
                       double _avai0_beta,
                       double _avai2_beta,
                       double _avai3_beta,
                       double _avai5_beta,
                       double _avai7_beta,
                       const vector<int> &_selected_szgroups,
                       const vector<double> &init_tot_N_at_szgroup,
                       const vector<double> &init_prop_migrants_in_tot_N_at_szgroup,
                       const vector<double> &init_fecundity_at_szgroup,
                       vector<double> init_weight_at_szgroup,
                       const vector<int> &init_comcat_at_szgroup,
                       const vector<double> &init_maturity_at_szgroup,
                       vector<double> init_M_at_szgroup,
                       const vector<double> &init_proprecru_at_szgroup,
                       const vector<double> &_param_sr,
                       const vector<double> &_selectivity_per_stock_ogives_for_oth_land,
                       const multimap<types::NodeId, double> &_full_spatial_availability,
                       const multimap<types::NodeId, double> &field_of_coeff_diffusion_this_pop,
                       const map<int, int>& nbhours_for_distance_internodes_this_pop, // a map with one key for now, just in case we´d like to expand to szgroups in the future...
                       const map<types::NodeId, double> &_oth_land,
                       const vector<map<types::NodeId, double> >& _oth_land_map_per_met,
                       const multimap<int, double> &overall_migration_fluxes,
                       const map<string, double> &relative_stability_key,
                       const map<int, double>& percent_tac_per_vessel_length_class,   
                       const map<int, double> & percent_tac_cumul_over_months_key,
                       const vector<vector<double> > &_percent_szgroup_per_age_matrix,
                       const vector<vector<double> > &_percent_age_per_szgroup_matrix,
                       const vector<vector<double> > &_growth_transition_matrix,
                       const vector<Node *> &nodes,
                       const vector<double> &_fbar_ages_min_max,
                       const vector<double> &init_tac,
                       double tac_percent_simulated,
                       double hyperstability_param,
                       double a_calib_cpue_multiplier,
                       double a_calib_weight_at_szgroup)
{
  //  UNUSED(lst_idx_nodes_per_pop);
  //  UNUSED(landings_so_far);

    name=a_name;
    pop_name=a_pop_name;

    dout(cout << "create pop " << name << "\n");

	full_spatial_availability= _full_spatial_availability;

	oth_land= _oth_land;
    oth_land_map_per_met=_oth_land_map_per_met;

	growth_transition_matrix=_growth_transition_matrix;
	percent_szgroup_per_age_matrix=_percent_szgroup_per_age_matrix;
	percent_age_per_szgroup_matrix=_percent_age_per_szgroup_matrix;

    nb_szgroups=init_tot_N_at_szgroup.size();
    nb_ages=percent_szgroup_per_age_matrix[0].size();

	param_sr=_param_sr;
    selectivity_per_stock_ogives_for_oth_land = _selectivity_per_stock_ogives_for_oth_land;

    this->set_landings_so_far(0.0);

	// init...
    dout(cout << "init..." << name << "\n");

    // a quick check
    if(init_tot_N_at_szgroup.empty() || init_tot_N_at_szgroup.size()==0)
     {
        cout << "No N_at_szgroup found!! Check init_pops_per_szgroup. dat input file, correct and re-run..." << "\n";
        int aa;
        cin >> aa;
     }

    for(unsigned int sz=0; sz<nb_szgroups; sz++)
	{



        tot_N_at_szgroup.push_back(0);
        a_tot_N_at_szgroup_before_applying_M.push_back(0);
        tot_C_at_szgroup.push_back(0);
        tot_D_at_szgroup.push_back(0);
        true_tot_N_at_szgroup.push_back(0);
        prop_migrants_in_N_at_szgroup.push_back(0);
        tot_N_at_szgroup_just_after_redistribution.push_back(0);
		tot_N_at_szgroup_month_minus_1.push_back(0);
		tot_N_at_szgroup_year_minus_1.push_back(0);
		fecundity_at_szgroup.push_back(0);
		weight_at_szgroup.push_back(0);
		comcat_at_szgroup.push_back(0);
		maturity_at_szgroup.push_back(0);
		M_at_szgroup.push_back(0);
        beta_ssm_at_szgroup.push_back(0);
        background_mortality_at_szgroup.push_back(0);
        proprecru_at_szgroup.push_back(0);
        SSB_at_szgroup.push_back(0);

	}
    for(unsigned int a=0; a<nb_ages; a++)
	{
		tot_F_at_age.push_back(0);
        tot_F_at_age_running_average.push_back(0);
        tot_M_at_age.push_back(0);
        FFmsy.push_back(0);
    }
    for(unsigned int a=0; a<nb_ages; a++)
	{
		tot_F_at_age_last_quarter.push_back(0);
	}

    // for catch equation
    dout(cout << "loading parameters for the catch equation" << name << "\n");
                                 // for the catch equation
	this->set_selected_szgroups(_selected_szgroups);

	// ...then fill in with avai0_beta
                                 // set the pop-specific beta from glm for szgroup selected id0
	this->set_avai0_beta(_avai0_beta);

    // ...then fill in with avai2_beta
                                 // set the pop-specific beta from glm for szgroup selected id1
	this->set_avai2_beta(_avai2_beta);

    // ...then fill in with avai3_beta
                                 // set the pop-specific beta from glm for szgroup selected id2
	this->set_avai3_beta(_avai3_beta);

    // ...then fill in with avai5_beta
                                 // set the pop-specific beta from glm for szgroup selected id3
	this->set_avai5_beta(_avai5_beta);

    // ...then fill in with avai7_beta
                                 // set the pop-specific beta from glm for szgroup selected id4
	this->set_avai7_beta(_avai7_beta);

    dout(cout << "calib..." << name << "\n");
    // CALIB: if increased then improve the catch rate of the simulated vessels....
    // look at the config.dat for calibration values

								 // set to 1 the first year
	this->set_cpue_multiplier(1*a_calib_cpue_multiplier);

	// ...then fill in with start pop
    dout(cout << "set the overall N_at_szgroup..." << name << "\n");
                                 // set the overall N_at_szgroup
	this->set_tot_N_at_szgroup(init_tot_N_at_szgroup);

    // ...then fill in with start pop
                                 // set the overall N_at_szgroup
    this->set_true_tot_N_at_szgroup(init_tot_N_at_szgroup);

    // ...then fill in with start info
                                 // set the migrant prop
    dout(cout << "set the prop_migrants..." << name << "\n");
    this->set_prop_migrants_in_tot_N_at_szgroup(init_prop_migrants_in_tot_N_at_szgroup);

    // ...then fill in with start pop
                                 // set the overall N_at_szgroup
	this->set_tot_N_at_szgroup_just_after_redistribution(init_tot_N_at_szgroup);

	// ...then fill in with start pop also for minus 1
								 // set the overall N_at_szgroup
	this->set_tot_N_at_szgroup_month_minus_1(init_tot_N_at_szgroup);

	// ...then fill in with start pop also for minus 1
								 // set the overall N_at_szgroup
	this->set_tot_N_at_szgroup_year_minus_1(init_tot_N_at_szgroup);

	// ...then fill in with start pop
	// CALIB: if decreased then smaller fish then higher F because numbers of fish for a given TAC increased....
    // look at the config.dat for calibration values

    dout(cout << "calib the weight at szgroup..." << name << "\n");
    for(unsigned int i=0; i < nb_szgroups; i++)
    {
        init_weight_at_szgroup.at(i)=init_weight_at_szgroup.at(i)*a_calib_weight_at_szgroup;
	}

	this->set_weight_at_szgroup(init_weight_at_szgroup);

    dout(cout << "fill in with start pop..." << name << "\n");
    // ...then fill in with start pop
    dout(cout << "for comcat..." << name << "\n");
    this->set_comcat_at_szgroup(init_comcat_at_szgroup);

	// ...then fill in with start pop
    dout(cout << "for maturity..." << name << "\n");
    this->set_maturity_at_szgroup(init_maturity_at_szgroup);

	// ...then fill in with start pop
    dout(cout << "for fecundity..." << name << "\n");
    this->set_fecundity_at_szgroup(init_fecundity_at_szgroup);

	// ...then fill in with start pop
    dout(cout << "for natural mortality..." << name << "\n");
  	this->set_M_at_szgroup(init_M_at_szgroup);

	// ...then fill in with start pop
    dout(cout << "for prop recru..." << name << "\n");
    this->set_proprecru_at_szgroup(init_proprecru_at_szgroup);

	// fill in the list of nodes specific to this particular pop
	/*   vector<Node* > p_spe_nodes;
	   multimap<string,int>::iterator lower_pop = lst_idx_nodes_per_pop.lower_bound(a_name);
	   multimap<string,int>::iterator upper_pop = lst_idx_nodes_per_pop.upper_bound(a_name);
	   for (multimap<string, int>::iterator a_pos=lower_pop; a_pos != upper_pop; a_pos++)
	   {
		   p_spe_nodes.push_back (nodes[  a_pos->second  ]);
		   // put the name of the pop on the pop-spe nodes at the mean time...
		   nodes[  a_pos->second  ]->set_pop_names_on_node(a_name);
	   }
	   for(int i=0; i<p_spe_nodes.size(); i++)
		   list_nodes.push_back(p_spe_nodes[i]);
	*/
	// REPLACE BY: (TO ONLY USE THE NODES LISTED IN THE AVAI FILE...)
    dout(cout << "set up the list of nodes for this pop" << name << "\n");

    vector<Node* > p_spe_nodes;
    for(auto iter=full_spatial_availability.begin(); iter != full_spatial_availability.end();
		iter = full_spatial_availability.upper_bound( iter->first ) )
	{
        //cout << iter->first.toIndex() <<" : " << iter->second << " - ";

        dout(if(iter->first.toIndex()>nodes.size()) cout << "error in the avai field: trying to push back node idx " << iter->first.toIndex() << " for max nb of nodes " << nodes.size() << "\n");
        p_spe_nodes.push_back (nodes[  iter->first.toIndex()  ]);
        nodes[ iter->first.toIndex() ]->set_pop_names_on_node(a_name);

   }
    for(unsigned int i=0; i<p_spe_nodes.size(); i++)
		list_nodes.push_back(p_spe_nodes[i]);

	// add these Ns to the multimap of the concerned nodes
    dout(cout << " lst nodes: " << "\n");
	for(unsigned int i=0; i<list_nodes.size(); i++)
	{
    //cout  << list_nodes.at(i)->get_idx_node().toIndex() << " ";
        // caution: here is tot N on the node! need to call distribute_N()
		list_nodes.at(i)->set_Ns_pops_at_szgroup(a_name, tot_N_at_szgroup);
        dout(cout  << list_nodes.at(i)->get_idx_node().toIndex() << " ");
	}

    dout(cout << "\n");


    dout(cout << "field_of_coeff_diffusion_this_pop " << name << "\n");

    // for diffusion of N per szgroup
    this->set_field_of_coeff_diffusion_this_pop(field_of_coeff_diffusion_this_pop); // a fraction from 0 to 1
    this->set_nbhours_for_distance_internodes_this_pop(nbhours_for_distance_internodes_this_pop); // a nb of hours to set the pace of the diffusion
    
    // After `full_spatial_availability` has been populated:
    build_availability_cache();

	// distribute tot_N_at_szgroup on nodes knowing the avai spatial key
	// i.e. update the multimap Ns_pops_at_szgroup of the nodes
	this->distribute_N();		 // divide on nodes according to avai

	// init the N on node at month start
	for(unsigned int i=0; i<list_nodes.size(); i++)
	{
		vector<double> N_at_szgroup=list_nodes.at(i)->get_Ns_pops_at_szgroup(a_name);
		list_nodes.at(i)->set_Ns_pops_at_szgroup_at_month_start(name, N_at_szgroup);

	}

    dout(cout << "init tac " << name << "\n");

    // init tac
	tac = new Tac(init_tac[0], tac_percent_simulated, relative_stability_key, 
                    percent_tac_per_vessel_length_class, percent_tac_cumul_over_months_key);
    fbar_type1 = 0.0;
    fbar_type2 = 0.0;
    oth_land_multiplier=1.0;

    for (auto it = relative_stability_key.begin(); it != relative_stability_key.end(); it++)
    {
        landings_so_far_per_nation.insert(std::make_pair(it->first, 0));
    }
    
    for (auto it2 = percent_tac_per_vessel_length_class.begin(); it2 != percent_tac_per_vessel_length_class.end(); it2++)
    {
        landings_so_far_per_vessel_length_class.insert(std::make_pair(it2->first, 0));
    }

    // check
    //cout << "check relative_stability_key " << "\n";
    //for (auto elem : relative_stability_key)
    //{
    //    std::cout << elem.first << " " << elem.second << "\n";
    //}

    // check
    //cout << "check landings_so_far_per_nation " << "\n";
    //for (auto elem : landings_so_far_per_nation)
    //{
    //    std::cout << elem.first << " " << elem.second << "\n";
    //}



    quota =init_tac[0];
    quota_uptake=0.0;
    is_choking_fisheries=0;

	// init related to F
	fbar_ages_min_max=_fbar_ages_min_max;

    // other MSFD D3
    proportion_mature_fish=1.0;

    // hyperstability
    this->set_hyperstability_param(hyperstability_param);

    int nbsz = percent_szgroup_per_age_matrix.size();
    int nbages = percent_szgroup_per_age_matrix[0].size();


    // will be of use for TAC STF
    tot_N_at_age0 =0.0;
    for(int sz = 0; sz < nbsz; sz++)
    {
        tot_N_at_age0 +=  percent_szgroup_per_age_matrix[sz][0] * tot_N_at_szgroup[sz];
        cout << "pop "<< this->get_name() << ": tot_N_at_szgroup[" << sz <<"] is " << tot_N_at_szgroup.at(sz) << "\n";
        cout << "pop "<< this->get_name() << ": percent_szgroup_per_age_matrix[" <<sz<< "][0] is " << percent_szgroup_per_age_matrix[sz][0] << "\n";
    }
    cout << "pop "<< this->get_name() << ": tot_N_at_age0 is " << tot_N_at_age0 << "\n";


}


Population::Population()
{
	//ctor
}


Population::~Population()
{
	//dtor
}

int Population::get_name() const
{
    return (name);
}

string Population::get_pop_name() const
{
    return (pop_name);
}

void Population::set_name(int nm)
{
    name = nm;
}

void Population::set_pop_name(std::string nm)
{
    pop_name = std::move(nm);
}

int Population::get_nb_szgroups() const
{
    return (nb_szgroups);
}

int Population::get_nb_ages() const
{
    return (nb_ages);
}


const vector<Node* > &Population::get_list_nodes() const
{
	return(list_nodes);
}


const vector<int> &Population::get_selected_szgroups() const
{
	return(selected_szgroups);
}


double Population::get_avai0_beta() const
{
	return(avai0_beta);
}


double Population::get_avai2_beta() const
{
	return(avai2_beta);
}


double Population::get_avai3_beta() const
{
	return(avai3_beta);
}


double Population::get_avai5_beta() const
{
	return(avai5_beta);
}


double Population::get_cpue_multiplier() const
{
	return(cpue_multiplier);
}


double Population::get_avai7_beta() const
{
	return(avai7_beta);
}


const vector<double>& Population::get_tot_N_at_szgroup() const
{
	return(tot_N_at_szgroup);
}

const vector<double>& Population::get_tot_C_at_szgroup() const
{
    return(tot_C_at_szgroup);
}

const vector<double>& Population::get_tot_D_at_szgroup() const
{
    return(tot_D_at_szgroup);
}

const vector<double>& Population::get_true_tot_N_at_szgroup() const
{
    return(true_tot_N_at_szgroup);
}

const vector<double>& Population::get_prop_migrants_in_tot_N_at_szgroup() const
{
    return(prop_migrants_in_N_at_szgroup);
}

const vector<double>& Population::get_tot_N_at_szgroup_just_after_redistribution() const
{
	return(tot_N_at_szgroup_just_after_redistribution);
}


const vector<double>& Population::get_tot_N_at_szgroup_month_minus_1() const
{
	return(tot_N_at_szgroup_month_minus_1);
}


const vector<double>& Population::get_tot_N_at_szgroup_year_minus_1() const
{
	return(tot_N_at_szgroup_year_minus_1);
}

const vector<double>& Population::get_a_tot_N_at_szgroup_before_applying_M() const
{
    return(a_tot_N_at_szgroup_before_applying_M);
}

const vector<double>& Population::get_tot_N_at_age() const
{
    return(tot_N_at_age);
}

const vector<double>& Population::get_tot_F_at_age() const
{
	return(tot_F_at_age);
}

const vector<double>& Population::get_tot_F_at_age_running_average() const
{
    return(tot_F_at_age_running_average);
}


const vector<double>& Population::get_perceived_tot_F_at_age() const
{
    return(perceived_tot_F_at_age);
}


const vector<double>& Population::get_tot_F_at_age_last_quarter() const
{
	return(tot_F_at_age_last_quarter);
}


const vector<double>& Population::get_perceived_tot_N_at_age() const
{
    return(perceived_tot_N_at_age);
}


const vector<double>& Population::get_tot_M_at_age() const
{
	return(tot_M_at_age);
}


const vector<double>& Population::get_tot_W_at_age() const
{
	return(tot_W_at_age);
}

const vector<double>& Population::get_tot_Mat_at_age() const
{
    return(tot_Mat_at_age);
}

const vector<double>& Population::get_fbar_ages_min_max() const
{
	return(fbar_ages_min_max);
}


const vector<double>& Population::get_weight_at_szgroup() const
{
	return(weight_at_szgroup);
}


vector<int> Population::get_comcat_at_szgroup() const
{
	return(comcat_at_szgroup);
}


const vector<double>& Population::get_maturity_at_szgroup() const
{
	return(maturity_at_szgroup);
}


const vector<double>& Population::get_fecundity_at_szgroup() const
{
	return(fecundity_at_szgroup);
}


const vector<double>& Population::get_M_at_szgroup() const
{
	return(M_at_szgroup);
}

const vector<double>& Population::get_beta_ssm_at_szgroup() const
{
    return(beta_ssm_at_szgroup);
}

const vector<double>& Population::get_background_mortality_at_szgroup() const
{
    return(background_mortality_at_szgroup);
}

const vector<double>& Population::get_proprecru_at_szgroup() const
{
	return(proprecru_at_szgroup);
}

const vector<double>& Population::get_SSB_at_szgroup() const
{
    return(SSB_at_szgroup);
}

double Population::get_SSB() const
{
	return(SSB);
}


double Population::get_fbar_type1() const
{
    return(fbar_type1);
}

double Population::get_fbar_type2() const
{
    return(fbar_type2);
}

const vector<double>& Population::get_FFmsy() const
{
    return(FFmsy);
}

double Population::get_proportion_mature_fish() const
{
    return(proportion_mature_fish);
}

double Population::get_landings_so_far() const
{
	return(landings_so_far);
}

map<string,double> Population::get_landings_so_far_per_nation() 
{
    return(landings_so_far_per_nation);
}

map<int, double> Population::get_landings_so_far_per_vessel_length_class()
{
    return(landings_so_far_per_vessel_length_class);
}


double Population::get_tot_N_at_age0() const
{
    return(tot_N_at_age0);
}


const vector<double>& Population::get_landings_at_end_of_years() const
{
    return(landings_at_end_of_years);
}


const vector<double>& Population::get_param_sr() const
{
	return(param_sr);
}

const vector<double>& Population::get_selectivity_per_stock_ogives_for_oth_land() const
{
    return(selectivity_per_stock_ogives_for_oth_land);
}


vector< vector <double> > Population::get_growth_transition_matrix() const
{
	return(growth_transition_matrix);
}


vector< vector <double> > Population::get_percent_szgroup_per_age_matrix() const
{
	return(percent_szgroup_per_age_matrix);
}


vector< vector <double> > Population::get_percent_age_per_szgroup_matrix() const
{
	return(percent_age_per_szgroup_matrix);
}


multimap<types::NodeId,double>  Population::get_full_spatial_availability() const
{
	return(full_spatial_availability);
}


multimap<types::NodeId,double>  Population::get_field_of_coeff_diffusion_this_pop() const
{
    return(field_of_coeff_diffusion_this_pop);
}

map<int, int>  Population::get_nbhours_for_distance_internodes_this_pop() const
{
    return(nbhours_for_distance_internodes_this_pop);
}



multimap<int,double>  Population::get_overall_migration_fluxes() const
{
    return(overall_migration_fluxes);
}



map<types::NodeId,double>  Population::get_oth_land() const
{
	return(oth_land);
}

vector<map<types::NodeId, double> >   Population::get_oth_land_map_per_met() const
{
    return(oth_land_map_per_met);
}


double  Population::get_oth_land_multiplier() const
{
	return(oth_land_multiplier);
}

double  Population::get_hyperstability_param() const
{
    return(hyperstability_param);
}


Tac* Population::get_tac() const
{
	return(tac);
}

double Population::get_quota() const
{
    return(quota);
}

double Population::get_quota_uptake() const
{
    return(quota_uptake);
}

int Population::get_is_choking_fisheries() const
{
    return(is_choking_fisheries);
}

void Population::set_quota(double _quota)
{
    quota=_quota;
}

void Population::set_quota_uptake(double _quota_uptake)
{
    quota_uptake=_quota_uptake;
}

void Population::set_selected_szgroups(vector<int> _selected_szgroups)
{
	selected_szgroups =_selected_szgroups;

}


void Population::set_avai0_beta(double _avai0_beta)
{
	avai0_beta =_avai0_beta;

}


void Population::set_avai2_beta(double _avai2_beta)
{
	avai2_beta =_avai2_beta;

}


void Population::set_avai3_beta(double _avai3_beta)
{
	avai3_beta =_avai3_beta;

}


void Population::set_avai5_beta(double _avai5_beta)
{
	avai5_beta =_avai5_beta;

}


void Population::set_avai7_beta(double _avai7_beta)
{

	avai7_beta =_avai7_beta;

}


void Population::set_cpue_multiplier(double _cpue_multiplier)
{

	cpue_multiplier =_cpue_multiplier;

}

void Population::set_hyperstability_param(double _hyperstability_param)
{

    hyperstability_param =_hyperstability_param;

}


void Population::set_tot_N_at_szgroup(const vector<double>& _N_at_szgroup)
{
	tot_N_at_szgroup =_N_at_szgroup;

}

void Population::set_tot_C_at_szgroup(const vector<double>& _C_at_szgroup)
{
    tot_C_at_szgroup =_C_at_szgroup;

}

void Population::set_tot_D_at_szgroup(const vector<double>& _D_at_szgroup)
{
    tot_D_at_szgroup =_D_at_szgroup;

}

void Population::set_true_tot_N_at_szgroup(const vector<double>& _true_tot_N_at_szgroup)
{
    true_tot_N_at_szgroup =_true_tot_N_at_szgroup;

}

void Population::set_prop_migrants_in_tot_N_at_szgroup(const vector<double>& _prop_migrants_in_N_at_szgroup)
{
    prop_migrants_in_N_at_szgroup =_prop_migrants_in_N_at_szgroup;

}


void Population::set_tot_N_at_szgroup_just_after_redistribution(const vector<double>& _N_at_szgroup_just_after_redistribution)
{
	tot_N_at_szgroup_just_after_redistribution =_N_at_szgroup_just_after_redistribution;

}


void Population::set_tot_N_at_szgroup_month_minus_1(const vector<double>& _N_at_szgroup_month_minus_1)
{
	tot_N_at_szgroup_month_minus_1 =_N_at_szgroup_month_minus_1;

}


void Population::set_tot_N_at_szgroup_year_minus_1(const vector<double>& _N_at_szgroup_year_minus_1)
{
	tot_N_at_szgroup_year_minus_1 =_N_at_szgroup_year_minus_1;

}

void Population::set_a_tot_N_at_szgroup_before_applying_M(const vector<double>& _a_tot_N_at_szgroup_before_applying_M)
{
    a_tot_N_at_szgroup_before_applying_M =_a_tot_N_at_szgroup_before_applying_M;

}



void Population::set_tot_N_at_age(const vector<double>& _tot_N_at_age)
{
	tot_N_at_age =_tot_N_at_age;

}


void Population::set_tot_F_at_age(const vector<double>& _tot_F_at_age)
{
	tot_F_at_age =_tot_F_at_age;

}

void Population::set_tot_F_at_age_running_average(const vector<double>& _tot_F_at_age_running_average)
{
    tot_F_at_age_running_average =_tot_F_at_age_running_average;

}



void Population::set_perceived_tot_N_at_age(const vector<double>& _perceived_N_at_age)
{
    perceived_tot_N_at_age =_perceived_N_at_age;

}


void Population::set_perceived_tot_F_at_age(const vector<double>& _perceived_F_at_age)
{
    perceived_tot_F_at_age =_perceived_F_at_age;

}


void Population::set_tot_F_at_age_last_quarter(const vector<double>& _tot_F_at_age_last_quarter)
{
	tot_F_at_age_last_quarter =_tot_F_at_age_last_quarter;

}


void Population::set_tot_M_at_age(const vector<double>& _tot_M_at_age)
{
	tot_M_at_age =_tot_M_at_age;

}


void Population::set_tot_W_at_age(const vector<double>& _tot_W_at_age)
{
	tot_W_at_age =_tot_W_at_age;

}

void Population::set_tot_Mat_at_age(const vector<double>& _tot_Mat_at_age)
{
    tot_Mat_at_age =_tot_Mat_at_age;

}


void Population::set_adults_diet_preference_per_stock(const vector<double>& _adults_diet_preference_per_stock)
{
    adults_diet_preference_per_stock =_adults_diet_preference_per_stock;

}

void Population::set_juveniles_diet_preference_per_stock(const vector<double>& _juveniles_diet_preference_per_stock)
{
   juveniles_diet_preference_per_stock =_juveniles_diet_preference_per_stock;

}


void Population::set_maturity_at_szgroup(const vector<double>& _maturity_at_szgroup)
{
	for(unsigned int sz =0; sz<_maturity_at_szgroup.size(); sz++)
	{
		maturity_at_szgroup[sz] =_maturity_at_szgroup[sz];
	}

}


void Population::set_fecundity_at_szgroup(const vector<double>& _fecundity_at_szgroup)
{
	for(unsigned int sz =0; sz<_fecundity_at_szgroup.size(); sz++)
	{
		fecundity_at_szgroup[sz] =_fecundity_at_szgroup[sz];
	}

}


void Population::set_weight_at_szgroup(const vector<double> &_weight_at_szgroup)
{

	for(unsigned int sz =0; sz<_weight_at_szgroup.size(); sz++)
	{
		weight_at_szgroup[sz] =_weight_at_szgroup[sz];
	}

}


void Population::set_comcat_at_szgroup(vector<int> _comcat_at_szgroup)
{

	for(unsigned int sz =0; sz<_comcat_at_szgroup.size(); sz++)
	{
		comcat_at_szgroup[sz] =_comcat_at_szgroup[sz];
	}

}


void Population::set_M_at_szgroup(const vector<double>& _M_at_szgroup)
{
	for(unsigned int sz =0; sz<_M_at_szgroup.size(); sz++)
	{
		M_at_szgroup[sz] =_M_at_szgroup[sz];
	}

}


void Population::set_beta_ssm_at_szgroup(const vector<double>& _beta_ssm_at_szgroup)
{
    for (unsigned int sz = 0; sz < _beta_ssm_at_szgroup.size(); sz++)
    {
        beta_ssm_at_szgroup[sz] = _beta_ssm_at_szgroup[sz];
    }

}

void Population::set_background_mortality_at_szgroup(const vector<double>& _background_mortality_at_szgroup)
{
    for (unsigned int sz = 0; sz < _background_mortality_at_szgroup.size(); sz++)
    {
        background_mortality_at_szgroup[sz] = _background_mortality_at_szgroup[sz];
    }

}



void Population::set_proprecru_at_szgroup(const vector<double>& _proprecru_at_szgroup)
{
	for(unsigned int sz =0; sz<_proprecru_at_szgroup.size(); sz++)
	{
		proprecru_at_szgroup[sz] =_proprecru_at_szgroup[sz];
	}

}

void Population::set_SSB_at_szgroup(const vector<double>& _SSB_at_szgroup)
{
    for(unsigned int sz =0; sz<_SSB_at_szgroup.size(); sz++)
    {
        SSB_at_szgroup[sz] =_SSB_at_szgroup[sz];
    }

}

void Population::set_SSB(double _SSB)
{
	SSB=_SSB;
}

void Population::set_fbar_type1(double _fbar)
{
    fbar_type1=_fbar;
}

void Population::set_fbar_type2(double _fbar)
{
    fbar_type2 = _fbar;
}

void Population::set_FFmsy(const vector<double> &_FFmsy)
{
    FFmsy=_FFmsy;
}

void Population::set_is_choking_fisheries(int _is_choking_fisheries)
{
    is_choking_fisheries=_is_choking_fisheries;
}


void Population::set_proportion_mature_fish(double _proportion_mature_fish)
{
    proportion_mature_fish=_proportion_mature_fish;
}

void Population::set_landings_so_far(double _landings_so_far)
{
	landings_so_far=_landings_so_far;
}

void Population::set_landings_so_far_this_nation(string nation, double _landings_so_far)
{
    landings_so_far_per_nation[nation] = _landings_so_far;
}

void Population::set_landings_so_far_this_vessel_length_class(int vessel_class, double _landings_so_far)
{
    landings_so_far_per_vessel_length_class[vessel_class] = _landings_so_far;
}

void Population::reset_landings_so_far_per_nation()
{
    for (auto& p : landings_so_far_per_nation) p.second = 0;
    cout << "reset_landings_so_far_per_nation this pop " << this->get_name() << "\n";

}

void Population::reset_landings_so_far_per_vessel_length_class()
{
    for (auto& p : landings_so_far_per_vessel_length_class) p.second = 0;
    cout << "reset_landings_so_far_per_vessel_length_class this pop " << this->get_name() << "\n";

}

void Population::add_to_landings_at_end_of_years(double value)
{
    landings_at_end_of_years.push_back(value);
}


void Population::set_param_sr(const vector<double>& _param_sr)
{

	for(unsigned int i =0; i<_param_sr.size(); i++)
	{
		param_sr[i]=_param_sr[i];
	}
}

void Population::set_selectivity_per_stock_ogives_for_oth_land(const vector<double>& _selectivity_per_stock_ogives_for_oth_land)
{

    for (unsigned int i = 0; i < _selectivity_per_stock_ogives_for_oth_land.size(); i++)
    {
        selectivity_per_stock_ogives_for_oth_land[i] = _selectivity_per_stock_ogives_for_oth_land[i];
    }
}



void Population::set_full_spatial_availability(multimap<types::NodeId,double> _full_spatial_availability)
{
	full_spatial_availability= _full_spatial_availability;
}

void Population::set_field_of_coeff_diffusion_this_pop(multimap<types::NodeId,double> _field_of_coeff_diffusion_this_pop)
{
    field_of_coeff_diffusion_this_pop= _field_of_coeff_diffusion_this_pop;
}

void Population::set_nbhours_for_distance_internodes_this_pop(map<int, int> _nbhours_for_distance_internodes_this_pop)
{
    nbhours_for_distance_internodes_this_pop = _nbhours_for_distance_internodes_this_pop;
}



void Population::set_overall_migration_fluxes(multimap<int,double> _overall_migration_fluxes)
{
    overall_migration_fluxes= _overall_migration_fluxes;
}


void Population::set_oth_land(map<types::NodeId,double> _oth_land)
{
	oth_land= _oth_land;
}


void Population::set_oth_land_map_per_met(vector<map<types::NodeId, double> > _oth_land_map_per_met)
{
    oth_land_map_per_met = _oth_land_map_per_met;
}


void Population::set_oth_land_multiplier(double _oth_land_multiplier)
{
	oth_land_multiplier= _oth_land_multiplier;
}


void Population::set_list_nodes(vector<Node* > _list_nodes)
{
	list_nodes= _list_nodes;
}



// -----------------------------------------------------------------
//  Build the cache from the multimap.
//  After this call, `avail_cache[nid]` contains a vector whose length
//  equals the number of size‑groups (the same length as `tot_N_at_szgroup`).
// -----------------------------------------------------------------
void Population::build_availability_cache()
{
    // Clear any previous data (important if you call this more than once)
    avail_cache.clear();

    // -----------------------------------------------------------------
    // Walk the multimap once.  The multimap is ordered by key, then by
    // insertion order, which matches the order of size‑groups in the
    // original code.
    // -----------------------------------------------------------------
    for (const auto& kv : full_spatial_availability) {
        const types::NodeId& nid = kv.first;
        const double        val = kv.second;

        // `operator[]` creates an empty vector the first time we see a node.
        avail_cache[nid].push_back(val);
    }

    // -----------------------------------------------------------------
    // (Optional) shrink‑to‑fit each vector if you want to free any
    // over‑allocation that `push_back` may have caused.
    // -----------------------------------------------------------------
    for (auto& p : avail_cache) {
        p.second.shrink_to_fit();
    }

    cache_ready = true;   // mark the cache as valid
}

void Population::rebuild_availability_cache()
{
    // 1️⃣  Build a temporary map (no lock needed – local to this thread)
    using TempMap = std::unordered_map<types::NodeId, std::vector<double>, NodeIdHash>;
    TempMap temp;

    // -------------------------------------------------------------
    // 2️⃣  Fill the temporary map
    // -------------------------------------------------------------
    for (const auto& kv : full_spatial_availability) {
        const types::NodeId& nid = kv.first;
        const double        val = kv.second;
        temp[nid].push_back(val);          // creates the vector on first use
    }

    // -------------------------------------------------------------
    // 3️⃣  OPTIONAL: verify that every vector has the expected size
    // -------------------------------------------------------------
    const std::size_t expected_sz = 14;   // whatever your model uses
    for (auto& p : temp) {
        if (p.second.size() != expected_sz) {
            std::cerr << "[WARN] Node " << p.first
                << " has " << p.second.size()
                << " availability entries (expected "
                << expected_sz << "). Filling missing slots with 0.\n";
            p.second.resize(expected_sz, 0.0);   // pad with zeros
        }
    }

    // -------------------------------------------------------------
    // 4️⃣  Swap the temporary map into the class member under exclusive lock
    // -------------------------------------------------------------
    {
        std::unique_lock<std::shared_mutex> lock(cache_mtx);
        avail_cache.swap(temp);            // O(1) pointer swap
        cache_ready = true;
    }
}

/*
void Population::rebuild_availability_cache()
{
    // Caller must hold exclusive access – we acquire it here.
    std::unique_lock<std::shared_mutex> lock(cache_mtx); // exclusive

    avail_cache.clear();
    for (const auto& kv : full_spatial_availability) {
        const types::NodeId& nid = kv.first;
        const double        val = kv.second;
        avail_cache[nid].push_back(val);
    }
    for (auto& p : avail_cache) p.second.shrink_to_fit();
    cache_ready = true;
}
*/


std::vector<double>& Population::get_availability(const types::NodeId& nid)
{
    // 1 Fast path – if the cache is already built, just read it.
    {
        std::shared_lock<std::shared_mutex> lock(cache_mtx);
        if (cache_ready) {
            // `at` throws if nid is truly missing – that's fine.
            return avail_cache.at(nid);
        }
    }

    // 2 Cache not ready – we need exclusive access to build it.
    {
        std::unique_lock<std::shared_mutex> lock(cache_mtx);
        if (!cache_ready) {               // double‑checked (another thread may have built it)
            // Build the whole cache (single‑threaded section)
            avail_cache.clear();
            for (const auto& kv : full_spatial_availability) {
                const types::NodeId& id = kv.first;
                const double val = kv.second;
                avail_cache[id].push_back(val);
            }
            for (auto& p : avail_cache) p.second.shrink_to_fit();
            cache_ready = true;
        }
        // Now the cache is ready – return the entry.
        return avail_cache.at(nid);
    }
}

void Population::set_node_availability(Node* node,
    const std::vector<double>& new_avai)
{
    const std::size_t expected = 14;   // or whatever constant you use

    if (new_avai.size() != expected) {
        std::cout << "[ERROR] Node " << node->get_idx_node()
            << " received new_avai vector of size " << new_avai.size()
            << " (expected " << expected << ").\n";
    }
    
    // 3. Update the fast cache – exclusive lock
    {
    std::unique_lock<std::shared_mutex> lock(cache_mtx);

    // Update the avai on node
    // ...on selected sz 
    
    vector<double> spat_avai_per_selected_szgroup;
    std::vector<char> is_selected(expected, 0);
    for (int idx : selected_szgroups) {
        if (idx >= 0 && static_cast<std::size_t>(idx) < expected)
            is_selected[static_cast<std::size_t>(idx)] = 1;
    }
    spat_avai_per_selected_szgroup.clear();
    spat_avai_per_selected_szgroup.reserve(std::min(expected,
        static_cast<std::size_t>(selected_szgroups.size())));

    for (std::size_t i = 0; i < expected; ++i) {
        if (is_selected[i])
            spat_avai_per_selected_szgroup.push_back(new_avai[i]);
    }
    node->set_avai_pops_at_selected_szgroup(get_name(), spat_avai_per_selected_szgroup);
    

   
    // ...on all sz
    // Keep the multimap in sync 
    const types::NodeId nid = node->get_idx_node();
    // Erase old entries for this node
    full_spatial_availability.erase(
        full_spatial_availability.lower_bound(nid),
        full_spatial_availability.upper_bound(nid));
    // Insert the new values
    for (double v : new_avai)
        full_spatial_availability.emplace(nid, v);

        // Option A – replace the whole vector (fast if you already have new_avai)
        avail_cache[nid] = new_avai;   // copy or move
        // Option B – just invalidate, let lazy rebuild handle it:
        // avail_cache.erase(nid);
    
    
    }
}


void Population::distribute_N()
{

    dout (cout<< "\n");
    dout(cout<< "BEGIN distribute_N(): distribute on nodes for the pop " << name << "\n");
    //vector<double> all_N_at_szgroup(14,0);

	// save for later use i.e. in the erosion of the avai after each catch event
	this->set_tot_N_at_szgroup_just_after_redistribution(tot_N_at_szgroup);

	// update the nodes
	//  N_at_szgroup on node x = N_at_szgroup for pop1 * avai on node x...
    for (unsigned int idx =0; idx < list_nodes.size(); idx++)
	{

		// get a node
        auto idx_node = list_nodes[idx]->get_idx_node();

		// get avai for this node
		//vector<double> avai_this_node;
        //auto lower = full_spatial_availability.lower_bound(idx_node);
        //auto upper = full_spatial_availability.upper_bound(idx_node);
        //for (auto pos=lower; pos != upper; pos++)
		//	avai_this_node.push_back(pos->second);
        // because looking into the multimap is very time demanding, refactored and replaced by:
         // Fast O(1) lookup of the availability vector
        const std::vector<double>& avai_this_node = get_availability(idx_node); // search in avail_cache
        // => Using a convert of the multimap(node → availability per size‑group) into a hash table that gives you the whole vector in O(1).

		// check avai
		//cout << "avai on node "<< idx_node<< ":" << "\n";
		//for(int i=0; i<avai_this_node.size(); i++)
        //    dout(cout << avai_this_node[i] << " ");
		//cout << "\n";

		// check tot_N_at_szgroup
		//cout << "tot_N_at_szgroup on node "<< idx_node<< ":" << "\n";
		//for(int i=0; i<tot_N_at_szgroup.size(); i++)
        //    dout(cout << tot_N_at_szgroup[i] << " ");
		//cout << "\n";

        if (avai_this_node.size() != tot_N_at_szgroup.size()) {
            std::cerr << "[ERROR] Size mismatch for node " << idx_node
                << ": tot_N size = " << tot_N_at_szgroup.size()
                << ", avai size = " << avai_this_node.size()
                << ". STOP.\n";

        }


		// distribute on node applying avai
		vector <double> N_at_szgroup;
        for (unsigned int i = 0; i < tot_N_at_szgroup.size(); i++)
        {
            double a_N = tot_N_at_szgroup.at(i) * avai_this_node.at(i);
            if (a_N < 0) {
                cout << "A negative a_N in distribute_N() detected for this pop " << this->get_name() << "!...set to 0" << "\n";
                cout << " check the avai_this_node.at(sz) value. currently: " << avai_this_node.at(i) << "\n";
                cout << "\n";
            }
            N_at_szgroup.push_back(a_N);
        }
        /*dout(cout << "N at szgroup on the node "<< idx_node<< ":" << "\n";
        for(int i=0; i<N_at_szgroup.size(); i++)
           cout << N_at_szgroup[i] << " ";
           cout << "\n"
         );
         */

		// set the new Ns for this specific pop in the multimap of this node
        //dout(cout << "set the new Ns for this specific pop "<< idx_node<< ":" << "\n");
        list_nodes[idx]->set_Ns_pops_at_szgroup(name, N_at_szgroup);

       // if(this->get_name()==1) for(int i=0; i<N_at_szgroup.size(); i++)
       //    all_N_at_szgroup[i]+=N_at_szgroup[i];

    }

     //if(this->get_name()==1)  for(int i=0; i<all_N_at_szgroup.size(); i++)
     //    cout << "in distribute_N: all_N_at_szgroup["<< i <<"] is " << all_N_at_szgroup[i];

    dout(cout<< "END distribute_N()"<< "\n");
    dout(cout<< "\n");

}


void Population::aggregate_N()
{

    dout(cout<< "\n");
    dout(cout<< "BEGIN aggregate_N(): aggregate from nodes for the pop " << name << "\n");

	// temporary objects
	vector<double> agg_Ns_at_szgroup;
//	vector<double> Ns_this_node;

	// a for-loop over nodes specficic to this pop
	for (unsigned int idx =0; idx < list_nodes.size(); idx++)
	{
		// get idx node (just for info)
//		int idx_node = list_nodes[idx]->get_idx_node();

		// init with the first node
		if(idx==0)
		{
			agg_Ns_at_szgroup = list_nodes[idx]->get_Ns_pops_at_szgroup(name);
		}
		else
		{
			// get the Ns on this node and add to the agg
            const vector<double> &Ns_this_node = list_nodes[idx]->get_Ns_pops_at_szgroup(name);
            for(unsigned int i=0; i<agg_Ns_at_szgroup.size(); i++)
			{
				if(!isnan(Ns_this_node.at(i))) agg_Ns_at_szgroup.at(i)= agg_Ns_at_szgroup.at(i)+Ns_this_node.at(i);
			}

		}
	}							 // end for-loop over nodes

	// set the new tot Ns for this specific pop in the vector tot_N_at_szgroup of the pop
	this->set_tot_N_at_szgroup(agg_Ns_at_szgroup);

	// check
	//if(this->get_name()==10)
	//{
    //    dout(cout << "in aggregate(): tot N at szgroup for the pop "<< this->get_name() << " after aggregation:"  << "\n");
	//    for(int i=0; i<tot_N_at_szgroup.size(); i++)
	//    {
    //        dout(cout << tot_N_at_szgroup[i] << " ");
	//    }
	//}

    dout(cout << "\n");

    dout(cout<< "END aggregate_N()" << "\n");
    dout(cout<< "\n");
}


void Population::aggregate_N_display_for_check()
{ 
    cout << "in aggregate_N_display_for_check(): tot N at szgroup for the pop " << this->get_name() << " before aggregation:" << "\n";
    for (int i = 0; i < tot_N_at_szgroup.size(); i++)
    {
        cout << tot_N_at_szgroup[i] << " ";
    }
    cout << "\n";

    cout << "in aggregate_N_display_for_check(): SSB at szgroup for the pop " << this->get_name() << " before aggregation:" << "\n";
    vector <double> SSB_vect_before(tot_N_at_szgroup.size(), 0.0);
    for (unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
    {

        // reminder: tot_N_at_szgroup are in thousand in input file
        //  but in absolute numbers here because have been multiplied by 1000 when importing
        SSB_vect_before.at(sz) = this->get_weight_at_szgroup().at(sz) * tot_N_at_szgroup.at(sz) * this->get_maturity_at_szgroup().at(sz);
        cout << sz << ": "<< SSB_vect_before.at(sz);
    }
    // check numbers
    cout << "\n";
    cout << "given a_tot_N_at_szgroup: " << "\n";
    for (unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
    {
        cout << "sz" << sz << ": " << tot_N_at_szgroup.at(sz) << " ";
    }
    cout << "\n";
    cout << "given maturity_at_szgroup: " << "\n";
    for (unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
    {
        cout << "sz" << sz << ": " << this->get_maturity_at_szgroup().at(sz) << " ";
    }
    cout << "\n";
    cout << "given weight_at_szgroup: " << "\n";
    for (unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
    {
        cout << "sz" << sz << ": " << this->get_weight_at_szgroup().at(sz) << "kg ";
    }
    cout << "\n";
    double tot_SSB_kg_before_agg = 0;
    double tot_N_before_agg = 0;
    for (int sz = 0; sz < SSB_vect_before.size(); sz++)
    {
        tot_SSB_kg_before_agg += SSB_vect_before.at(sz);
        tot_N_before_agg += tot_N_at_szgroup.at(sz);
    }
    cout << "===>tot_SSB_before_agg is " << tot_SSB_kg_before_agg/1e3 << " tonnes" << "\n";
    cout << "===>tot_N_before_agg is " << tot_N_before_agg / 1e3 << " thousands" << "\n";



    // temporary objects
    vector<double> agg_Ns_at_szgroup;
 
    // a for-loop over nodes specficic to this pop
    for (unsigned int idx = 0; idx < list_nodes.size(); idx++)
    {
        // get idx node (just for info)
//		int idx_node = list_nodes[idx]->get_idx_node();

        // init with the first node
        if (idx == 0)
        {
            agg_Ns_at_szgroup = list_nodes[idx]->get_Ns_pops_at_szgroup(name);
        }
        else
        {
            // get the Ns on this node and add to the agg
            const vector<double>& Ns_this_node = list_nodes[idx]->get_Ns_pops_at_szgroup(name);
            for (unsigned int i = 0; i < agg_Ns_at_szgroup.size(); i++)
            {
                if (!isnan(Ns_this_node.at(i))) agg_Ns_at_szgroup.at(i) = agg_Ns_at_szgroup.at(i) + Ns_this_node.at(i);
            }

        }
    }							 // end for-loop over nodes

    // check
    cout << "in aggregate_N_display_for_check(): SSB at szgroup for the pop " << this->get_name() << " if we were aggregating over nodes now:" << "\n";
    vector <double> SSB_vect(agg_Ns_at_szgroup.size(), 0.0);
    for (unsigned int sz = 0; sz < agg_Ns_at_szgroup.size(); sz++)
    {

        // reminder: tot_N_at_szgroup are in thousand in input file
        //  but in absolute numbers here because have been multiplied by 1000 when importing
        SSB_vect.at(sz) = this->get_weight_at_szgroup().at(sz) * agg_Ns_at_szgroup.at(sz) * this->get_maturity_at_szgroup().at(sz);
        cout << sz << ": " << SSB_vect.at(sz);
    }
    // check numbers
    cout << "\n";
    cout << "given a_tot_N_at_szgroup: " << "\n";
    for (unsigned int sz = 0; sz < agg_Ns_at_szgroup.size(); sz++)
    {
        cout << "sz" << sz << ": " << agg_Ns_at_szgroup.at(sz) << " ";
    }
    cout << "\n";
    cout << "given maturity_at_szgroup: " << "\n";
    for (unsigned int sz = 0; sz < agg_Ns_at_szgroup.size(); sz++)
    {
        cout << "sz" << sz << ": " << this->get_maturity_at_szgroup().at(sz) << " ";
    }
    cout << "\n";
    cout << "given weight_at_szgroup: " << "\n";
    for (unsigned int sz = 0; sz < agg_Ns_at_szgroup.size(); sz++)
    {
        cout << "sz" << sz << ": " << this->get_weight_at_szgroup().at(sz) << "kg ";
    }
    cout << "\n";
    double tot_SSB_kg_after_agg = 0;
    double tot_N_after_agg = 0;
    for (int sz = 0; sz < SSB_vect.size(); sz++)
    {
        tot_SSB_kg_after_agg += SSB_vect.at(sz);
        tot_N_after_agg += agg_Ns_at_szgroup.at(sz);
    }
    cout << "===>tot_SSB_after_agg is " << tot_SSB_kg_after_agg/ 1e3 << " tonnes" << "\n";
    cout << "===>tot_N_after_agg is " << tot_N_after_agg / 1e3 << " thousands" << "\n";


    


    cout << "\n";

}


//-------------------------------------------------------------------//
//-------------diffusePopN Option------------------------------------//
//-------------------------------------------------------------------//


const CoeffMap& Population::get_cached_coeff_map() const
{
    if (cached_coeff_map_.empty())
        build_coeff_cache();          // fill the map the first time we need it
    return cached_coeff_map_;
}

void Population::build_coeff_cache() const
{
    if (!cached_coeff_map_.empty())
        return;                       // already built

    const auto& field = this->get_field_of_coeff_diffusion_this_pop(); // multimap<NodeId,double>
    const size_t n_sz = this->get_tot_N_at_szgroup().size(); // number of size groups (e.g. 14)

    // Walk through the multimap once and pack the coefficients per node
    for (auto it = field.begin(); it != field.end(); ++it) {
        const types::NodeId& nid = it->first;
        const double coeff_val = it->second;

        // Ensure a vector exists for this node
        auto& vec = cached_coeff_map_[nid];
        if (vec.empty())
            vec.assign(n_sz, 0.0);   // allocate once with the correct size

        // The multimap is ordered by node then by size‑group, so we can fill sequentially
        // (the original code used a counter `i` – we mimic that here)
        static size_t i = 0;
        vec[i++] = coeff_val;
        if (i == n_sz) i = 0;        // reset for the next node
    }
}




void Population::diffuse_N_from_field(const vector<Node*>& nodes, adjacency_map_t& adjacency_map)
{
//#ifdef ENABLE_DIFFUSION_LOG
    std::cout << "Start diffusion for this pop …\n";
//#endif

    // -----------------------------------------------------------------
    // 1 Gather data that never changes inside the node loop
    // -----------------------------------------------------------------
    const std::vector<Node*>& node_list = this->get_list_nodes();   // already a vector of pointers
    const size_t n_nodes = node_list.size();

    // Build a fast‑lookup set of node ids that belong to this population

    std::unordered_set<types::NodeId, NodeIdHash> pop_node_ids;
    pop_node_ids.reserve(n_nodes * 2);
    for (Node* nd : node_list)
        pop_node_ids.insert(nd->get_idx_node());
    // Shuffle the order *once* (the original code shuffled each iteration)
    std::vector<Node*> shuffled_nodes = node_list;   // copy
    std::shuffle(shuffled_nodes.begin(), shuffled_nodes.end(), g);

    // Cached diffusion coefficients (node to vector of coeff per size‑group)
    const auto& coeff_map = this->get_cached_coeff_map();

    // -----------------------------------------------------------------
    // 2️⃣  Temporary buffers (allocated once, reused)
    // -----------------------------------------------------------------
    std::vector<double> departure_N;          // will be resized per node
    std::vector<double> arrival_N;            // reused for each neighbour
    // consider using array for speed: if the number of size groups is known at compile time. This removes dynamic allocation for each node’s 
    //std::array<double, 14> departure_N;          
    //std::array<double, 14> arrival_N;            
    std::vector<int>    neighbour_idxs;       // indices of neighbours that belong to the pop
    neighbour_idxs.reserve(64);               // typical neighbourhood size

    // -----------------------------------------------------------------
    // 3️⃣  Main diffusion loop (node‑by‑node)
    // -----------------------------------------------------------------
    for (size_t n = 0; n < n_nodes; ++n) {
        Node* cur_node = shuffled_nodes[n];
        const types::NodeId cur_id = cur_node->get_idx_node();

        // ---- progress report (optional) ---------------------------------
//#ifdef ENABLE_DIFFUSION_LOG
        if ((n & 0xFFF) == 0) { // every ~4096 nodes
            double pct = 100.0 * (static_cast<double>(n) / n_nodes);
            std::cout << "Diffusion progress: " << pct << "%\n";
        }
//#endif

        // ---- 3.1  Get diffusion coefficients for this node ------------
        auto coeff_it = coeff_map.find(cur_id);
        if (coeff_it == coeff_map.end())
            continue;   // no coefficients then nothing diffuses from this node

        const std::vector<double>& some_coeffs = coeff_it->second; // already sized correctly

        // ---- 3.2  Load the current N vector for this node ------------
        departure_N = cur_node->get_Ns_pops_at_szgroup(this->get_name()); // copies the vector

        // ---- 3.3  Find neighbours (graph adjacency) --------------------
        const vertex_t u = cur_id.toIndex();
        const std::list<edge>& edges = adjacency_map[u];

        neighbour_idxs.clear();
        for (const edge& e : edges) {
            const types::NodeId neigh_id(e.target);
            // keep only neighbours that belong to the spatial extent of the pop
            if (pop_node_ids.find(neigh_id) != pop_node_ids.end())
                neighbour_idxs.push_back(static_cast<int>(neigh_id.toIndex()));
        }

        const size_t n_neigh = neighbour_idxs.size();
        if (n_neigh == 0)
            continue;   // isolated node – nothing to diffuse

        // ---- 3.4  Diffuse to each neighbour ---------------------------
        const double inv_n_neigh = 1.0 / static_cast<double>(n_neigh);
        for (int neigh_idx : neighbour_idxs) {
            Node* neigh_node = nodes[neigh_idx];

            arrival_N = neigh_node->get_Ns_pops_at_szgroup(this->get_name()); // copy

            // ---- per‑size‑group exchange --------------------------------
            const size_t n_sz = some_coeffs.size();   // number of size groups (e.g. 14)
            for (size_t sz = 0; sz < n_sz; ++sz) {
                const double exchanged = some_coeffs[sz] * departure_N[sz] * inv_n_neigh;
                arrival_N[sz] += exchanged;
                departure_N[sz] -= exchanged;
            }

            // ---- write back neighbour ------------------------------------
            neigh_node->set_Ns_pops_at_szgroup(this->get_name(), arrival_N);
        }

        // ---- 3.5  Write back the updated departure node ----------------
        cur_node->set_Ns_pops_at_szgroup(this->get_name(), departure_N);
    }

//#ifdef ENABLE_DIFFUSION_LOG
    std::cout << "Stop diffusion for this pop …\n";
//#endif


}


/*
void Population::diffuse_N_from_field(adjacency_map_t& adjacency_map)
{
   cout << "start diffusion for this pop...." << "\n";

    vector<Node*> list_of_nodes = this->get_list_nodes();
    vector<types::NodeId> list_of_nodes_idx;
    std::shuffle (list_of_nodes.begin(), list_of_nodes.end(), g);
    for (int n=0; n<list_of_nodes.size(); ++n)
       {
       list_of_nodes_idx.push_back(list_of_nodes.at(n)->get_idx_node());
       }

    vector<double> coeff(14);
    //vector<double> coeff;
    for (int n=0; n<list_of_nodes.size(); ++n)
       {
        auto idx_node=list_of_nodes.at(n)->get_idx_node();

        double percent = (static_cast<double>(n+1) / list_of_nodes.size()) * 100;
        cout << "Covered: " <<  percent << " % " << "\n";

       // This takes quite some time. therefore 
       //     # worth reducing a bit the dimensionality in the R input routine...
       //     nodes_to_keep < -popsspe_coeffs_semester_this_pop[as.numeric(as.character(popsspe_coeffs_semester_this_pop$coeff)) > 0.1, "pt_graph"]
       //     popsspe_coeffs_semester_this_pop < -popsspe_coeffs_semester_this_pop[popsspe_coeffs_semester_this_pop$pt_graph % in % nodes_to_keep, ]



        //cout << "On this node...." << idx_node  << "\n";

        // get coeff of diffusion per szgroup for this node
        auto field_of_coeff_diffusion_this_pop = this->get_field_of_coeff_diffusion_this_pop();
        auto lower = field_of_coeff_diffusion_this_pop.lower_bound(idx_node);
        auto upper = field_of_coeff_diffusion_this_pop.upper_bound(idx_node);
        int i=0;
        for (auto pos = lower; pos != upper; pos++)
        {
            //  coeff.push_back(pos->second);
            coeff[i] = pos->second;
            i++;
        }

        try {
            if (coeff.size() == 0) {
                throw -1;
            }
        }
        catch (int e) {
            cout << "inconsistency between popsspe static_avai avai.dat nodes and popsspe static_avai field_of_coeff_diffusion_this_pop.dat nodes" << "\n";
        }

        // get the N for this pop on this node
        vector<double> departure_N = list_of_nodes.at(n)->get_Ns_pops_at_szgroup(this->get_name());


        //cout << "get the list of neighbouring nodes for this node...." << "\n";
        // get the list of neighbouring nodes
        vector<types::NodeId> neighbour_nodes;
        vertex_t u = idx_node.toIndex();
        // Visit each edge exiting u
        for (std::list<edge>::iterator edge_iter = adjacency_map[u].begin();
             edge_iter != adjacency_map[u].end();
             edge_iter++)
        {
            neighbour_nodes.push_back(types::NodeId(edge_iter->target));
        }
        
      
        
        // remove duplicates
        std::sort(neighbour_nodes.begin(), neighbour_nodes.end()); // {1 1 2 3 4 4 5}
        auto last = std::unique(neighbour_nodes.begin(), neighbour_nodes.end());
        neighbour_nodes.erase(last, neighbour_nodes.end());


        // check if neighbouring nodes belong to the spatial extent of this pop
        // (no diffusion outside....caution: possible border effects because of this assumption e.g. accumulation at the border)
        vector<int> neighbour_nodes_on_spatial_extent;
        for (int nei=0; nei<neighbour_nodes.size(); ++nei)
           {
           auto it =find(list_of_nodes_idx.begin(),list_of_nodes_idx.end(), neighbour_nodes.at(nei));
           if(it != list_of_nodes_idx.end())
             {
             auto pos = distance(list_of_nodes_idx.begin(), it);
             neighbour_nodes_on_spatial_extent.push_back(pos);
             }
           }
        int count = neighbour_nodes_on_spatial_extent.size();

        if(count!=0){

            // displace a proportion of N from departure node to neighbours nodes
               vector <double> depN=departure_N;
               vector <double> arrival_N;
               for (int nei=0; nei<count; ++nei)
                  {
                   auto a_node= list_of_nodes.at(neighbour_nodes_on_spatial_extent.at(nei));
                       //cout << "for neighbours node...." << list_of_nodes.at(neighbour_nodes_on_spatial_extent.at(nei))->get_idx_node() << "\n";
                   arrival_N = a_node->get_Ns_pops_at_szgroup( this->get_name() );
                   for (int sz=0; sz<arrival_N.size(); ++sz)
                      {
                      double exchanged       = ((coeff.at(sz)*depN.at(sz))/count);
                      arrival_N.at(sz)       = arrival_N.at(sz) + exchanged;
                      departure_N.at(sz)     = departure_N.at(sz) - exchanged;
                     }
                   a_node->set_Ns_pops_at_szgroup( this->get_name(), arrival_N );//update arrival
                   }
               list_of_nodes.at(n)->set_Ns_pops_at_szgroup( this->get_name(), departure_N ); //update departure

       } // count!=0

       // cout << "go to next node...." << "\n";
    } // node by node

  cout << "stop diffusion for this pop...." << "\n";
}


*/



void Population::do_growth(int is_stochastic)
{
    dout(cout << "BEGIN do_growth() "  << "\n" );

//	int namepop = this->get_name();

								 // init
	vector <double> new_tot_N_at_szgroup (tot_N_at_szgroup.size());

    double growth_error;
    if(is_stochastic)
    {
        //cout << "before adding norm error on growth matrix:" << "\n";

        for (unsigned int i = 0; i < growth_transition_matrix.size(); i++)
        {
            for (unsigned int j = 0; j < growth_transition_matrix[i].size(); j++)
            {
                //std::cout << growth_transition_matrix[i][j] << " ";
            }
            //std::cout << "\n";
        }
 
        //cout << "after adding norm error on growth matrix:" << "\n";

        double sd = 0.1;
        vector<double> marginal_col_sums(growth_transition_matrix[0].size(), 0);
        for (unsigned int i = 0; i < growth_transition_matrix.size(); i++)
        {
            for (unsigned int j = 0; j < growth_transition_matrix[i].size(); j++)
            {
                // adding a lognormal error
                growth_transition_matrix[i][j] *= exp(0 + sd * norm_rand()) / exp((sd * sd) / 2.0);
                marginal_col_sums.at(j) += growth_transition_matrix[i][j];
                //std::cout << growth_transition_matrix[i][j] << " ";
            }
            //std::cout << "\n";
        }
 
        //cout << "then normalize it to sum to 1 in columns:" << "\n";

        for (unsigned int i = 0; i < growth_transition_matrix.size(); i++)
        {
            for (unsigned int j = 0; j < growth_transition_matrix[i].size(); j++)
            {
                if(marginal_col_sums.at(j)>0) growth_transition_matrix[i][j] /= marginal_col_sums.at(j);
                //std::cout << growth_transition_matrix[i][j] << " ";
            }
            //std::cout << "\n";
        }

    }

    cout << "\n";

	// size transition matrix:
	// row i: output
	// column j: input
	// sum to 1 in columns
	// e.g. for group 1, N1_output= G[1,j]*N_input[j] with j from 0 to xx group

	// apply size transition matrix
	for(unsigned int i = 0; i < growth_transition_matrix.size(); i++)
	{
		for(unsigned int j = 0; j < growth_transition_matrix[i].size(); j++)
		{

			//if(namepop==12) {
            //     dout(cout << "i is "<< i  << "\n");
            //     dout(cout << "j is "<< j  << "\n");
            //     dout(cout << "growth[i,j] is "<< growth_transition_matrix[i][j]  << "\n");
            //     dout(cout << "tot_N_at_szgroup[i]  is "<< tot_N_at_szgroup[i]  << "\n");
			//}
			new_tot_N_at_szgroup[i] +=  growth_transition_matrix[i][j] * tot_N_at_szgroup[j] ;
		}
	}

	// set the tot N at szgroup
	this->set_tot_N_at_szgroup(new_tot_N_at_szgroup);

   
    //check
    //cout << "Check the Ns:" << "\n";
    //for (unsigned int i = 0; i < new_tot_N_at_szgroup.size(); i++)
    //{
    //    std::cout << new_tot_N_at_szgroup[i] << " ";
    //}
    //std::cout << "\n";


    dout(cout << "END do_growth() "  << "\n" );
}


void Population::apply_overall_migration_fluxes(vector<Population* >& populations)
{
    dout(cout << "BEGIN overall_migration_fluxes() "  << "\n" );
    // caution: the current implementation is biased by the reading order of pops...

    multimap <int, double> migration_fluxes= this->get_overall_migration_fluxes();

    if(!migration_fluxes.empty())
    {


        multimap<int, double>::iterator pos;

       int sz=0;
       for(pos = migration_fluxes.begin(); pos != migration_fluxes.end(); ++pos)
          {

          int arrival_pop = pos->first;
          double flux_prop =pos->second;


 cout << "departure pop is " << this->get_name() << "\n";
 cout << "arrival_pop is " << arrival_pop << "\n";
 cout << "sz is " << sz << "\n";
 cout << "flux_prop is " << flux_prop << "\n";

           // input
          vector<double> N_at_szgroup_this_pop = this->get_tot_N_at_szgroup();
          vector<double> N_at_szgroup_arr_pop  = populations.at(arrival_pop)->get_tot_N_at_szgroup();
          vector<double> weight_at_szgroup_this_pop = this->get_weight_at_szgroup();
          vector<double> weight_at_szgroup_arr_pop = populations.at(arrival_pop)->get_weight_at_szgroup();

          if((sz+1)>=N_at_szgroup_this_pop.size()) sz = 0; // reinit because next pop

 cout << " before: N_at_szgroup_this_pop.at(sz) is " <<  N_at_szgroup_this_pop.at(sz) << "\n";
 cout << " before: N_at_szgroup_arr_pop.at(sz) is " <<  N_at_szgroup_arr_pop.at(sz) << "\n";

 cout << " before: weight_at_szgroup_arr_pop.at(sz) is " <<  weight_at_szgroup_arr_pop.at(sz) << "\n";

 // impact the Ns from emigration/immigration specified in the multimap
          double tot_before_in_arr = N_at_szgroup_arr_pop.at(sz);
          double tot_to_in_arr     = N_at_szgroup_this_pop.at(sz)*flux_prop;
          double tot_now_in_arr    = N_at_szgroup_arr_pop.at(sz)+(N_at_szgroup_this_pop.at(sz)*flux_prop);
          double tot_now_in_dep    = N_at_szgroup_this_pop.at(sz)*(1-flux_prop);

          N_at_szgroup_arr_pop.at(sz)  = tot_now_in_arr;
          N_at_szgroup_this_pop.at(sz) = tot_now_in_dep;

 cout << " after: N_at_szgroup_this_pop.at(sz) is " <<  N_at_szgroup_this_pop.at(sz) << "\n";
 cout << " after: N_at_szgroup_arr_pop.at(sz) is " <<  N_at_szgroup_arr_pop.at(sz) << "\n";


          // do a weighted average of the weight
          if(tot_now_in_arr!=0) weight_at_szgroup_arr_pop.at(sz)  = (tot_to_in_arr/tot_now_in_arr) *weight_at_szgroup_arr_pop.at(sz) +
                                                    (tot_before_in_arr/tot_now_in_arr)* weight_at_szgroup_this_pop.at(sz);

cout << " after: weight_at_szgroup_arr_pop.at(sz) is " <<  weight_at_szgroup_arr_pop.at(sz) << "\n";

          // output this pop
          this->set_tot_N_at_szgroup(N_at_szgroup_this_pop);


          // output arrival pop
          populations.at(arrival_pop)->set_tot_N_at_szgroup(N_at_szgroup_arr_pop);

          // ....including affecting the weight-at-szgroup
          populations.at(arrival_pop)->set_weight_at_szgroup(weight_at_szgroup_arr_pop);


          sz +=1;
       }
      } else{

       dout(cout << "no migration for this pop" << this->get_name()  << "\n" );


    }


    dout(cout << "END overall_migration_fluxes() "  << "\n" );
}



void Population::add_recruits_from_SR()
{
    dout(cout << "BEGIN add_recruits() form SR "  << "\n" );

	// compute SSB
    // first of all, get the true N
    vector <double> a_tot_N_at_szgroup=this->get_tot_N_at_szgroup();
    vector <double> prop_migrants=this->get_prop_migrants_in_tot_N_at_szgroup();
    for(unsigned int sz = 0; sz < a_tot_N_at_szgroup.size(); sz++)
    {
       a_tot_N_at_szgroup.at(sz) = a_tot_N_at_szgroup.at(sz) - (a_tot_N_at_szgroup.at(sz) *prop_migrants.at(sz)); // retrieve the true N
    }


    vector <double> SSB_per_szgroup (a_tot_N_at_szgroup.size());
    dout(cout << "pop is " << this->get_name()  << " " << "\n" );
    for(unsigned int i = 0; i < a_tot_N_at_szgroup.size(); i++)
    {

       // reminder: tot_N_at_szgroup are in thousand in input file
       //  but in absolute numbers here because have been multiplied by 1000 when importing
       SSB_per_szgroup.at(i) =  weight_at_szgroup.at(i) * a_tot_N_at_szgroup.at(i) * maturity_at_szgroup.at(i);
       dout(cout << "szgroup is " << i  << " " << "\n" );
       cout << "tot_N_at_szgroup is " << a_tot_N_at_szgroup.at(i)  << " " << "\n" ;
       cout << "maturity_at_szgroup is " << maturity_at_szgroup.at(i)  << " " << "\n" ;
       cout << "weight_at_szgroup is " << weight_at_szgroup.at(i)  << " kg" << "\n" ;

    }



	// ...then, cumul for getting tot SSB (here in kilos)
	SSB=0;
    for(unsigned int i = 0; i < SSB_per_szgroup.size(); i++)
	{
		SSB +=  SSB_per_szgroup.at(i);
	}
    SSB= SSB/1000;			 //
    cout << "SSB is " << SSB  << " tons" << "\n" ;
    //dout(cout << "SSB is " << SSB  << " tons" << "\n" );

	// compute R from SSB-R relationship
	// (caution: age dependent, e.g. SSB-R for cod 2532 is usually simulated for age2)...
    double recruits = 0.0;
    // ricker (default):
    if (param_sr[3] < 1) 
    {
        recruits = (param_sr[0] * SSB * exp(-param_sr[1] * SSB)) * 1000;
        cout << "pop" << this->get_name() << ", Ricker model with param alpha "<< param_sr[0]  << " and beta " << param_sr[1] << ": New recruits are " << recruits << "\n";
    }
    // B&H ((alpha*ssb)/(1+beta*ssb)):
    if (param_sr[3] == 1)
    {
        recruits = (param_sr[0] * SSB) / (1 + param_sr[1] * SSB) * 1000;
        cout << "pop" << this->get_name() << ", B&H model with param alpha " << param_sr[0] << " and beta " << param_sr[1] << ": New recruits are " << recruits << "\n";
    }
    // fixed recruits:
    if (param_sr[3] == 2) 
    {
        recruits = (param_sr[0]) * 1000;
        cout << "pop" << this->get_name() <<  ", Fixed recruits model: New recruits are " << recruits << "\n";
    }
    
    // add stochasticity on recruits (MAGIC NUMBER default: lognormal with CV at 20%)
	// TO DO: use a stock-specific input there...
    double sd=param_sr[2];
    if(sd==0.0) sd=0.2; // default
	double rec_error=0;
	rec_error= exp( 0 + sd*norm_rand() ) / exp((sd*sd)/2.0);
	recruits= recruits * rec_error;
    //dout(cout << "stochastic recruits are " << recruits  << "\n" );
    cout << "stochastic recruits are " << recruits  << "\n" ;

    // init
    vector <double> new_tot_N_at_szgroup (SSB_per_szgroup.size());


    // ...then distribute among szgroup
    for(unsigned int i = 0; i < SSB_per_szgroup.size(); i++)
	{
        new_tot_N_at_szgroup[i] =  a_tot_N_at_szgroup.at(i) + (recruits* proprecru_at_szgroup.at(i));
        cout << "recruits for szgroup " << i << ": " << recruits* proprecru_at_szgroup.at(i) << " to add to N this grp " << "\n" ;
        //dout(cout << "for szgroup " << i << ": " << recruits* proprecru_at_szgroup.at(i)  << "\n" );
    }

	//if(this->get_name()==29){
	//int a_int;
	//cin >> a_int;
	//}

	// set the tot N at szgroup
	this->set_tot_N_at_szgroup(new_tot_N_at_szgroup);

	// redistribute on nodes
	//distribute_N();

    dout(cout << "END add_recruits() "  << "\n" );
}

void Population::add_recruits_from_a_fixed_number()
{
    dout(cout << "BEGIN add_recruits() form SR "  << "\n" );

    // first of all, get the true N
    vector <double> a_tot_N_at_szgroup=this->get_tot_N_at_szgroup();
    vector <double> prop_migrants=this->get_prop_migrants_in_tot_N_at_szgroup();
    for(unsigned int sz = 0; sz < a_tot_N_at_szgroup.size(); sz++)
    {
       a_tot_N_at_szgroup.at(sz) = a_tot_N_at_szgroup.at(sz) - (a_tot_N_at_szgroup.at(sz) *prop_migrants.at(sz)); // retrieve the true N
    }


    // init
    vector <double> new_tot_N_at_szgroup (a_tot_N_at_szgroup.size());


    double recruits =param_sr[0];
    dout(cout << "New recruits are " << recruits  << "\n" );

    // add stochasticity on recruits (MAGIC NUMBER default: lognormal with CV at 20%)
    // TO DO: use a stock-specific input there...
    double sd=param_sr[2];
    if(sd==0.0) sd=0.2; // default
    double rec_error=0;
    rec_error= exp( 0 + sd*norm_rand() ) / exp((sd*sd)/2.0);
    recruits= recruits * rec_error;
    //dout(cout << "stochastic recruits are " << recruits  << "\n" );
    cout << "stochastic recruits are " << recruits  << "\n" ;

    // ...then distribute among szgroup
    for(unsigned int i = 0; i < a_tot_N_at_szgroup.size(); i++)
    {
        new_tot_N_at_szgroup[i] =  a_tot_N_at_szgroup.at(i) + (recruits* proprecru_at_szgroup.at(i));
        cout << "recruits for szgroup " << i << ": " << recruits* proprecru_at_szgroup.at(i) << " to add to N this grp "  << a_tot_N_at_szgroup.at(i) << "\n" ;
        //dout(cout << "for szgroup " << i << ": " << recruits* proprecru_at_szgroup.at(i)  << "\n" );
    }

    //if(this->get_name()==29){
    //int a_int;
    //cin >> a_int;
    //}

    // set the tot N at szgroup
    this->set_tot_N_at_szgroup(new_tot_N_at_szgroup);

    // redistribute on nodes
    //distribute_N();

    dout(cout << "END add_recruits() "  << "\n" );
}


void Population::add_recruits_from_eggs()
{
    dout(cout << "BEGIN add_recruits() from eggs "  << "\n" );

								 // init
	vector <double> new_tot_N_at_szgroup (tot_N_at_szgroup.size());

	// aggregate from nodes
	//aggregate_N();

	// compute recruits from SSB
								 // init
	vector <double> recruits_per_szgroup (tot_N_at_szgroup.size());
	for(unsigned int i = 0; i < tot_N_at_szgroup.size(); i++)
	{

		recruits_per_szgroup.at(i) =  tot_N_at_szgroup.at(i)/2 * fecundity_at_szgroup.at(i) * maturity_at_szgroup.at(i);

	}
	// ...then, add recruits to bin 0
	for(unsigned int i = 0; i < tot_N_at_szgroup.size(); i++)
	{

		new_tot_N_at_szgroup[0] +=  recruits_per_szgroup.at(i);
	}

	// set the tot N at szgroup
	this->set_tot_N_at_szgroup(new_tot_N_at_szgroup);

	// redistribute on nodes
	//distribute_N();

    dout(cout << "END add_recruits() "  << "\n");
}


void Population::compute_tot_N_and_F_and_W_at_age(int a_month_i)
{
    dout(cout << "BEGIN compute_tot_N_and_F_and_W_at_age() for pop " << this->get_name()  << "\n") ;

	vector <double> tot_F_at_age = get_tot_F_at_age();
    vector <double> tot_F_at_age_running_average = get_tot_F_at_age_running_average();
    vector <double> FFmsy (tot_F_at_age.size());
    vector <double> perceived_tot_F_at_age = get_tot_F_at_age();
                                 // init
    //vector <double> tot_M_at_age (tot_F_at_age.size());
								 // init
	vector <double> tot_W_at_age (tot_F_at_age.size());
								 // init
    vector <double> tot_Mat_at_age (tot_F_at_age.size());
                                 // init
    vector <double> tot_N_at_age (tot_F_at_age.size());
    vector <double> perceived_tot_N_at_age (tot_F_at_age.size());
                                 // init
	vector <double> tot_N_at_age_minus_1 (tot_F_at_age.size());
    vector <double> perceived_tot_N_at_age_minus_1 (tot_F_at_age.size());

	int nbsz = percent_szgroup_per_age_matrix.size();
	int nbages = percent_szgroup_per_age_matrix[0].size();


    // first of all, get the true N
    vector <double> a_tot_N_at_szgroup         =this->get_tot_N_at_szgroup();
    vector <double> perceived_tot_N_at_szgroup =this->get_tot_N_at_szgroup();
    vector <double> prop_migrants              =this->get_prop_migrants_in_tot_N_at_szgroup();
    for(unsigned int sz = 0; sz < a_tot_N_at_szgroup.size(); sz++)
    {
       a_tot_N_at_szgroup.at(sz) = a_tot_N_at_szgroup.at(sz) - (a_tot_N_at_szgroup.at(sz) *prop_migrants.at(sz));
    }

    vector <double> a_tot_N_at_szgroup_month_minus_1=this->get_tot_N_at_szgroup_month_minus_1();
    vector <double> perceived_tot_N_at_szgroup_month_minus_1=this->get_tot_N_at_szgroup_month_minus_1();
    for(unsigned int sz = 0; sz < a_tot_N_at_szgroup_month_minus_1.size(); sz++)
    {
       a_tot_N_at_szgroup_month_minus_1.at(sz) =a_tot_N_at_szgroup_month_minus_1.at(sz) - (a_tot_N_at_szgroup_month_minus_1.at(sz) *prop_migrants.at(sz));
    }

    // apply size percent_szgroup_per_age_matrix to finds out N IN AGE!
    for(int sz = 0; sz < nbsz; sz++)
	{
        for(int a = 0; a < nbages; a++)
		{

			// check
			/*
			if(this->get_name()==10 && sz<6)
			{
				dout << "pop "<< this->get_name()  << "\n";
				dout << "percent_szgroup_per_age_matrix.size() "<< percent_szgroup_per_age_matrix.size()  << "\n";
				dout << "a is "<< a  << "\n";
				dout << "sz is "<< sz  << "\n";
				dout << "percent_szgroup_per_age_matrix[sz,a] is "<< percent_szgroup_per_age_matrix[sz][a]  << "\n";
				dout << "tot_N_at_szgroup[sz]  is "<< tot_N_at_szgroup[sz]  << "\n";
				dout << "tot_N_at_szgroup_month_minus_1[sz]  is "<< tot_N_at_szgroup_month_minus_1[sz] << "\n";
			}
			*/

            // used for perceived stock in the management procedure
            perceived_tot_N_at_age[a] +=  percent_szgroup_per_age_matrix[sz][a] * perceived_tot_N_at_szgroup[sz] ;
            perceived_tot_N_at_age_minus_1[a] +=  percent_szgroup_per_age_matrix[sz][a] * perceived_tot_N_at_szgroup_month_minus_1[sz] ;

            // used for outcomes
            tot_N_at_age[a] +=  percent_szgroup_per_age_matrix[sz][a] * a_tot_N_at_szgroup[sz] ;
            tot_N_at_age_minus_1[a] +=  percent_szgroup_per_age_matrix[sz][a] * a_tot_N_at_szgroup_month_minus_1[sz] ;
        }


    }

    // apply a WEIGHTED MEAN for weight-at-age and mortality-at-age
    for(int a = 0; a < nbages; a++)
	{
        for(int sz = 0; sz < nbsz; sz++)
		{

			// check
			/*
			if(this->get_name()==10 && sz<6)
			{
				dout << "pop "<< this->get_name()  << "\n";
				dout << "percent_age_per_szgroup_matrix.size() "<< percent_age_per_szgroup_matrix.size()  << "\n";
                dout(cout << "a is "<< a  << "\n");
                dout(cout << "sz is "<< sz  << "\n");
                dout(cout << "percent_age_per_szgroup_matrix[sz,a] is "<< percent_age_per_szgroup_matrix[sz][a]  << "\n");
                dout(cout << "weight_at_szgroup[sz]  is "<< weight_at_szgroup[sz]  << "\n");
			}
			*/
            //tot_M_at_age[a] +=  percent_age_per_szgroup_matrix[sz][a] * M_at_szgroup[sz] ;
            tot_W_at_age[a] +=  percent_age_per_szgroup_matrix[sz][a] * weight_at_szgroup[sz] ;
            tot_Mat_at_age[a] +=  percent_age_per_szgroup_matrix[sz][a] * maturity_at_szgroup[sz] ;
        }

		// check
		/*
		if(this->get_name()==10)
		{
            dout(cout << "tot_W_at_age[a]  is "<< tot_W_at_age[a]  << "\n");
		}
		*/
	}

	// then, compute F from the formula N(t+1)=N(t)*exp(-M-F) => F=-M-ln(N(t+1)/N(t))
	// with M=0 here because apply_natural_mortality() actually is applied just AFTER this function

    vector <double> fbar_ages_min_max = this->get_fbar_ages_min_max();
    double FMSY = 1.0;
    if(fbar_ages_min_max.size()>5)  FMSY = fbar_ages_min_max.at(6);

    for(unsigned int a = 0; a < tot_F_at_age.size(); a++)
	{
        if(tot_N_at_age_minus_1.at(a) >0 && tot_N_at_age.at(a)>0)
		{
            perceived_tot_F_at_age.at(a)+= -log(perceived_tot_N_at_age.at(a)/perceived_tot_N_at_age_minus_1.at(a)); // used for perceived stock in the management procedure
            tot_F_at_age.at(a)+= -log(tot_N_at_age.at(a)/tot_N_at_age_minus_1.at(a));  // used for outcomes
            FFmsy.at(a)=tot_F_at_age.at(a)/ FMSY;
            // note that because tot_F_at_age is biased by the growth computation within a year, this making some sz group gaining some individuals at t+1, then we need
            // an alternative proxy computation for the F at age: a running average over months of month F raised to year.
            if(a_month_i!=1)
            {
                tot_F_at_age_running_average.at(a)= (tot_F_at_age_running_average.at(a)+(-log(perceived_tot_N_at_age.at(a)/perceived_tot_N_at_age_minus_1.at(a))*12)) /2;  // used for outcomes
            }
            else
            {
                tot_F_at_age_running_average.at(a)= -log(perceived_tot_N_at_age.at(a)/perceived_tot_N_at_age_minus_1.at(a))*12;
            }
        }
		else
		{
            perceived_tot_F_at_age.at(a)+= 0.0;
            tot_F_at_age.at(a)+= 0.0;
            tot_F_at_age_running_average.at(a) = 0.0;
            FFmsy.at(a)=0.0;
        }

        // prevent negative Fs in few cases where more N at t+1 than at t e.g. from growth and class change in the older ages...
        // should not happen if the growth timing is right.
        if(tot_F_at_age.at(a)<0) tot_F_at_age.at(a)  =0.0;
        if(tot_F_at_age_running_average.at(a)<0) tot_F_at_age_running_average.at(a)  =0.0;

		// => cumul over months
        dout(cout << "tot_N_at_age_minus_1[a]  is "<< tot_N_at_age_minus_1[a]  << "\n");
        dout(cout << "tot_N_at_age[a]  is "<< tot_N_at_age[a]  << "\n");
        dout(cout << "tot_F_at_age[a]  is "<< tot_F_at_age[a]  << "\n");
        dout(cout << "tot_F_at_age_running_average[a]  is "<< tot_F_at_age_running_average[a]  << "\n");
        //dout(cout << "tot_M_at_age[a]  is "<< tot_M_at_age[a]  << "\n");

       // if(this->get_name()==1){
       //     cout << "tot_N_at_age_minus_1[a]  is "<< tot_N_at_age_minus_1[a]  << "\n";
       //     cout << "tot_N_at_age[a]  is "<< tot_N_at_age[a]  << "\n";
       //     cout << "tot_F_at_age[a]  is "<< tot_F_at_age[a]  << "\n";
       // }

	}

	// set the tot N and F at age
	this->set_tot_N_at_age(tot_N_at_age);
    this->set_perceived_tot_N_at_age(perceived_tot_N_at_age);
    this->set_FFmsy(FFmsy);
    this->set_tot_F_at_age(tot_F_at_age);
    this->set_tot_F_at_age_running_average(tot_F_at_age_running_average);
    this->set_perceived_tot_F_at_age(perceived_tot_F_at_age);
    //this->set_tot_M_at_age(tot_M_at_age);
	this->set_tot_W_at_age(tot_W_at_age);
    this->set_tot_Mat_at_age(tot_Mat_at_age);

    dout(cout << "END compute_tot_N_and_F_and_W_at_age() "  << "\n");

}




void Population::compute_tot_M_at_age()
{
    dout(cout << "BEGIN compute_tot_M_at_age() for pop " << this->get_name()  << "\n");

    vector <double> tot_M_at_age = this->get_tot_M_at_age();
    vector <double> a_tot_N_at_szgroup         =this->get_tot_N_at_szgroup();
    vector <double> a_tot_N_at_szgroup_before_applying_M=this->get_a_tot_N_at_szgroup_before_applying_M();
    vector <double> tot_N_at_age (tot_M_at_age.size());
    vector <double> tot_N_at_age_before_M (tot_M_at_age.size());

    int nbsz = percent_szgroup_per_age_matrix.size();
    int nbages = percent_szgroup_per_age_matrix[0].size();

     for(int sz = 0; sz < nbsz; sz++)
    {
        for(int a = 0; a < nbages; a++)
        {
//if(this->get_name() ==2)    cout << "FOR sz  " << sz << " and age " << a  << "\n";
//if(this->get_name() ==2)      cout << "a_tot_N_at_szgroup[sz]   " << a_tot_N_at_szgroup[sz]   << "\n";
                  tot_N_at_age[a] +=  percent_szgroup_per_age_matrix[sz][a] * a_tot_N_at_szgroup[sz] ;
//if(this->get_name() ==2)      cout << "FOR2 sz  " << sz << " and age " << a  << "\n";
//if(this->get_name() ==2)      cout << "a_tot_N_at_szgroup_before_applying_M[sz]   " << a_tot_N_at_szgroup_before_applying_M[sz]   << "\n";
                  tot_N_at_age_before_M[a] +=  percent_szgroup_per_age_matrix[sz][a] * a_tot_N_at_szgroup_before_applying_M[sz] ;
        }
    }

//if(this->get_name() ==2)      cout << "comput the log " << "\n";

     for(unsigned int a = 0; a < tot_M_at_age.size(); a++)
    {
//if(this->get_name() ==2)        cout << "FOR age " << a  << "\n";

         if(tot_N_at_age_before_M.at(a) >0 && tot_N_at_age.at(a)>0)
        {
            tot_M_at_age.at(a)+= -log(tot_N_at_age.at(a)/tot_N_at_age_before_M.at(a));  // used for outcomes
        }
        else
        {
            tot_M_at_age.at(a)+= 0;
        }
//if(this->get_name() ==2)  cout << " tot_M_at_age.at(a) is " <<  tot_M_at_age.at(a)  << "\n";
    }
//if(this->get_name() ==2)    cout << "set the new M at age " << "\n";

   this->set_tot_M_at_age(tot_M_at_age);


     // don´t forget to update the perceived N with the N affected by the natural mortality
     this->set_perceived_tot_N_at_age(tot_N_at_age);
     this->set_tot_N_at_age(tot_N_at_age);

   dout(cout << "END compute_tot_M_at_age() "  << "\n");
}



void Population::clear_tot_F_at_age()
{
	for(unsigned int a = 0; a < tot_F_at_age.size(); a++)
	{
		tot_F_at_age[a]=0;
		tot_F_at_age_last_quarter[a]=0;
        tot_F_at_age_running_average[a]=0;
    }
}

void Population::clear_tot_M_at_age()
{
    for(unsigned int a = 0; a < tot_M_at_age.size(); a++)
    {
        tot_M_at_age[a]=0;
    }
}


void Population::clear_tot_C_at_szgroup()
{
    for(unsigned int sz = 0; sz < tot_C_at_szgroup.size(); sz++)
    {
        tot_C_at_szgroup[sz]=0.0;
    }
}

void Population::clear_tot_D_at_szgroup()
{
    for(unsigned int sz = 0; sz < tot_D_at_szgroup.size(); sz++)
    {
        tot_D_at_szgroup[sz]=0.0;
    }
}


void Population::compute_fbar()
{
    dout(cout<< "compute fbar for pop..." << this->get_name() << "\n");

    vector <double> a_tot_F_at_age_1  = this->get_tot_F_at_age_running_average(); // perceived
    vector <double> a_tot_F_at_age_2 = this->get_tot_F_at_age();


    dout(cout<< "compute fbar..." << "\n");
	double fbar=0;
	int age_min =this->fbar_ages_min_max.at(0);
	int age_max =this->fbar_ages_min_max.at(1);
    if(age_max==0 || (age_max < age_min))
	{
        dout(cout << "age_max at 0 for this pop??" << "\n");
		age_max=5;
	}


    if(age_max == age_min)
    {
        this->set_fbar_type1(a_tot_F_at_age_1[age_max]);
        this->set_fbar_type2(a_tot_F_at_age_2[age_max]);
    }

    for(int a = age_min; a < age_max; a++)
	{
								 // sum...
        fbar_type1+= a_tot_F_at_age_1[a];
        fbar_type2 += a_tot_F_at_age_2[a];
    }
								 // then do the average...
    dout(cout<< "sum fbar..." << fbar << "\n");
    fbar_type1= fbar_type1 /((fbar_ages_min_max.at(1)-fbar_ages_min_max.at(0)) +1);
    this->set_fbar_type1(fbar_type1);

    fbar_type2 = fbar_type2 / ((fbar_ages_min_max.at(1) - fbar_ages_min_max.at(0)) + 1);
    this->set_fbar_type2(fbar_type2);

    dout(cout<< "fbar type 1..." << fbar_type1 << "\n");
    dout(cout << "fbar type 2..." << fbar_type2 << "\n");
    dout(cout<< "compute fbar...ok" << "\n");
  
}


vector<double> Population::compute_SSB()
{
    // first of all, get the N
    vector <double> a_tot_N_at_szgroup=this->get_tot_N_at_szgroup();

    vector <double> SSB_per_szgroup (a_tot_N_at_szgroup.size());
    dout(cout << "compute SSB given pop is " << this->get_name()  << " " << "\n");
    for (unsigned int i = 0; i < a_tot_N_at_szgroup.size(); i++)
    {

        // reminder: tot_N_at_szgroup are in thousand in input file
        //  but in absolute numbers here because have been multiplied by 1000 when importing
        SSB_per_szgroup.at(i) = weight_at_szgroup.at(i) * a_tot_N_at_szgroup.at(i) * maturity_at_szgroup.at(i);
    }
    

    // check numbers
    /*cout << "given a_tot_N_at_szgroup; ";
    for (unsigned int i = 0; i < a_tot_N_at_szgroup.size(); i++)
    {
        cout << "sz" << i << " is " << a_tot_N_at_szgroup.at(i) << " ";
    }
    cout << "\n";
    cout << "given maturity_at_szgroup; " ;
    for (unsigned int i = 0; i < maturity_at_szgroup.size(); i++)
    {
        cout << "sz" << i << " is " << maturity_at_szgroup.at(i) << " ";
    }
    cout << "\n";
    cout << "given weight_at_szgroup; ";
    for (unsigned int i = 0; i < weight_at_szgroup.size(); i++)
    {
        cout << "sz" << i << " is " << weight_at_szgroup.at(i) << "kg ";
    }
    cout << "\n";
    */
    

  return(SSB_per_szgroup);
}


double Population::compute_proportion_mature_fish()
{
    // first of all, get the N
    vector <double> a_tot_N_at_szgroup=this->get_tot_N_at_szgroup();
    vector <double> a_maturity_at_szgroup=this->get_maturity_at_szgroup();

    double totN_mature_fish=0.0;
    double totN=0.0;
    for(unsigned int i = 0; i < a_tot_N_at_szgroup.size(); i++)
    {
       totN_mature_fish+=a_tot_N_at_szgroup.at(i)*a_maturity_at_szgroup.at(i);
       totN+=a_tot_N_at_szgroup.at(i);
    }

  return((totN>0) ? totN_mature_fish/totN : 0.0);
}




void Population::apply_natural_mortality()
{
    dout(cout << "BEGIN apply_natural_mortality() "  << "\n");

								 // init
	vector <double> new_tot_N_at_szgroup (tot_N_at_szgroup.size());

	// aggregate from nodes
	//aggregate_N();

	// get Mortality
	vector <double>  M_at_szgroup= this->get_M_at_szgroup();

	for(unsigned int i=0; i<new_tot_N_at_szgroup.size(); i++)
	{
								 // divide according to tstep
		new_tot_N_at_szgroup[i] =  tot_N_at_szgroup[i] *exp(-M_at_szgroup[i]/12);
	}							 // annual M divided by nb of tstep (not relevant for spatial dim)

	// set the tot N at szgroup
	this->set_tot_N_at_szgroup(new_tot_N_at_szgroup);

	// redistribute on nodes
	//distribute_N();

    dout(cout << "END apply_natural_mortality() "  << "\n");

}


void Population::export_popdyn_N(ofstream& popdyn_N, int tstep)
{

	//this->aggregate_N(); // get total N from summing up N over nodes
	popdyn_N << setprecision(0) << fixed;
	// tstep / pop / tot N at szgroup
	popdyn_N << tstep << " " << this->get_name() << " ";
	vector <double>tot_N_at_szgroup=this->get_tot_N_at_szgroup();
	for(unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
	{
								 // output in thousands of individuals
		popdyn_N  << tot_N_at_szgroup.at(sz) / 1000 << " " ;
	}
	popdyn_N << " " <<  "\n";
}


void Population::export_popdyn_F(ofstream& popdyn_F, int tstep)
{

    popdyn_F << setprecision(6) << fixed;
	// tstep / pop / F at szgroup
	popdyn_F << tstep << " " << this->get_name() << " ";
	vector <double>tot_F_at_age=this->get_tot_F_at_age();
	for(unsigned int a = 0; a < tot_F_at_age.size(); a++)
	{
								 // output F in CUMUL over months, caution!
		popdyn_F  << tot_F_at_age.at(a)  << " " ;
	}
	popdyn_F << " " <<  "\n";
}

void Population::export_popdyn_SSB(ofstream& popdyn_SSB, int tstep)
{

    popdyn_SSB << setprecision(6) << fixed;
    // tstep / pop / SSB at szgroup
    popdyn_SSB << tstep << " " << this->get_name() << " ";
    vector <double>SSB_at_szgroup=this->get_SSB_at_szgroup();
    for(unsigned int sz = 0; sz < SSB_at_szgroup.size(); sz++)
    {
                                 // output SSB
        popdyn_SSB  << SSB_at_szgroup.at(sz)  << " " ;
    }
    popdyn_SSB << " " <<  "\n";
}



void Population::export_popdyn_annual_indic(ofstream& popdyn_annual_indic, int tstep, const DynAllocOptions &dyn_alloc_sce)
{

    dout(cout << "begin export_popdyn_annual_indic..."<< "\n");

    popdyn_annual_indic << setprecision(4) << fixed;
	// tstep / pop / F at szgroup / tot landings on pop i.e. including oth landings as well
	popdyn_annual_indic << tstep << " " << this->get_name() << " ";
	double oth_mult   =this->get_oth_land_multiplier();
	double cpue_mult  =this->get_cpue_multiplier();

								 // output the annual multiplier
	popdyn_annual_indic  << oth_mult << " " << cpue_mult << " ";

    dout(cout<< "when exporting, get fbar for pop..." << this->get_name() << "\n");
    double fbar_type1_py= this->get_fbar_type1();
	popdyn_annual_indic  << fbar_type1_py << " ";
    double fbar_type2_py = this->get_fbar_type2();
    popdyn_annual_indic << fbar_type2_py << " ";

								 //...also including the oth land
    //double so_far    =this->get_landings_so_far();
    //popdyn_annual_indic  << so_far << " ";

    double land_so_far    = 0;
    vector <double> C_at_szgroup= this->get_tot_C_at_szgroup();
    for(unsigned int sz = 0; sz < C_at_szgroup.size(); sz++)
    {
       land_so_far+=C_at_szgroup.at(sz);
    }
    popdyn_annual_indic  << land_so_far << " ";


    double disc_so_far    = 0;
    vector <double> D_at_szgroup= this->get_tot_D_at_szgroup();
    for(unsigned int sz = 0; sz < D_at_szgroup.size(); sz++)
    {
       disc_so_far+=D_at_szgroup.at(sz);
    }
    popdyn_annual_indic  << disc_so_far << " ";

    double tot_SSB    = 0;
    vector <double> SSB_at_szgroup= this->get_SSB_at_szgroup();
    for(unsigned int sz = 0; sz < SSB_at_szgroup.size(); sz++)
    {
       tot_SSB+=SSB_at_szgroup.at(sz);
    }
    popdyn_annual_indic  << tot_SSB << " ";


    dout(cout << "retrieve tacs if any..."<< "\n");

     double last_year_tac=0.0;
     if(dyn_alloc_sce.option(Options::TACs))
     {
         vector<double> tacs = this->get_tac()->get_ts_tac();
         last_year_tac=tacs.at(tacs.size()-2); // because computeTAC() has just been called before this export
     }
     popdyn_annual_indic  << last_year_tac << " ";

     //
     vector <double>N_at_age=this->get_tot_N_at_age();
     for(unsigned int a = 0; a < N_at_age.size(); a++)
     {
        popdyn_annual_indic  << N_at_age.at(a)  << " " ;
     }

     //
     vector <double>F_at_age=this->get_tot_F_at_age();
     for(unsigned int a = 0; a < F_at_age.size(); a++)
     {
        popdyn_annual_indic  << F_at_age.at(a)  << " " ;
     }
                                 // attempt for a calibration to obtain same F
	vector <double>W_at_age=this->get_tot_W_at_age();
	for(unsigned int a = 0; a < W_at_age.size(); a++)
	{
		popdyn_annual_indic  << W_at_age.at(a)  << " " ;
	}

								 // attempt for a calibration to obtain same F
	vector <double>M_at_age=this->get_tot_M_at_age();
	for(unsigned int a = 0; a < M_at_age.size(); a++)
	{
		popdyn_annual_indic  << M_at_age.at(a)  << " " ;
	}

	popdyn_annual_indic << " " <<  "\n";

    dout(cout << "export_popdyn_annual_indic...ok"<< "\n");

}
