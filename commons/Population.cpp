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
#include "Population.h"
#include <helpers.h>

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
                       const multimap<int, types::NodeId> &lst_idx_nodes_per_pop,
                       const multimap<types::NodeId, double> &_full_spatial_availability,
                       const multimap<types::NodeId, double> &field_of_coeff_diffusion_this_pop,
                       const map<types::NodeId, double> &_oth_land,
                       const multimap<int, double> &overall_migration_fluxes,
                       const map<string, double> &relative_stability_key,
                       const vector<vector<double> > &_percent_szgroup_per_age_matrix,
                       const vector<vector<double> > &_percent_age_per_szgroup_matrix,
                       const vector<vector<double> > &_growth_transition_matrix,
                       const vector<Node *> &nodes,
                       const vector<double> &_fbar_ages_min_max,
                       const vector<double> &init_tac,
                       double tac_percent_simulated,
                       double hyperstability_param,
                       double landings_so_far,
                       double a_calib_cpue_multiplier,
                       double a_calib_weight_at_szgroup)
{
    UNUSED(lst_idx_nodes_per_pop);
    UNUSED(landings_so_far);

    name=a_name;
    pop_name=a_pop_name;

    dout(cout << "create pop " << name << endl);

	full_spatial_availability= _full_spatial_availability;

	oth_land= _oth_land;

	growth_transition_matrix=_growth_transition_matrix;
	percent_szgroup_per_age_matrix=_percent_szgroup_per_age_matrix;
	percent_age_per_szgroup_matrix=_percent_age_per_szgroup_matrix;

    nb_szgroups=init_tot_N_at_szgroup.size();
    nb_ages=percent_szgroup_per_age_matrix[0].size();

	param_sr=_param_sr;

	landings_so_far= 0.0;

	// init...
    dout(cout << "init..." << name << endl);

    // a quick check
    if(init_tot_N_at_szgroup.empty() || init_tot_N_at_szgroup.size()==0)
     {
        cout << "No N_at_szgroup found!! Check init_pops_per_szgroup. dat input file, correct and re-run..." << endl;
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
		proprecru_at_szgroup.push_back(0);
        SSB_at_szgroup.push_back(0);

	}
    for(unsigned int a=0; a<nb_ages; a++)
	{
		tot_F_at_age.push_back(0);
        tot_M_at_age.push_back(0);
        FFmsy.push_back(0);
    }
    for(unsigned int a=0; a<nb_ages; a++)
	{
		tot_F_at_age_last_quarter.push_back(0);
	}

    // for catch equation
    dout(cout << "loading parameters for the catch equation" << name << endl);
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

    dout(cout << "calib..." << name << endl);
    // CALIB: if increased then improve the catch rate of the simulated vessels....
    // look at the config.dat for calibration values

								 // set to 1 the first year
	this->set_cpue_multiplier(1*a_calib_cpue_multiplier);

	// ...then fill in with start pop
    dout(cout << "set the overall N_at_szgroup..." << name << endl);
                                 // set the overall N_at_szgroup
	this->set_tot_N_at_szgroup(init_tot_N_at_szgroup);

    // ...then fill in with start pop
                                 // set the overall N_at_szgroup
    this->set_true_tot_N_at_szgroup(init_tot_N_at_szgroup);

    // ...then fill in with start info
                                 // set the migrant prop
    dout(cout << "set the prop_migrants..." << name << endl);
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

    dout(cout << "calib the weight at szgroup..." << name << endl);
    for(unsigned int i=0; i < nb_szgroups; i++)
    {
        init_weight_at_szgroup.at(i)=init_weight_at_szgroup.at(i)*a_calib_weight_at_szgroup;
	}

	this->set_weight_at_szgroup(init_weight_at_szgroup);

    dout(cout << "fill in with start pop..." << name << endl);
    // ...then fill in with start pop
    dout(cout << "for comcat..." << name << endl);
    this->set_comcat_at_szgroup(init_comcat_at_szgroup);

	// ...then fill in with start pop
    dout(cout << "for maturity..." << name << endl);
    this->set_maturity_at_szgroup(init_maturity_at_szgroup);

	// ...then fill in with start pop
    dout(cout << "for fecundity..." << name << endl);
    this->set_fecundity_at_szgroup(init_fecundity_at_szgroup);

	// ...then fill in with start pop
    dout(cout << "for natural mortality..." << name << endl);
    for(unsigned int i=0; i < init_M_at_szgroup.size(); i++)
	{
		init_M_at_szgroup.at(i)=init_M_at_szgroup.at(i);
	}

	this->set_M_at_szgroup(init_M_at_szgroup);

	// ...then fill in with start pop
    dout(cout << "for prop recru..." << name << endl);
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
    dout(cout << "set up the list of nodes for this pop" << name << endl);

    vector<Node* > p_spe_nodes;
    for(auto iter=full_spatial_availability.begin(); iter != full_spatial_availability.end();
		iter = full_spatial_availability.upper_bound( iter->first ) )
	{
        dout(if(iter->first.toIndex()>nodes.size()) cout << "error in the avai field: trying to push back node idx " << iter->first.toIndex() << " for max nb of nodes " << nodes.size() << endl);
        p_spe_nodes.push_back (nodes[  iter->first.toIndex()  ]);
        nodes[ iter->first.toIndex() ]->set_pop_names_on_node(a_name);;

	}
    for(unsigned int i=0; i<p_spe_nodes.size(); i++)
		list_nodes.push_back(p_spe_nodes[i]);

	// add these Ns to the multimap of the concerned nodes
    dout(cout << " lst nodes: " << endl);
	for(unsigned int i=0; i<list_nodes.size(); i++)
	{
								 // caution: here is tot N on the node! need to call distribute_N()
		list_nodes.at(i)->set_Ns_pops_at_szgroup(a_name, tot_N_at_szgroup);
        dout(cout  << list_nodes.at(i)->get_idx_node().toIndex() << " ");
	}

    dout(cout << endl);


    dout(cout << "field_of_coeff_diffusion_this_pop " << name << endl);

    // for diffusion of N per szgroup
    this->set_field_of_coeff_diffusion_this_pop(field_of_coeff_diffusion_this_pop);


	// distribute tot_N_at_szgroup on nodes knowing the avai spatial key
	// i.e. update the multimap Ns_pops_at_szgroup of the nodes
	this->distribute_N();		 // divide on nodes according to avai

	// init the N on node at month start
	for(unsigned int i=0; i<list_nodes.size(); i++)
	{
		vector<double> N_at_szgroup=list_nodes.at(i)->get_Ns_pops_at_szgroup(a_name);
		list_nodes.at(i)->set_Ns_pops_at_szgroup_at_month_start(name, N_at_szgroup);

	}

    dout(cout << "init tac " << name << endl);

    // init tac
	tac = new Tac(init_tac[0], tac_percent_simulated, relative_stability_key);
    fbar=0.0;
    oth_land_multiplier=1.0;

    quota =init_tac[0];
    quota_uptake=0.0;
    is_choking_fisheries=0;

	// init related to F
	fbar_ages_min_max=_fbar_ages_min_max;

    // other MSFD D3
    proportion_mature_fish=1.0;

    // hyperstability
    this->set_hyperstability_param(hyperstability_param);


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
    return(name);
}

string Population::get_pop_name() const
{
    return(pop_name);
}

int Population::get_nb_szgroups() const
{
    return(nb_szgroups);
}

int Population::get_nb_ages() const
{
    return(nb_ages);
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

double Population::get_fbar() const
{
    return(fbar);
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

const vector<double>& Population::get_landings_at_end_of_years() const
{
    return(landings_at_end_of_years);
}


const vector<double>& Population::get_param_sr() const
{
	return(param_sr);
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

multimap<int,double>  Population::get_overall_migration_fluxes() const
{
    return(overall_migration_fluxes);
}



map<types::NodeId,double>  Population::get_oth_land() const
{
	return(oth_land);
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

void Population::set_fbar(double _fbar)
{
    SSB=_fbar;
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


void Population::set_full_spatial_availability(multimap<types::NodeId,double> _full_spatial_availability)
{
	full_spatial_availability= _full_spatial_availability;
}

void Population::set_field_of_coeff_diffusion_this_pop(multimap<types::NodeId,double> _field_of_coeff_diffusion_this_pop)
{
    field_of_coeff_diffusion_this_pop= _field_of_coeff_diffusion_this_pop;
}



void Population::set_overall_migration_fluxes(multimap<int,double> _overall_migration_fluxes)
{
    overall_migration_fluxes= _overall_migration_fluxes;
}


void Population::set_oth_land(map<types::NodeId,double> _oth_land)
{
	oth_land= _oth_land;
}


void Population::set_oth_land_multiplier(double _oth_land_multiplier)
{
	oth_land_multiplier= _oth_land_multiplier;
}


void Population::set_list_nodes(vector<Node* > _list_nodes)
{
	list_nodes= _list_nodes;
}


void Population::distribute_N()
{

    dout (cout<< endl);
    dout(cout<< "BEGIN distribute_N(): distribute on nodes for the pop " << name << endl);
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
		vector<double> avai_this_node;
        auto lower = full_spatial_availability.lower_bound(idx_node);
        auto upper = full_spatial_availability.upper_bound(idx_node);
        for (auto pos=lower; pos != upper; pos++)
			avai_this_node.push_back(pos->second);

		// check avai
		//cout << "avai on node "<< idx_node<< ":" << endl;
		//for(int i=0; i<avai_this_node.size(); i++)
        //    dout(cout << avai_this_node[i] << " ");
		//cout << endl;

		// check tot_N_at_szgroup
		//cout << "tot_N_at_szgroup on node "<< idx_node<< ":" << endl;
		//for(int i=0; i<tot_N_at_szgroup.size(); i++)
        //    dout(cout << tot_N_at_szgroup[i] << " ");
		//cout << endl;

		// distribute on node applying avai
		vector <double> N_at_szgroup;
        for(unsigned int i=0; i<tot_N_at_szgroup.size(); i++)
			N_at_szgroup.push_back( tot_N_at_szgroup.at(i)*avai_this_node.at(i) );

        /*dout(cout << "N at szgroup on the node "<< idx_node<< ":" << endl;
        for(int i=0; i<N_at_szgroup.size(); i++)
           cout << N_at_szgroup[i] << " ";
           cout << endl
         );
         */

		// set the new Ns for this specific pop in the multimap of this node
        //dout(cout << "set the new Ns for this specific pop "<< idx_node<< ":" << endl);
        list_nodes[idx]->set_Ns_pops_at_szgroup(name, N_at_szgroup);

       // if(this->get_name()==1) for(int i=0; i<N_at_szgroup.size(); i++)
       //    all_N_at_szgroup[i]+=N_at_szgroup[i];

    }

     //if(this->get_name()==1)  for(int i=0; i<all_N_at_szgroup.size(); i++)
     //    cout << "in distribute_N: all_N_at_szgroup["<< i <<"] is " << all_N_at_szgroup[i];

    dout(cout<< "END distribute_N()"<< endl);
    dout(cout<< endl);

}


void Population::aggregate_N()
{

    dout(cout<< endl);
    dout(cout<< "BEGIN aggregate_N(): aggregate from nodes for the pop " << name << endl);

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
				agg_Ns_at_szgroup.at(i)= agg_Ns_at_szgroup.at(i)+Ns_this_node.at(i);
			}

		}
	}							 // end for-loop over nodes

	// set the new tot Ns for this specific pop in the vector tot_N_at_szgroup of the pop
	this->set_tot_N_at_szgroup(agg_Ns_at_szgroup);

	// check
	//if(this->get_name()==10)
	//{
    //    dout(cout << "in aggregate(): tot N at szgroup for the pop "<< this->get_name() << " after aggregation:"  << endl);
	//    for(int i=0; i<tot_N_at_szgroup.size(); i++)
	//    {
    //        dout(cout << tot_N_at_szgroup[i] << " ");
	//    }
	//}

    dout(cout << endl);

    dout(cout<< "END aggregate_N()" << endl);
    dout(cout<< endl);
}



void Population::diffuse_N_from_field(adjacency_map_t& adjacency_map)
{

   cout << "start diffusion for this pop...." << endl;

    vector<Node*> list_of_nodes = this->get_list_nodes();
    vector<types::NodeId> list_of_nodes_idx;
    random_shuffle (list_of_nodes.begin(), list_of_nodes.end() );
    for (int n=0; n<list_of_nodes.size(); ++n)
       {
       list_of_nodes_idx.push_back(list_of_nodes.at(n)->get_idx_node());
       }

    for (int n=0; n<list_of_nodes.size(); ++n)
       {
        auto idx_node=list_of_nodes.at(n)->get_idx_node();

        // get coeff of diffusion per szgroup for this node
        auto field_of_coeff_diffusion_this_pop = this->get_field_of_coeff_diffusion_this_pop();
        vector<double> coeff;
        auto lower = field_of_coeff_diffusion_this_pop.lower_bound(idx_node);
        auto upper = field_of_coeff_diffusion_this_pop.upper_bound(idx_node);
        for (auto pos=lower; pos != upper; pos++)
            coeff.push_back(pos->second);


        // get the N for this pop on this node
        vector<double> departure_N = list_of_nodes.at(n)->get_Ns_pops_at_szgroup(this->get_name());


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

        std::unique(neighbour_nodes.begin(), neighbour_nodes.end());

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
               for (int nei=0; nei<count; ++nei)
                  {
                   vector <double> arrival_N = list_of_nodes.at(neighbour_nodes_on_spatial_extent.at(nei))->get_Ns_pops_at_szgroup( this->get_name() );
                   for (int sz=0; sz<arrival_N.size(); ++sz)
                      {
                      double exchanged       = ((coeff.at(sz)*depN.at(sz))/count);
                      arrival_N.at(sz)       = arrival_N.at(sz) + exchanged;
                      departure_N.at(sz)     = departure_N.at(sz) - exchanged;
                     }
                   list_of_nodes.at(neighbour_nodes_on_spatial_extent.at(nei))->set_Ns_pops_at_szgroup( this->get_name(), arrival_N );//update arrival
                   }
               list_of_nodes.at(n)->set_Ns_pops_at_szgroup( this->get_name(), departure_N ); //update departure

       } // count!=0

      } // node by node

  cout << "stop diffusion for this pop...." << endl;
}






void Population::do_growth()
{
    dout(cout << "BEGIN do_growth() "  << endl );

//	int namepop = this->get_name();

								 // init
	vector <double> new_tot_N_at_szgroup (tot_N_at_szgroup.size());

	// aggregate from nodes
	//aggregate_N();

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
            //     dout(cout << "i is "<< i  << endl);
            //     dout(cout << "j is "<< j  << endl);
            //     dout(cout << "growth[i,j] is "<< growth_transition_matrix[i][j]  << endl);
            //     dout(cout << "tot_N_at_szgroup[i]  is "<< tot_N_at_szgroup[i]  << endl);
			//}
			new_tot_N_at_szgroup[i] +=  growth_transition_matrix[i][j] * tot_N_at_szgroup[j] ;
		}
	}

	// set the tot N at szgroup
	this->set_tot_N_at_szgroup(new_tot_N_at_szgroup);

	// redistribute on nodes
	//distribute_N();

    dout(cout << "END do_growth() "  << endl );
}


void Population::apply_overall_migration_fluxes(vector<Population* >& populations)
{
    dout(cout << "BEGIN overall_migration_fluxes() "  << endl );
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


 cout << "departure pop is " << this->get_name() << endl;
 cout << "arrival_pop is " << arrival_pop << endl;
 cout << "sz is " << sz << endl;
 cout << "flux_prop is " << flux_prop << endl;

           // input
          vector<double> N_at_szgroup_this_pop = this->get_tot_N_at_szgroup();
          vector<double> N_at_szgroup_arr_pop  = populations.at(arrival_pop)->get_tot_N_at_szgroup();
          vector<double> weight_at_szgroup_this_pop = this->get_weight_at_szgroup();
          vector<double> weight_at_szgroup_arr_pop = populations.at(arrival_pop)->get_weight_at_szgroup();

          if((sz+1)>=N_at_szgroup_this_pop.size()) sz = 0; // reinit because next pop

 cout << " before: N_at_szgroup_this_pop.at(sz) is " <<  N_at_szgroup_this_pop.at(sz) << endl;
 cout << " before: N_at_szgroup_arr_pop.at(sz) is " <<  N_at_szgroup_arr_pop.at(sz) << endl;

 cout << " before: weight_at_szgroup_arr_pop.at(sz) is " <<  weight_at_szgroup_arr_pop.at(sz) << endl;

 // impact the Ns from emigration/immigration specified in the multimap
          double tot_before_in_arr = N_at_szgroup_arr_pop.at(sz);
          double tot_to_in_arr     = N_at_szgroup_this_pop.at(sz)*flux_prop;
          double tot_now_in_arr    = N_at_szgroup_arr_pop.at(sz)+(N_at_szgroup_this_pop.at(sz)*flux_prop);
          double tot_now_in_dep    = N_at_szgroup_this_pop.at(sz)*(1-flux_prop);

          N_at_szgroup_arr_pop.at(sz)  = tot_now_in_arr;
          N_at_szgroup_this_pop.at(sz) = tot_now_in_dep;

 cout << " after: N_at_szgroup_this_pop.at(sz) is " <<  N_at_szgroup_this_pop.at(sz) << endl;
 cout << " after: N_at_szgroup_arr_pop.at(sz) is " <<  N_at_szgroup_arr_pop.at(sz) << endl;


          // do a weighted average of the weight
          weight_at_szgroup_arr_pop.at(sz)  = (tot_to_in_arr/tot_now_in_arr) *weight_at_szgroup_arr_pop.at(sz) +
                                                    (tot_before_in_arr/tot_now_in_arr)* weight_at_szgroup_this_pop.at(sz);

cout << " after: weight_at_szgroup_arr_pop.at(sz) is " <<  weight_at_szgroup_arr_pop.at(sz) << endl;

          // output this pop
          this->set_tot_N_at_szgroup(N_at_szgroup_this_pop);


          // output arrival pop
          populations.at(arrival_pop)->set_tot_N_at_szgroup(N_at_szgroup_arr_pop);

          // ....including affecting the weight-at-szgroup
          populations.at(arrival_pop)->set_weight_at_szgroup(weight_at_szgroup_arr_pop);


          sz +=1;
       }
      } else{

       dout(cout << "no migration for this pop" << this->get_name()  << endl );

       cout << "no migration for this pop " << this->get_name()   << endl;

    }


    dout(cout << "END overall_migration_fluxes() "  << endl );
}



void Population::add_recruits_from_SR()
{
    dout(cout << "BEGIN add_recruits() form SR "  << endl );

	// compute SSB
    // first of all, get the true N
    vector <double> a_tot_N_at_szgroup=this->get_tot_N_at_szgroup();
    vector <double> prop_migrants=this->get_prop_migrants_in_tot_N_at_szgroup();
    for(unsigned int sz = 0; sz < a_tot_N_at_szgroup.size(); sz++)
    {
       a_tot_N_at_szgroup.at(sz) = a_tot_N_at_szgroup.at(sz) - (a_tot_N_at_szgroup.at(sz) *prop_migrants.at(sz)); // retrieve the true N
    }


    vector <double> SSB_per_szgroup (a_tot_N_at_szgroup.size());
    dout(cout << "pop is " << this->get_name()  << " " << endl );
    for(unsigned int i = 0; i < a_tot_N_at_szgroup.size(); i++)
    {

       // reminder: tot_N_at_szgroup are in thousand in input file
       //  but in absolute numbers here because have been multiplied by 1000 when importing
       SSB_per_szgroup.at(i) =  weight_at_szgroup.at(i) * a_tot_N_at_szgroup.at(i) * maturity_at_szgroup.at(i);
       dout(cout << "szgroup is " << i  << " " << endl );
       cout << "tot_N_at_szgroup is " << a_tot_N_at_szgroup.at(i)  << " " << endl ;
       cout << "maturity_at_szgroup is " << maturity_at_szgroup.at(i)  << " " << endl ;
       cout << "weight_at_szgroup is " << weight_at_szgroup.at(i)  << " kg" << endl ;

    }



	// ...then, cumul for getting tot SSB (here in kilos)
	SSB=0;
    for(unsigned int i = 0; i < SSB_per_szgroup.size(); i++)
	{
		SSB +=  SSB_per_szgroup.at(i);
	}
    SSB= SSB/1000;			 //
    cout << "SSB is " << SSB  << " tons" << endl ;
    //dout(cout << "SSB is " << SSB  << " tons" << endl );

	// compute R from SSB-R relationship
	// (caution: age dependent, e.g. SSB-R for cod 2532 is usually simulated for age2)...
	double recruits =(param_sr[0]*SSB*exp(-param_sr[1]*SSB)) * 1000;
    dout(cout << "New recruits are " << recruits  << endl );

    // add stochasticity on recruits (MAGIC NUMBER default: lognormal with CV at 20%)
	// TO DO: use a stock-specific input there...
    double sd=param_sr[2];
    if(sd==0.0) sd=0.2; // default
	double rec_error=0;
	rec_error= exp( 0 + sd*norm_rand() ) / exp((sd*sd)/2.0);
	recruits= recruits * rec_error;
    //dout(cout << "stochastic recruits are " << recruits  << endl );
    cout << "stochastic recruits are " << recruits  << endl ;

    // init
    vector <double> new_tot_N_at_szgroup (SSB_per_szgroup.size());


    // ...then distribute among szgroup
    for(unsigned int i = 0; i < SSB_per_szgroup.size(); i++)
	{
        new_tot_N_at_szgroup[i] =  a_tot_N_at_szgroup.at(i) + (recruits* proprecru_at_szgroup.at(i));
        cout << "recruits for szgroup " << i << ": " << recruits* proprecru_at_szgroup.at(i) << " to add to N this grp " << endl ;
        //dout(cout << "for szgroup " << i << ": " << recruits* proprecru_at_szgroup.at(i)  << endl );
    }

	//if(this->get_name()==29){
	//int a_int;
	//cin >> a_int;
	//}

	// set the tot N at szgroup
	this->set_tot_N_at_szgroup(new_tot_N_at_szgroup);

	// redistribute on nodes
	//distribute_N();

    dout(cout << "END add_recruits() "  << endl );
}

void Population::add_recruits_from_a_fixed_number()
{
    dout(cout << "BEGIN add_recruits() form SR "  << endl );

    // first of all, get the true N
    vector <double> a_tot_N_at_szgroup=this->get_tot_N_at_szgroup();
    vector <double> prop_migrants=this->get_prop_migrants_in_tot_N_at_szgroup();
    for(unsigned int sz = 0; sz < a_tot_N_at_szgroup.size(); sz++)
    {
       a_tot_N_at_szgroup.at(sz) = a_tot_N_at_szgroup.at(sz) - (a_tot_N_at_szgroup.at(sz) *prop_migrants.at(sz)); // retrieve the true N
    }


    // init
    vector <double> new_tot_N_at_szgroup (a_tot_N_at_szgroup.size());


    // assign a fixed numbernt, e.g. SSB-R for cod 2532 is usually simulated for age2)...
    double recruits =param_sr[0];
    dout(cout << "New recruits are " << recruits  << endl );

    // add stochasticity on recruits (MAGIC NUMBER default: lognormal with CV at 20%)
    // TO DO: use a stock-specific input there...
    double sd=param_sr[2];
    if(sd==0.0) sd=0.2; // default
    double rec_error=0;
    rec_error= exp( 0 + sd*norm_rand() ) / exp((sd*sd)/2.0);
    recruits= recruits * rec_error;
    //dout(cout << "stochastic recruits are " << recruits  << endl );
    cout << "stochastic recruits are " << recruits  << endl ;

    // ...then distribute among szgroup
    for(unsigned int i = 0; i < a_tot_N_at_szgroup.size(); i++)
    {
        new_tot_N_at_szgroup[i] =  a_tot_N_at_szgroup.at(i) + (recruits* proprecru_at_szgroup.at(i));
        cout << "recruits for szgroup " << i << ": " << recruits* proprecru_at_szgroup.at(i) << " to add to N this grp "  << a_tot_N_at_szgroup.at(i) << endl ;
        //dout(cout << "for szgroup " << i << ": " << recruits* proprecru_at_szgroup.at(i)  << endl );
    }

    //if(this->get_name()==29){
    //int a_int;
    //cin >> a_int;
    //}

    // set the tot N at szgroup
    this->set_tot_N_at_szgroup(new_tot_N_at_szgroup);

    // redistribute on nodes
    //distribute_N();

    dout(cout << "END add_recruits() "  << endl );
}


void Population::add_recruits_from_eggs()
{
    dout(cout << "BEGIN add_recruits() from eggs "  << endl );

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

    dout(cout << "END add_recruits() "  << endl);
}


void Population::compute_tot_N_and_F_and_W_at_age()
{
    cout << "BEGIN compute_tot_N_and_F_and_W_at_age() for pop " << this->get_name()  << endl ;

	vector <double> tot_F_at_age = get_tot_F_at_age();
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
				dout << "pop "<< this->get_name()  << endl;
				dout << "percent_szgroup_per_age_matrix.size() "<< percent_szgroup_per_age_matrix.size()  << endl;
				dout << "a is "<< a  << endl;
				dout << "sz is "<< sz  << endl;
				dout << "percent_szgroup_per_age_matrix[sz,a] is "<< percent_szgroup_per_age_matrix[sz][a]  << endl;
				dout << "tot_N_at_szgroup[sz]  is "<< tot_N_at_szgroup[sz]  << endl;
				dout << "tot_N_at_szgroup_month_minus_1[sz]  is "<< tot_N_at_szgroup_month_minus_1[sz] << endl;
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
				dout << "pop "<< this->get_name()  << endl;
				dout << "percent_age_per_szgroup_matrix.size() "<< percent_age_per_szgroup_matrix.size()  << endl;
                dout(cout << "a is "<< a  << endl);
                dout(cout << "sz is "<< sz  << endl);
                dout(cout << "percent_age_per_szgroup_matrix[sz,a] is "<< percent_age_per_szgroup_matrix[sz][a]  << endl);
                dout(cout << "weight_at_szgroup[sz]  is "<< weight_at_szgroup[sz]  << endl);
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
            dout(cout << "tot_W_at_age[a]  is "<< tot_W_at_age[a]  << endl);
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
        }
		else
		{
            perceived_tot_F_at_age.at(a)+= 0;
            tot_F_at_age.at(a)+= 0;
            FFmsy.at(a)=0;
        }

        // prevent negative Fs in few cases where more N at t+1 than at t e.g. from growth and class change in the older ages...
        // should not happen if the growth timing is right.
        if(tot_F_at_age.at(a)<0) tot_F_at_age.at(a)  =0.0;

		// => cumul over months
        dout(cout << "tot_N_at_age_minus_1[a]  is "<< tot_N_at_age_minus_1[a]  << endl);
        dout(cout << "tot_N_at_age[a]  is "<< tot_N_at_age[a]  << endl);
        dout(cout << "tot_F_at_age[a]  is "<< tot_F_at_age[a]  << endl);
        //dout(cout << "tot_M_at_age[a]  is "<< tot_M_at_age[a]  << endl);

        if(this->get_name()==1){
            cout << "tot_N_at_age_minus_1[a]  is "<< tot_N_at_age_minus_1[a]  << endl;
            cout << "tot_N_at_age[a]  is "<< tot_N_at_age[a]  << endl;
            cout << "tot_F_at_age[a]  is "<< tot_F_at_age[a]  << endl;
        }

	}

	// set the tot N and F at age
	this->set_tot_N_at_age(tot_N_at_age);
    this->set_perceived_tot_N_at_age(perceived_tot_N_at_age);
    this->set_FFmsy(FFmsy);
    this->set_tot_F_at_age(tot_F_at_age);
    this->set_perceived_tot_F_at_age(perceived_tot_F_at_age);
    //this->set_tot_M_at_age(tot_M_at_age);
	this->set_tot_W_at_age(tot_W_at_age);
    this->set_tot_Mat_at_age(tot_Mat_at_age);

    cout << "END compute_tot_N_and_F_and_W_at_age() "  << endl;

}




void Population::compute_tot_M_at_age()
{
    cout << "BEGIN compute_tot_M_at_age() for pop " << this->get_name()  << endl;

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
//if(this->get_name() ==2)    cout << "FOR sz  " << sz << " and age " << a  << endl;
//if(this->get_name() ==2)      cout << "a_tot_N_at_szgroup[sz]   " << a_tot_N_at_szgroup[sz]   << endl;
                  tot_N_at_age[a] +=  percent_szgroup_per_age_matrix[sz][a] * a_tot_N_at_szgroup[sz] ;
//if(this->get_name() ==2)      cout << "FOR2 sz  " << sz << " and age " << a  << endl;
//if(this->get_name() ==2)      cout << "a_tot_N_at_szgroup_before_applying_M[sz]   " << a_tot_N_at_szgroup_before_applying_M[sz]   << endl;
                  tot_N_at_age_before_M[a] +=  percent_szgroup_per_age_matrix[sz][a] * a_tot_N_at_szgroup_before_applying_M[sz] ;
        }
    }

//if(this->get_name() ==2)      cout << "comput the log " << endl;

     for(unsigned int a = 0; a < tot_M_at_age.size(); a++)
    {
//if(this->get_name() ==2)        cout << "FOR age " << a  << endl;

         if(tot_N_at_age_before_M.at(a) >0 && tot_N_at_age.at(a)>0)
        {
            tot_M_at_age.at(a)+= -log(tot_N_at_age.at(a)/tot_N_at_age_before_M.at(a));  // used for outcomes
        }
        else
        {
            tot_M_at_age.at(a)+= 0;
        }
//if(this->get_name() ==2)  cout << " tot_M_at_age.at(a) is " <<  tot_M_at_age.at(a)  << endl;
    }
//if(this->get_name() ==2)    cout << "set the new M at age " << endl;

   this->set_tot_M_at_age(tot_M_at_age);


     // don´t forget to update the perceived N with the N affected by the natural mortality
     this->set_perceived_tot_N_at_age(tot_N_at_age);
     this->set_tot_N_at_age(tot_N_at_age);

   dout(cout << "END compute_tot_M_at_age() "  << endl);
}



void Population::clear_tot_F_at_age()
{
	for(unsigned int a = 0; a < tot_F_at_age.size(); a++)
	{
		tot_F_at_age[a]=0;
		tot_F_at_age_last_quarter[a]=0;
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


double Population::compute_fbar()
{
    tout(cout<< "compute fbar for pop..." << this->get_name() << endl);

    dout(cout<< "compute fbar..." << endl);
	double fbar=0;
	int age_min =this->fbar_ages_min_max.at(0);
	int age_max =this->fbar_ages_min_max.at(1);
	if(age_max==0 || (age_max < age_min))
	{
        dout(cout << "age_max at 0 for this pop??" << endl);
		age_max=5;
	}

    vector <double> a_tot_F_at_age= this->get_perceived_tot_F_at_age(); // perceived

    for(int a = age_min; a < age_max; a++)
	{
								 // sum...
        fbar+=a_tot_F_at_age[a];
	}
								 // then do the average...
    tout(cout<< "sum fbar..." << fbar << endl);
    fbar=fbar/(fbar_ages_min_max.at(1)-fbar_ages_min_max.at(0));
    tout(cout<< "fbar..." << fbar << endl);
    tout(cout<< "compute fbar...ok" << endl);
    return(fbar);
}


vector<double> Population::compute_SSB()
{
    // first of all, get the N
    vector <double> a_tot_N_at_szgroup=this->get_tot_N_at_szgroup();

    vector <double> SSB_per_szgroup (a_tot_N_at_szgroup.size());
    cout << "compute SSB given pop is " << this->get_name()  << " " << endl;
    for(unsigned int i = 0; i < a_tot_N_at_szgroup.size(); i++)
    {

       // reminder: tot_N_at_szgroup are in thousand in input file
       //  but in absolute numbers here because have been multiplied by 1000 when importing
       SSB_per_szgroup.at(i) =  weight_at_szgroup.at(i) * a_tot_N_at_szgroup.at(i) * maturity_at_szgroup.at(i);
       //cout << "szgroup is " << i  << " " << endl );
       //cout << "tot_N_at_szgroup is " << a_tot_N_at_szgroup.at(i)  << " " << endl ;
       //cout << "maturity_at_szgroup is " << maturity_at_szgroup.at(i)  << " " << endl ;
       //cout << "weight_at_szgroup is " << weight_at_szgroup.at(i)  << " kg" << endl ;

    }

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



void Population::compute_TAC(int tstep, double multiOnTACconstraint, int HCR)
{
    dout(cout<< "------------" << endl);
    dout(cout<< "COMPUTE TAC for HCR (based on F) for this coming year for pop " << this->get_name() << endl);
	vector<double> fbar_ages_min_max =this-> get_fbar_ages_min_max();
	int age_min = fbar_ages_min_max.at(0);
    dout(cout<< "age min:" << age_min << endl);
	int age_max = fbar_ages_min_max.at(1);
    dout(cout<< "age_max:" <<age_max << endl);
	double ftarget = fbar_ages_min_max.at(2);
    dout(cout<< "ftarget:" << ftarget << endl);
	double Fpercent = fbar_ages_min_max.at(3);
    dout(cout<< "Fpercent:" << Fpercent << endl);
	double TACpercent = fbar_ages_min_max.at(4);
    dout(cout<< "TACpercent:" << TACpercent << endl);
    double Btrigger = fbar_ages_min_max.at(5);
    dout(cout<< "TACpercent:" << Btrigger << endl);
    double FMSY = fbar_ages_min_max.at(6);
    dout(cout<< "FMSY:" << FMSY << endl);
    //double FMSYdown = fbar_ages_min_max.at(7);
    double FMSYdown=FMSY*0.9; // TO BE REMOVED WHEN INPUT DATA
    dout(cout<< "FMSYdown:" << FMSY << endl);
    //double FMSYup = fbar_ages_min_max.at(8);
    double FMSYup=FMSY*1.1; // TO BE REMOVED WHEN INPUT DATA
    dout(cout<< "FMSYup:" << FMSY << endl);

    int nbages=this->get_nb_ages();
    int nbszgroups=this->get_nb_szgroups();
    vector <double> tot_N_at_age_end_previous_y(nbages);
    vector <double> tot_N_at_sz_end_previous_y(nbszgroups);
    vector <double> tot_W_at_age_y_plus_1(nbages);
    vector <double> maturity_at_age_y_plus_1(nbages);
    vector <double> tot_F_at_age_end_previous_y(nbages);
    vector <double> tot_M_at_age_y(nbages);

    // 1. compute previous fbar
								 // at the end of the last year, then for last year py...
    tout(cout<< "when computing TAC, first compute fbar for pop..." << this->get_name() << endl);
    double fbar_py=1.0;
    if(tstep==0) // THIS CODE BLOCK IS NOT USED IN NORMAL MODE, THIS IS JUST FOR DEBUGGING (Options::DEBUG) TO AVOID WAITING THE FULL Y TO LOOK AT TAC NUMBERS...
    {
        fbar_py=FMSY; // in case the tac is computed at start (for debugging) then use FMSY, because by nature perceived_F does not exist yet

        // but first apply size percent_szgroup_per_age_matrix to finds out N & co IN AGE!
        tot_N_at_sz_end_previous_y                 = this->get_tot_N_at_szgroup_month_minus_1();
        vector <double> a_weight_at_szgroup        =this->get_weight_at_szgroup();
        vector <double> a_maturity_at_szgroup      =this->get_maturity_at_szgroup();
        vector <vector <double> > percent_szgroup_per_age_matrix= this->get_percent_age_per_szgroup_matrix();
            for(int sz = 0; sz < nbszgroups; sz++)
            {
                for(int a = 0; a < nbages; a++)
                {
                    tot_F_at_age_end_previous_y[a] = fbar_py; // assumption
                    tot_M_at_age_y[a]              = 0.3; // assumption
                    tot_N_at_age_end_previous_y[a] +=  percent_szgroup_per_age_matrix[sz][a] * tot_N_at_sz_end_previous_y[sz] ; // assumption
                    tot_W_at_age_y_plus_1[a]       +=  percent_szgroup_per_age_matrix[sz][a] * a_weight_at_szgroup[sz] ; // assumption
                    maturity_at_age_y_plus_1[a]    +=  percent_szgroup_per_age_matrix[sz][a] * a_maturity_at_szgroup[sz] ; // assumption
                }

            }
        this->set_perceived_tot_F_at_age(tot_F_at_age_end_previous_y); // assumption
        this->set_tot_F_at_age(tot_F_at_age_end_previous_y); // assumption
        this->set_tot_M_at_age(tot_M_at_age_y); // assumption
        this->set_perceived_tot_N_at_age(tot_N_at_age_end_previous_y); // assumption
        this->set_tot_W_at_age(tot_W_at_age_y_plus_1); // assumption
        this->set_tot_Mat_at_age(maturity_at_age_y_plus_1);// assumption

    }
    else
    {   // by default...
        fbar_py= this->compute_fbar();
        tot_N_at_sz_end_previous_y  = this->get_tot_N_at_szgroup_month_minus_1(); // true N
        tot_N_at_age_end_previous_y = this->get_perceived_tot_N_at_age(); // perceived
        tot_W_at_age_y_plus_1       = this->get_tot_W_at_age();
        maturity_at_age_y_plus_1    = this->get_tot_Mat_at_age();
    }
    this->set_fbar(fbar_py);
    dout(cout << "the fbar at y-1 for this pop is " << fbar_py << endl);



    double tac_y_plus_1=0.0;
    double tac_y=0.0; // only used for YEAR 2
    if(fbar_py>0)
    {
       if(fbar_py>2)
       {
        fbar_py=2; // a safeguard to avoid non-sense computation of TAC change
        cout << "fbar_py set to 2 for pop " << this->get_name() << endl;
       }



       if(HCR==1){ // long-term management plan

       // 2. compare with the target
       double fmultiplier=1;
       if(fbar_py > ftarget)
       {
		fmultiplier = 1.0 - (Fpercent/100);
       }
       else
       {
		fmultiplier = 1.0 + (Fpercent/100);
       }
    cout << "HCR type 1: the fmultiplier for this pop " << this->get_name() <<" is then " << fmultiplier <<
		" given the target F " <<  ftarget << " in the plan..." << endl;

    // 3. perform a short-term forecast on N with F * fmultipier * fmultiplier

    //a. for year y from y-1
    dout(cout << "the  N by age at the end of  y is " << endl);
    tot_N_at_age_end_previous_y = this->get_perceived_tot_N_at_age(); // perceived

    for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
    {
        cout << "tot_N_at_age_end_previous_y age" << i << ": " << tot_N_at_age_end_previous_y.at(i) << endl;
    }


    dout(cout << "the forecast F by age for y (from y-1) is " << endl);
    vector <double> tot_F_at_age_end_previous_y = this->get_tot_F_at_age();
    vector <double> tot_F_at_age_y(tot_F_at_age_end_previous_y.size());
    for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
    {
                                 // target for y
        tot_F_at_age_y.at(i) = tot_F_at_age_end_previous_y.at(i)*fmultiplier;
    }
    for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
    {
        cout << "tot_F_at_age_y age" << i << ": " << tot_F_at_age_y.at(i) << endl;
    }


    dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
    vector <double> tot_M_at_age_y = this->get_tot_M_at_age();
    for (unsigned int i=0; i < tot_M_at_age_y.size(); i++)
    {
        cout << "tot_M_at_age_y age" << i << ": " << tot_M_at_age_y.at(i) << endl;
    }

    dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                 // init
    vector <double> forecast_tot_N_at_age_end_y(tot_N_at_age_end_previous_y.size());

    //  adding recruits before the F, assuming fixed recruits
    double recruits = tot_N_at_age_end_previous_y.at(0)*0.9; // precautionary pessimistic recruits
    forecast_tot_N_at_age_end_y.at(0)=recruits;

    dout(cout << "then, the forecast N by age for y (from y-1) is " << endl);
    for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
    {
        forecast_tot_N_at_age_end_y.at(i) =
            tot_N_at_age_end_previous_y.at(i-1)*
            exp( -((tot_F_at_age_y.at(i-1)) + tot_M_at_age_y.at(i-1)) );
        ;						 // class change and apply mortality F+M
    }							 // TO DO: add the plusgroup (if necessary)


    for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
    {
        cout << "forecast_tot_N_at_age_end_y at age" << i << ": " << forecast_tot_N_at_age_end_y.at(i) << endl;
    }

    // CAUTION in YEAR 2 compute the TAC y (in tons) BECAUSE THIS INTERMEDIATE YEAR IS MISSING!
    // according to the forecast N (Baronovs equation)
    // TAC per age and then sum over ages....
    if(tstep==8761)
    {
       cout << "FOR YEAR y i.e. year 2" << endl;
        double contribution_this_age=0.0;
       for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
       {
        if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

        contribution_this_age=  (tot_F_at_age_y.at(i)/((tot_F_at_age_y.at(i))+tot_M_at_age_y.at(i))) *
            forecast_tot_N_at_age_end_y.at(i)* tot_W_at_age_y_plus_1.at(i)*
            (1- (exp(-(tot_F_at_age_y.at(i)+tot_M_at_age_y.at(i)))));

       if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
       tac_y+= contribution_this_age;
       cout << "ADDING the contribution of age" << i << ", tac_y in kg is now   : " << tac_y << endl;
       }
       cout << "tac_y in kg is: " << tac_y << endl;
       tac_y= tac_y/1000;   // convert in tons;
    }


    //b. for year y+1 from y
    dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                 //init
    vector <double> forecast_tot_N_at_age_end_y_plus_1 (forecast_tot_N_at_age_end_y.size());
                                 //init
    vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
    for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
    {
                                 // target for y+1
        tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_y.at(i)*fmultiplier;
    }
    for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
    {
        cout << "tot_F_at_age_y_plus_1 age" << i << ": " << tot_F_at_age_y_plus_1.at(i) << endl;
    }

    dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
    vector <double> tot_M_at_age_y_plus_1 = this->get_tot_M_at_age();
    for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
    {
        cout << "tot_M_at_age_y_plus_1 age" << i << ": " << tot_M_at_age_y_plus_1.at(i) << endl;
    }

    dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
    tot_W_at_age_y_plus_1 = this->get_tot_W_at_age();
    for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
    {
        cout << "tot_W_at_age_y_plus_1 age" << i << ": " << tot_W_at_age_y_plus_1.at(i) << endl;
    }


    //  Adding recruits before the F, assuming fixed recruits and distribute recruits among szgroup
    recruits = forecast_tot_N_at_age_end_y.at(0)*0.9; // precautionary pessimistic recruits
    forecast_tot_N_at_age_end_y_plus_1.at(0) = recruits;

    dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
    for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
    {
        forecast_tot_N_at_age_end_y_plus_1.at(i) =
            forecast_tot_N_at_age_end_y.at(i-1)*
            exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
        ;						 // class change and apply mortality F+M
    }							 // TO DO: add the plusgroup (if necessary)


    for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
    {
        cout << "forecast_tot_N_at_age_end_y_plus_1 age" << i << ": " << forecast_tot_N_at_age_end_y_plus_1.at(i) << endl;
    }

    // 4. compute the TAC y+1 (in tons) according to the forecast N (Baronovs equation)
    // TAC per age and then sum over ages....
    cout << "compute TAC from contributions of age classes---" << endl;
    double contribution_this_age=0.0;
    for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
    {
        if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

       contribution_this_age=  (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
            forecast_tot_N_at_age_end_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
            (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))));

       if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
       tac_y_plus_1+= contribution_this_age;


       cout << "ADDING the contribution of age" << i << ", tac_y_plus_1 in kg is now   : " << tac_y_plus_1 << endl;
   }
    cout << "tac_y_plus_1 in kg is: " << tac_y_plus_1 << endl;
   tac_y_plus_1= tac_y_plus_1/1000;   // convert in tons;


	// 5. check if within the TAC range, otherwise force to keep the range.
								 // i.e. at y


    // apply here a scenario if any (default multi is 1)
    TACpercent=TACpercent*multiOnTACconstraint;

    double tac_y= this->get_tac()->get_current_tac();
	if((tac_y_plus_1/tac_y) > (1+(TACpercent/100)))
	{
		tac_y_plus_1=tac_y* (1+(TACpercent/100));
	}
	if((tac_y_plus_1/tac_y) < (1-(TACpercent/100)))
	{
		tac_y_plus_1=tac_y* (1-(TACpercent/100));
	}
	cout << "but actually the TAC for y+1 will be " << tac_y_plus_1 <<
		" to comply with the TAC limits..." << endl;
								 // the current tac is the avant-dernier



   } // end HCR type 1




   if(HCR==2){ //  F-MSY ICES approach (i.e. incl. a B-MSY trigger)

        // 1. check B trigger
        cout << "F-MSY approach applies with Btrigger at " << Btrigger << "  (when necessary)... " << endl;
        double biomass_computed_from_ages =0.0;
        double biomass_computed_from_szgroups =0.0;
        for (unsigned int a=0; a < tot_N_at_age_end_previous_y.size(); a++)
        {
            biomass_computed_from_ages+= tot_N_at_age_end_previous_y.at(a)* tot_W_at_age_y_plus_1.at(a) * maturity_at_age_y_plus_1.at(a); // SSB
        }
        for (unsigned int z=0; z < tot_N_at_sz_end_previous_y.size(); z++)
        {
            biomass_computed_from_szgroups+=  tot_N_at_sz_end_previous_y.at(z) * this->get_weight_at_szgroup().at(z) * this->get_maturity_at_szgroup().at(z); // SSB
        }


        biomass_computed_from_szgroups= biomass_computed_from_szgroups/1000; // SSB in tons
        cout << "current SSB pop"<< this->get_name() <<" in tons is " << biomass_computed_from_szgroups << "  for this stock...(when computed from szgroups (true)) " << endl;
        biomass_computed_from_ages= biomass_computed_from_ages/1000; // SSB in tons
        cout << "current SSB pop"<< this->get_name() <<" in tons is " << biomass_computed_from_ages << "  for this stock...(when computed from ages (perceived)) " << endl;



        double fmultiplier=1;
        // 2. compare with the target
            fmultiplier = 1.0 *FMSY /fbar_py;
          if(biomass_computed_from_ages<Btrigger)
          {
          fmultiplier = 1.0 *FMSY*(biomass_computed_from_ages/Btrigger) /fbar_py;
          cout << "so...Btrigger applies! " << endl;
          }

         cout << "HCR type 2: the fmultiplier for this pop " << this->get_name() <<" is then " << fmultiplier <<
            " given the target F " <<  FMSY << " in the plan..." << " while fbar_py is " << fbar_py << endl;

         // 3. perform a short-term forecast on N with F * fmultipier * fmultiplier

         //a. for year y from y-1
         dout(cout << "the  N by age at the end of  y is " << endl);
         tot_N_at_age_end_previous_y = this->get_perceived_tot_N_at_age(); // perceived
         for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
         {
             cout << "tot_N_at_age_end_previous_y age" << i << ": " << tot_N_at_age_end_previous_y.at(i) << endl;
         }

         vector <double> tot_F_at_age_end_previous_y = this->get_tot_F_at_age();
         for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
         {
             cout << "tot_F_at_age_end_previous_y age" << i << ": " << tot_F_at_age_end_previous_y.at(i) << endl;
         }

         dout(cout << "the forecast F by age for y (from y-1) is " << endl);
         vector <double> tot_F_at_age_y(tot_F_at_age_end_previous_y.size());
         for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
         {
                                      // target for y
             tot_F_at_age_y.at(i) = tot_F_at_age_end_previous_y.at(i)*fmultiplier;
         }
         for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
         {
             cout << "tot_F_at_age_y age" << i << ": " << tot_F_at_age_y.at(i) << endl;
         }


         dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
         vector <double> tot_M_at_age_y = this->get_tot_M_at_age();
         for (unsigned int i=0; i < tot_M_at_age_y.size(); i++)
         {
             cout << "tot_M_at_age_y age" << i << ": " << tot_M_at_age_y.at(i) << endl;
         }

         dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                      // init
         vector <double> forecast_tot_N_at_age_end_y(tot_N_at_age_end_previous_y.size());

         //  adding recruits before the F, assuming fixed recruits
         double recruits = tot_N_at_age_end_previous_y.at(0)*0.9; // precautionary pessimistic recruits
         forecast_tot_N_at_age_end_y.at(0)=recruits;

         dout(cout << "then, the forecast N by age for y (from y-1) is " << endl);
         for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             forecast_tot_N_at_age_end_y.at(i) =
                 tot_N_at_age_end_previous_y.at(i-1)*
                 exp( -((tot_F_at_age_y.at(i-1)) + tot_M_at_age_y.at(i-1)) );
             ;						 // class change and apply mortality F+M
         }							 // TO DO: add the plusgroup (if necessary)


         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             cout << "forecast_tot_N_at_age_end_y at age" << i << ": " << forecast_tot_N_at_age_end_y.at(i) << endl;
         }

         // CAUTION in YEAR 2 compute the TAC y (in tons) BECAUSE THIS INTERMEDIATE YEAR IS MISSING!
         // according to the forecast N (Baronovs equation)
         // TAC per age and then sum over ages....
         if(tstep==8761)
         {
            cout << "FOR YEAR y i.e. year 2" << endl;
             double contribution_this_age=0.0;
            for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
            {
             if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

             contribution_this_age=  (tot_F_at_age_y.at(i)/((tot_F_at_age_y.at(i))+tot_M_at_age_y.at(i))) *
                 forecast_tot_N_at_age_end_y.at(i)* tot_W_at_age_y_plus_1.at(i)*
                 (1- (exp(-(tot_F_at_age_y.at(i)+tot_M_at_age_y.at(i)))));

            if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
            tac_y+= contribution_this_age;
            cout << "ADDING the contribution of age" << i << ", tac_y in kg is now   : " << tac_y << endl;
            }
            cout << "tac_y in kg is: " << tac_y << endl;
            tac_y= tac_y/1000;   // convert in tons;
         }


         //b. for year y+1 from y
         dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                      //init
         vector <double> forecast_tot_N_at_age_end_y_plus_1 (forecast_tot_N_at_age_end_y.size());
                                      //init
         vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
         for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
         {
                                      // target for y+1
             tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_y.at(i)*fmultiplier;
         }
         for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_F_at_age_y_plus_1 age" << i << ": " << tot_F_at_age_y_plus_1.at(i) << endl;
         }

         dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
         vector <double> tot_M_at_age_y_plus_1 = this->get_tot_M_at_age();
         for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_M_at_age_y_plus_1 age" << i << ": " << tot_M_at_age_y_plus_1.at(i) << endl;
         }

         dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
         tot_W_at_age_y_plus_1 = this->get_tot_W_at_age();
         for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_W_at_age_y_plus_1 age" << i << ": " << tot_W_at_age_y_plus_1.at(i) << endl;
         }


         //  Adding recruits before the F, assuming fixed recruits and distribute recruits among szgroup
         recruits = forecast_tot_N_at_age_end_y.at(0)*0.9; // precautionary pessimistic recruits
         forecast_tot_N_at_age_end_y_plus_1.at(0) = recruits;

         dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
         for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             forecast_tot_N_at_age_end_y_plus_1.at(i) =
                 forecast_tot_N_at_age_end_y.at(i-1)*
                 exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
             ;						 // class change and apply mortality F+M
         }							 // TO DO: add the plusgroup (if necessary)


         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
         {
             cout << "forecast_tot_N_at_age_end_y_plus_1 age" << i << ": " << forecast_tot_N_at_age_end_y_plus_1.at(i) << endl;
         }

         // 4. compute the TAC y+1 (in tons) according to the forecast N (Baronovs equation)
         // TAC per age and then sum over ages....
         cout << "compute TAC from contributions of age classes---" << endl;
         double contribution_this_age=0.0;
         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
         {
             if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

            contribution_this_age=  (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
                 forecast_tot_N_at_age_end_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
                 (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))));

            if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
            tac_y_plus_1+= contribution_this_age;


            cout << "ADDING the contribution of age" << i << ", tac_y_plus_1 in kg is now   : " << tac_y_plus_1 << endl;
        }
         cout << "tac_y_plus_1 in kg is: " << tac_y_plus_1 << endl;
        tac_y_plus_1= tac_y_plus_1/1000;   // convert in tons;

   } // end HCR type 2


   if(HCR==3){ //  F-MSY ICES approach (i.e. incl. a B-MSY trigger) but with flexibility of FMSY range

        // 1. check B trigger
        cout << "F-MSY approach applies with Btrigger at " << Btrigger << "  (when necessary)... " << endl;
        cout << "F-MSY approach applies with Btrigger at " << Btrigger << "  (when necessary)... " << endl;
        double biomass_computed_from_ages =0.0;
        double biomass_computed_from_szgroups =0.0;
        for (unsigned int a=0; a < tot_N_at_age_end_previous_y.size(); a++)
        {
            biomass_computed_from_ages+= tot_N_at_age_end_previous_y.at(a)* tot_W_at_age_y_plus_1.at(a) * maturity_at_age_y_plus_1.at(a); // SSB
        }
        for (unsigned int z=0; z < tot_N_at_age_end_previous_y.size(); z++)
        {
            biomass_computed_from_szgroups+=  tot_N_at_sz_end_previous_y.at(z) * this->get_weight_at_szgroup().at(z) * this->get_maturity_at_szgroup().at(z); // SSB
        }


        biomass_computed_from_szgroups= biomass_computed_from_szgroups/1000; // SSB in tons
        cout << "current SSB in tons is " << biomass_computed_from_szgroups << "  for this stock...(when computed from szgroups (true)) " << endl;
        biomass_computed_from_ages= biomass_computed_from_ages/1000; // SSB in tons
        cout << "current SSB in tons is " << biomass_computed_from_ages << "  for this stock...(when computed from ages (perceived)) " << endl;

        double fmultiplier=1;
        // 2. compare with the target
            fmultiplier = 1.0 *FMSY /fbar_py;
            if(this->get_is_choking_fisheries()) fmultiplier = 1.0 *FMSYup /fbar_py;
            if(!this->get_is_choking_fisheries()) fmultiplier = 1.0 *FMSYdown /fbar_py;
            fmultiplier = 1.0 *FMSY /fbar_py;
          if(biomass_computed_from_ages<Btrigger)
          {
          fmultiplier = 1.0 *FMSY*(biomass_computed_from_ages/Btrigger) /fbar_py;
          cout << "so...Btrigger applies! " << endl;
          }

         cout << "HCR type 3: the fmultiplier for this pop " << this->get_name() <<" is then " << fmultiplier <<
            " given the target F " <<  FMSY << " in the plan..." << endl;


         // 3. perform a short-term forecast on N with F * fmultipier * fmultiplier

         //a. for year y from y-1
         dout(cout << "the  N by age at the end of  y is " << endl);
         tot_N_at_age_end_previous_y = this->get_perceived_tot_N_at_age(); // perceived

         for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
         {
             cout << "tot_N_at_age_end_previous_y age" << i << ": " << tot_N_at_age_end_previous_y.at(i) << endl;
         }


         dout(cout << "the forecast F by age for y (from y-1) is " << endl);
         vector <double> tot_F_at_age_end_previous_y = this->get_tot_F_at_age();
         vector <double> tot_F_at_age_y(tot_F_at_age_end_previous_y.size());
         for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
         {
                                      // target for y
             tot_F_at_age_y.at(i) = tot_F_at_age_end_previous_y.at(i)*fmultiplier;
         }
         for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
         {
             cout << "tot_F_at_age_y age" << i << ": " << tot_F_at_age_y.at(i) << endl;
         }


         dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
         vector <double> tot_M_at_age_y = this->get_tot_M_at_age();
         for (unsigned int i=0; i < tot_M_at_age_y.size(); i++)
         {
             cout << "tot_M_at_age_y age" << i << ": " << tot_M_at_age_y.at(i) << endl;
         }

         dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                      // init
         vector <double> forecast_tot_N_at_age_end_y(tot_N_at_age_end_previous_y.size());

         //  adding recruits before the F, assuming fixed recruits
         double recruits = tot_N_at_age_end_previous_y.at(0)*0.9; // precautionary pessimistic recruits
         forecast_tot_N_at_age_end_y.at(0)=recruits;

         dout(cout << "then, the forecast N by age for y (from y-1) is " << endl);
         for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             forecast_tot_N_at_age_end_y.at(i) =
                 tot_N_at_age_end_previous_y.at(i-1)*
                 exp( -((tot_F_at_age_y.at(i-1)) + tot_M_at_age_y.at(i-1)) );
             ;						 // class change and apply mortality F+M
         }							 // TO DO: add the plusgroup (if necessary)


         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             cout << "forecast_tot_N_at_age_end_y at age" << i << ": " << forecast_tot_N_at_age_end_y.at(i) << endl;
         }

         // CAUTION in YEAR 2 compute the TAC y (in tons) BECAUSE THIS INTERMEDIATE YEAR IS MISSING!
         // according to the forecast N (Baronovs equation)
         // TAC per age and then sum over ages....
         if(tstep==8761)
         {
            cout << "FOR YEAR y i.e. year 2" << endl;
             double contribution_this_age=0.0;
            for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
            {
             if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

             contribution_this_age=  (tot_F_at_age_y.at(i)/((tot_F_at_age_y.at(i))+tot_M_at_age_y.at(i))) *
                 forecast_tot_N_at_age_end_y.at(i)* tot_W_at_age_y_plus_1.at(i)*
                 (1- (exp(-(tot_F_at_age_y.at(i)+tot_M_at_age_y.at(i)))));

            if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
            tac_y+= contribution_this_age;
            cout << "ADDING the contribution of age" << i << ", tac_y in kg is now   : " << tac_y << endl;
            }
            cout << "tac_y in kg is: " << tac_y << endl;
            tac_y= tac_y/1000;   // convert in tons;
         }


         //b. for year y+1 from y
         dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                      //init
         vector <double> forecast_tot_N_at_age_end_y_plus_1 (forecast_tot_N_at_age_end_y.size());
                                      //init
         vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
         for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
         {
                                      // target for y+1
             tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_y.at(i)*fmultiplier;
         }
         for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_F_at_age_y_plus_1 age" << i << ": " << tot_F_at_age_y_plus_1.at(i) << endl;
         }

         dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
         vector <double> tot_M_at_age_y_plus_1 = this->get_tot_M_at_age();
         for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_M_at_age_y_plus_1 age" << i << ": " << tot_M_at_age_y_plus_1.at(i) << endl;
         }

         dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
         tot_W_at_age_y_plus_1 = this->get_tot_W_at_age();
         for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_W_at_age_y_plus_1 age" << i << ": " << tot_W_at_age_y_plus_1.at(i) << endl;
         }


         //  Adding recruits before the F, assuming fixed recruits and distribute recruits among szgroup
         recruits = forecast_tot_N_at_age_end_y.at(0)*0.9; // precautionary pessimistic recruits
         forecast_tot_N_at_age_end_y_plus_1.at(0) = recruits;

         dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
         for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             forecast_tot_N_at_age_end_y_plus_1.at(i) =
                 forecast_tot_N_at_age_end_y.at(i-1)*
                 exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
             ;						 // class change and apply mortality F+M
         }							 // TO DO: add the plusgroup (if necessary)


         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
         {
             cout << "forecast_tot_N_at_age_end_y_plus_1 age" << i << ": " << forecast_tot_N_at_age_end_y_plus_1.at(i) << endl;
         }

         // 4. compute the TAC y+1 (in tons) according to the forecast N (Baronovs equation)
         // TAC per age and then sum over ages....
         cout << "compute TAC from contributions of age classes---" << endl;
         double contribution_this_age=0.0;
         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
         {
             if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

            contribution_this_age=  (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
                 forecast_tot_N_at_age_end_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
                 (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))));

            if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
            tac_y_plus_1+= contribution_this_age;


            cout << "ADDING the contribution of age" << i << ", tac_y_plus_1 in kg is now   : " << tac_y_plus_1 << endl;
        }
         cout << "tac_y_plus_1 in kg is: " << tac_y_plus_1 << endl;
        tac_y_plus_1= tac_y_plus_1/1000;   // convert in tons;


   } // end HCR type 3


   if(HCR==4){ //  statuquo TAC

             cout << "Statuquo TAC definition for next year... " << endl;
             double fmultiplier=1.0;
             cout << "HCR type 4: the fmultiplier for this pop " << this->get_name() <<" is then " << fmultiplier << endl;

             vector <double> tot_N_at_age_end_previous_y = this->get_perceived_tot_N_at_age(); // perceived
             vector <double> tot_W_at_age_y_plus_1 = this->get_tot_W_at_age();


             // 3. perform a short-term forecast on N with F * fmultipier * fmultiplier

             //a. for year y from y-1
             dout(cout << "the  N by age at the end of  y is " << endl);
             tot_N_at_age_end_previous_y = this->get_perceived_tot_N_at_age(); // perceived

             for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
             {
                 cout << "tot_N_at_age_end_previous_y age" << i << ": " << tot_N_at_age_end_previous_y.at(i) << endl;
             }


             dout(cout << "the forecast F by age for y (from y-1) is " << endl);
             vector <double> tot_F_at_age_end_previous_y = this->get_tot_F_at_age();
             vector <double> tot_F_at_age_y(tot_F_at_age_end_previous_y.size());
             for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
             {
                                          // target for y
                 tot_F_at_age_y.at(i) = tot_F_at_age_end_previous_y.at(i)*fmultiplier;
             }
             for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
             {
                 cout << "tot_F_at_age_y age" << i << ": " << tot_F_at_age_y.at(i) << endl;
             }


             dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
             vector <double> tot_M_at_age_y = this->get_tot_M_at_age();
             for (unsigned int i=0; i < tot_M_at_age_y.size(); i++)
             {
                 cout << "tot_M_at_age_y age" << i << ": " << tot_M_at_age_y.at(i) << endl;
             }

             dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                          // init
             vector <double> forecast_tot_N_at_age_end_y(tot_N_at_age_end_previous_y.size());

             //  adding recruits before the F, assuming fixed recruits
             double recruits = tot_N_at_age_end_previous_y.at(0)*0.9; // precautionary pessimistic recruits
             forecast_tot_N_at_age_end_y.at(0)=recruits;

             dout(cout << "then, the forecast N by age for y (from y-1) is " << endl);
             for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
             {
                 forecast_tot_N_at_age_end_y.at(i) =
                     tot_N_at_age_end_previous_y.at(i-1)*
                     exp( -((tot_F_at_age_y.at(i-1)) + tot_M_at_age_y.at(i-1)) );
                 ;						 // class change and apply mortality F+M
             }							 // TO DO: add the plusgroup (if necessary)


             for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
             {
                 cout << "forecast_tot_N_at_age_end_y at age" << i << ": " << forecast_tot_N_at_age_end_y.at(i) << endl;
             }

             // CAUTION in YEAR 2 compute the TAC y (in tons) BECAUSE THIS INTERMEDIATE YEAR IS MISSING!
             // according to the forecast N (Baronovs equation)
             // TAC per age and then sum over ages....
             if(tstep==8761)
             {
                cout << "FOR YEAR y i.e. year 2" << endl;
                 double contribution_this_age=0.0;
                for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
                {
                 if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

                 contribution_this_age=  (tot_F_at_age_y.at(i)/((tot_F_at_age_y.at(i))+tot_M_at_age_y.at(i))) *
                     forecast_tot_N_at_age_end_y.at(i)* tot_W_at_age_y_plus_1.at(i)*
                     (1- (exp(-(tot_F_at_age_y.at(i)+tot_M_at_age_y.at(i)))));

                if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
                tac_y+= contribution_this_age;
                cout << "ADDING the contribution of age" << i << ", tac_y in kg is now   : " << tac_y << endl;
                }
                cout << "tac_y in kg is: " << tac_y << endl;
                tac_y= tac_y/1000;   // convert in tons;
             }


             //b. for year y+1 from y
             dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                          //init
             vector <double> forecast_tot_N_at_age_end_y_plus_1 (forecast_tot_N_at_age_end_y.size());
                                          //init
             vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
             for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
             {
                                          // target for y+1
                 tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_y.at(i)*fmultiplier;
             }
             for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
             {
                 cout << "tot_F_at_age_y_plus_1 age" << i << ": " << tot_F_at_age_y_plus_1.at(i) << endl;
             }

             dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
             vector <double> tot_M_at_age_y_plus_1 = this->get_tot_M_at_age();
             for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
             {
                 cout << "tot_M_at_age_y_plus_1 age" << i << ": " << tot_M_at_age_y_plus_1.at(i) << endl;
             }

             dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
             tot_W_at_age_y_plus_1 = this->get_tot_W_at_age();
             for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
             {
                 cout << "tot_W_at_age_y_plus_1 age" << i << ": " << tot_W_at_age_y_plus_1.at(i) << endl;
             }


             //  Adding recruits before the F, assuming fixed recruits and distribute recruits among szgroup
             recruits = forecast_tot_N_at_age_end_y.at(0)*0.9; // precautionary pessimistic recruits
             forecast_tot_N_at_age_end_y_plus_1.at(0) = recruits;

             dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
             for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
             {
                 forecast_tot_N_at_age_end_y_plus_1.at(i) =
                     forecast_tot_N_at_age_end_y.at(i-1)*
                     exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
                 ;						 // class change and apply mortality F+M
             }							 // TO DO: add the plusgroup (if necessary)


             for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
             {
                 cout << "forecast_tot_N_at_age_end_y_plus_1 age" << i << ": " << forecast_tot_N_at_age_end_y_plus_1.at(i) << endl;
             }

             // 4. compute the TAC y+1 (in tons) according to the forecast N (Baronovs equation)
             // TAC per age and then sum over ages....
             cout << "compute TAC from contributions of age classes---" << endl;
             double contribution_this_age=0.0;
             for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
             {
                 if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

                contribution_this_age=  (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
                     forecast_tot_N_at_age_end_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
                     (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))));

                if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
                tac_y_plus_1+= contribution_this_age;


                cout << "ADDING the contribution of age" << i << ", tac_y_plus_1 in kg is now   : " << tac_y_plus_1 << endl;
            }
             cout << "tac_y_plus_1 in kg is: " << tac_y_plus_1 << endl;
            tac_y_plus_1= tac_y_plus_1/1000;   // convert in tons;


       } // end HCR type 4




     } // end fbar_py is 0


    if(tac_y==0 || !isfinite(tac_y))
    {
        cout << "WARNING: TACs Option is active but TAC y for this species found to go to 0.... Consider informing a initial TAC value for this pop..." << endl;
        cout << "a fake, non binding value is filled in for now" << endl;

        tac_y =100000; // in tons, and not limiting the fisheries
    }

     if(tac_y_plus_1==0 || !isfinite(tac_y_plus_1))
     {
         cout << "WARNING: TACs Option is active but TAC y+1 for this species found to go to 0.... Consider informing a initial TAC value for this pop..." << endl;
         cout << "a fake, non binding value is filled in for now" << endl;

         tac_y_plus_1 =100000; // in tons, and not limiting the fisheries
     }



    if(tstep==8761) {
        this->get_tac()->add_tac_to_ts(tac_y, 1);
        this->get_tac()->add_tac_to_ts(tac_y_plus_1, -1);
        cout << "so, the TAC (in tons) for y will be " << tac_y << endl;
        cout << "so, the TAC (in tons) for y+1 will be " << tac_y_plus_1 << endl;
    } else{
        cout << "so, the TAC (in tons) for y+1 will be " << tac_y_plus_1 << endl;
        this->get_tac()->add_tac_to_ts(tac_y_plus_1, -1);
    }

    this->set_is_choking_fisheries(0); // reinit
    this->get_tac()->set_is_tac_exhausted(0); // reinit
}


void Population::apply_natural_mortality()
{
    dout(cout << "BEGIN apply_natural_mortality() "  << endl);

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

    dout(cout << "END apply_natural_mortality() "  << endl);

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
	popdyn_N << " " <<  endl;
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
	popdyn_F << " " <<  endl;
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
    popdyn_SSB << " " <<  endl;
}



void Population::export_popdyn_annual_indic(ofstream& popdyn_annual_indic, int tstep, const DynAllocOptions &dyn_alloc_sce)
{

    tout(cout << "begin export_popdyn_annual_indic..."<< endl);

    popdyn_annual_indic << setprecision(4) << fixed;
	// tstep / pop / F at szgroup / tot landings on pop i.e. including oth landings as well
	popdyn_annual_indic << tstep << " " << this->get_name() << " ";
	double oth_mult   =this->get_oth_land_multiplier();
	double cpue_mult  =this->get_cpue_multiplier();

								 // output the annual multiplier
	popdyn_annual_indic  << oth_mult << " " << cpue_mult << " ";

    tout(cout<< "when exporting, get fbar for pop..." << this->get_name() << endl);
    double fbar_py= this->get_fbar();
	popdyn_annual_indic  << fbar_py << " ";

								 //...also including the oth land
	double so_far    =this->get_landings_so_far();
	popdyn_annual_indic  << so_far << " ";

    tout(cout << "retrieve tacs if any..."<< endl);

     double last_year_tac=0.0;
     if(dyn_alloc_sce.option(Options::TACs))
     {
         vector<double> tacs = this->get_tac()->get_ts_tac();
         last_year_tac=tacs.at(tacs.size()-2); // because compute_TAC() has just been called before this export
     }
     popdyn_annual_indic  << last_year_tac << " ";

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

	popdyn_annual_indic << " " <<  endl;

    tout(cout << "export_popdyn_annual_indic...ok"<< endl);

}
