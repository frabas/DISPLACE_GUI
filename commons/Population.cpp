// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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
                       const multimap<int, int> &lst_idx_nodes_per_pop,
                       const multimap<int, double> &_full_spatial_availability,
                       const map<int, double> &_oth_land,
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

    dout(cout << "create pop " << name << endl);

	full_spatial_availability= _full_spatial_availability;

	oth_land= _oth_land;

	growth_transition_matrix=_growth_transition_matrix;
	percent_szgroup_per_age_matrix=_percent_szgroup_per_age_matrix;
	percent_age_per_szgroup_matrix=_percent_age_per_szgroup_matrix;

	param_sr=_param_sr;

	landings_so_far= 0.0;

	// init...
	for(unsigned int sz=0; sz<init_tot_N_at_szgroup.size(); sz++)
	{
		tot_N_at_szgroup.push_back(0);
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
	for(unsigned int a=0; a<percent_szgroup_per_age_matrix[0].size(); a++)
	{
		tot_F_at_age.push_back(0);
	}
	for(unsigned int a=0; a<percent_szgroup_per_age_matrix[0].size(); a++)
	{
		tot_F_at_age_last_quarter.push_back(0);
	}

	// for catch equation
								 // for the catch equation
	this->set_selected_szgroups(_selected_szgroups);

	// ...then fill in with avai0_beta
								 // set the pop-specific beta from glm for szgroup0
	this->set_avai0_beta(_avai0_beta);

	// ...then fill in with avai0_beta
								 // set the pop-specific beta from glm for szgroup2
	this->set_avai2_beta(_avai2_beta);

	// ...then fill in with avai0_beta
								 // set the pop-specific beta from glm for szgroup3
	this->set_avai3_beta(_avai3_beta);

	// ...then fill in with avai0_beta
								 // set the pop-specific beta from glm for szgroup5
	this->set_avai5_beta(_avai5_beta);

	// ...then fill in with avai0_beta
								 // set the pop-specific beta from glm for szgroup7
	this->set_avai7_beta(_avai7_beta);

    // CALIB: if increased then improve the catch rate of the simulated vessels....
    // look at the config.dat for calibration values

								 // set to 1 the first year
	this->set_cpue_multiplier(1*a_calib_cpue_multiplier);

	// ...then fill in with start pop
                                 // set the overall N_at_szgroup
	this->set_tot_N_at_szgroup(init_tot_N_at_szgroup);

    // ...then fill in with start pop
                                 // set the overall N_at_szgroup
    this->set_true_tot_N_at_szgroup(init_tot_N_at_szgroup);

    // ...then fill in with start info
                                 // set the migrant prop
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

    for(unsigned int i=0; i < init_weight_at_szgroup.size(); i++)
	{
		init_weight_at_szgroup.at(i)=init_weight_at_szgroup.at(i)*a_calib_weight_at_szgroup;
	}

	this->set_weight_at_szgroup(init_weight_at_szgroup);

	// ...then fill in with start pop
	this->set_comcat_at_szgroup(init_comcat_at_szgroup);

	// ...then fill in with start pop
	this->set_maturity_at_szgroup(init_maturity_at_szgroup);

	// ...then fill in with start pop
	this->set_fecundity_at_szgroup(init_fecundity_at_szgroup);

	// ...then fill in with start pop
    for(unsigned int i=0; i < init_M_at_szgroup.size(); i++)
	{
		init_M_at_szgroup.at(i)=init_M_at_szgroup.at(i);
	}

	this->set_M_at_szgroup(init_M_at_szgroup);

	// ...then fill in with start pop
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
	vector<Node* > p_spe_nodes;
	for(multimap<int, double>::iterator iter=full_spatial_availability.begin(); iter != full_spatial_availability.end();
		iter = full_spatial_availability.upper_bound( iter->first ) )
	{
		p_spe_nodes.push_back (nodes[  iter->first  ]);
		nodes[ iter->first ]->set_pop_names_on_node(a_name);;

	}
    for(unsigned int i=0; i<p_spe_nodes.size(); i++)
		list_nodes.push_back(p_spe_nodes[i]);

	// add these Ns to the multimap of the concerned nodes
    dout(cout << " lst nodes: " << endl);
	for(unsigned int i=0; i<list_nodes.size(); i++)
	{
								 // caution: here is tot N on the node! need to call distribute_N()
		list_nodes.at(i)->set_Ns_pops_at_szgroup(a_name, tot_N_at_szgroup);
        dout(cout  << list_nodes.at(i)->get_idx_node() << " ");
	}

    dout(cout << endl);

	// distribute tot_N_at_szgroup on nodes knowing the avai spatial key
	// i.e. update the multimap Ns_pops_at_szgroup of the nodes
	this->distribute_N();		 // divide on nodes according to avai

	// init the N on node at month start
	for(unsigned int i=0; i<list_nodes.size(); i++)
	{
		vector<double> N_at_szgroup=list_nodes.at(i)->get_Ns_pops_at_szgroup(a_name);
		list_nodes.at(i)->set_Ns_pops_at_szgroup_at_month_start(name, N_at_szgroup);

	}

	// init tac
	tac = new Tac(init_tac[0], tac_percent_simulated, relative_stability_key);
	oth_land_multiplier=1.0;

	// init related to F
	fbar_ages_min_max=_fbar_ages_min_max;

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


double Population::get_landings_so_far() const
{
	return(landings_so_far);
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


multimap<int,double>  Population::get_full_spatial_availability() const
{
	return(full_spatial_availability);
}

multimap<int,double>  Population::get_overall_migration_fluxes() const
{
    return(overall_migration_fluxes);
}



map<int,double>  Population::get_oth_land() const
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


void Population::set_landings_so_far(double _landings_so_far)
{
	landings_so_far=_landings_so_far;
}


void Population::set_param_sr(const vector<double>& _param_sr)
{

	for(unsigned int i =0; i<_param_sr.size(); i++)
	{
		param_sr[i]=_param_sr[i];
	}
}


void Population::set_full_spatial_availability(multimap<int,double> _full_spatial_availability)
{
	full_spatial_availability= _full_spatial_availability;
}


void Population::set_overall_migration_fluxes(multimap<int,double> _overall_migration_fluxes)
{
    overall_migration_fluxes= _overall_migration_fluxes;
}


void Population::set_oth_land(map<int,double> _oth_land)
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

	// save for later use i.e. in the erosion of the avai after each catch event
	this->set_tot_N_at_szgroup_just_after_redistribution(tot_N_at_szgroup);

	// update the nodes
	//  N_at_szgroup on node x = N_at_szgroup for pop1 * avai on node x...
    for (unsigned int idx =0; idx < list_nodes.size(); idx++)
	{

		// get a node
		int idx_node = list_nodes[idx]->get_idx_node();

		// get avai for this node
		vector<double> avai_this_node;
		multimap<int,double>::iterator lower = full_spatial_availability.lower_bound(idx_node);
		multimap<int,double>::iterator upper = full_spatial_availability.upper_bound(idx_node);
		for (multimap<int, double>::iterator pos=lower; pos != upper; pos++)
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

        dout(cout << "N at szgroup on the node "<< idx_node<< ":" << endl);
		//for(int i=0; i<N_at_szgroup.size(); i++)
		//   dout << N_at_szgroup[i] << " ";
        dout(cout << endl);

		// set the new Ns for this specific pop in the multimap of this node
		list_nodes[idx]->set_Ns_pops_at_szgroup(name, N_at_szgroup);

	}
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

       dout(cout << "no migration for this pop"  << endl );

       cout << "no migration for this pop"  << endl;

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
    //SSB= SSB/1000/2;			 // convert in tons for the SSB-R // divide by 2 when assuming sex ratio 50:50
    SSB= SSB/1000;			 //
    cout << "SSB is " << SSB  << " tons" << endl ;
    //dout(cout << "SSB is " << SSB  << " tons" << endl );

	// compute R from SSB-R relationship
	// (caution: age dependent, e.g. SSB-R for cod 2532 is usually simulated for age2)...
	double recruits =(param_sr[0]*SSB*exp(-param_sr[1]*SSB)) * 1000;
    dout(cout << "New recruits are " << recruits  << endl );

    // add stochasticity on recruits (MAGIC NUMBER default: lognormal with CV at 20%)
	// TO DO: use a stock-specific input there...
    double sd=0.2;
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
    double sd=0.2;
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


void Population::compute_tot_N_and_F_and_M_and_W_at_age()
{
    dout(cout << "BEGIN compute_tot_N_and_F_and_M_and_W_at_age() "  << endl );

	vector <double> tot_F_at_age = get_tot_F_at_age();
    vector <double> perceived_tot_F_at_age = get_tot_F_at_age();
                                 // init
	vector <double> tot_M_at_age (tot_F_at_age.size());
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

	// apply a weighted mean for weight-at-age and mortality-at-age
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
			tot_M_at_age[a] +=  percent_age_per_szgroup_matrix[sz][a] * M_at_szgroup[sz] ;
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
	for(unsigned int a = 0; a < tot_F_at_age.size(); a++)
	{
        if(tot_N_at_age_minus_1.at(a) >0 && tot_N_at_age.at(a)>0)
		{
            perceived_tot_F_at_age.at(a)+= -log(perceived_tot_N_at_age.at(a)/perceived_tot_N_at_age_minus_1.at(a)); // used for perceived stock in the management procedure
            tot_F_at_age.at(a)+= -log(tot_N_at_age.at(a)/tot_N_at_age_minus_1.at(a));  // used for outcomes
        }
		else
		{
            perceived_tot_F_at_age.at(a)+= 0;
            tot_F_at_age.at(a)+= 0;
        }
		// => cumul over months
        dout(cout << "tot_N_at_age_minus_1[a]  is "<< tot_N_at_age_minus_1[a]  << endl);
        dout(cout << "tot_N_at_age[a]  is "<< tot_N_at_age[a]  << endl);
        dout(cout << "tot_F_at_age[a]  is "<< tot_F_at_age[a]  << endl);
        dout(cout << "tot_M_at_age[a]  is "<< tot_M_at_age[a]  << endl);

	}

	// set the tot N and F at age
	this->set_tot_N_at_age(tot_N_at_age);
    this->set_perceived_tot_N_at_age(perceived_tot_N_at_age);
    this->set_tot_F_at_age(tot_F_at_age);
    this->set_perceived_tot_F_at_age(perceived_tot_F_at_age);
    this->set_tot_M_at_age(tot_M_at_age);
	this->set_tot_W_at_age(tot_W_at_age);
    this->set_tot_Mat_at_age(tot_Mat_at_age);

    dout(cout << "END compute_tot_N_and_F_and_M_and_W_at_age() "  << endl);

}


void Population::clear_tot_F_at_age()
{
	for(unsigned int a = 0; a < tot_F_at_age.size(); a++)
	{
		tot_F_at_age[a]=0;
		tot_F_at_age_last_quarter[a]=0;
	}
}


double Population::compute_fbar()
{
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
	fbar=fbar/(fbar_ages_min_max.at(1)-fbar_ages_min_max.at(0));
	return(fbar);
}


vector<double> Population::compute_SSB()
{
    // first of all, get the N
    vector <double> a_tot_N_at_szgroup=this->get_tot_N_at_szgroup();

    vector <double> SSB_per_szgroup (a_tot_N_at_szgroup.size());
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

  return(SSB_per_szgroup);
}


void Population::compute_TAC(double multiOnTACconstraint, int HCR)
{
    dout(cout<< "COMPUTE TAC for HCR (based on F) for this coming year" << endl);
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
    // 1. compute previous fbar
								 // at the end of the last year, then for last year py...
	double fbar_py= this->compute_fbar();
    dout(cout << "the fbar at y-1 for this pop is " << fbar_py << endl);


    double tac_y_plus_1=0;

    if(HCR==1){ // long term management plan

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
	cout << "the fmultiplier for this pop " << this->get_name() <<" is then " << fmultiplier <<
		" given the target F " <<  ftarget << " in the plan..." << endl;

	// 3. perform a short-term forecast on N with F * fmultipier * fmultiplier

	//a. for year y from y-1
    dout(cout << "the  N by age at the end of  y is " << endl);
    vector <double> tot_N_at_age_end_previous_y = this->get_perceived_tot_N_at_age(); // perceived

    for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
	{
        dout(cout << "age" << i+1 << ": " << tot_N_at_age_end_previous_y.at(i) << endl);
	}

    dout(cout << "the forecast N by age for y (from y-1) is " << endl);
								 // init
	vector <double> forecast_tot_N_at_age_start_y(tot_N_at_age_end_previous_y.size());
    for (unsigned int i=1; i < tot_N_at_age_end_previous_y.size(); i++)
	{
								 // class change from 31th of Dec to 1st of Jan
		forecast_tot_N_at_age_start_y.at(i) = tot_N_at_age_end_previous_y.at(i-1);
	}
								 // note: forecast_tot_N_at_age_y.at(0) keeps the same=> assuming fixed recruits...
	forecast_tot_N_at_age_start_y.at(0) = tot_N_at_age_end_previous_y.at(0);
    for (unsigned int i=0; i < forecast_tot_N_at_age_start_y.size(); i++)
	{
        dout(cout << "age" << i+1 << ": " << forecast_tot_N_at_age_start_y.at(i) << endl);
	}

	//b. for year y+1 from y
    dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
								 //init
	vector <double> forecast_tot_N_at_age_start_y_plus_1 (forecast_tot_N_at_age_start_y.size());
	vector <double> tot_F_at_age_end_previous_y = this->get_tot_F_at_age();
								 //init
	vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
    for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
	{
								 // target for y+1
		tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_end_previous_y.at(i)*fmultiplier*fmultiplier;
	}
    for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
	{
        dout(cout << "age" << i+1 << ": " << tot_F_at_age_y_plus_1.at(i) << endl);
	}

    dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
	vector <double> tot_M_at_age_y_plus_1 = this->get_tot_M_at_age();
    for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
	{
        dout(cout << "age" << i+1 << ": " << tot_M_at_age_y_plus_1.at(i) << endl);
	}

    dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
	vector <double> tot_W_at_age_y_plus_1 = this->get_tot_W_at_age();
    for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
	{
        dout(cout << "age" << i+1 << ": " << tot_W_at_age_y_plus_1.at(i) << endl);
	}

    dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
    for (unsigned int i=1; i < forecast_tot_N_at_age_start_y.size(); i++)
	{
		forecast_tot_N_at_age_start_y_plus_1.at(i) =
			forecast_tot_N_at_age_start_y.at(i-1)*
			exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
		;						 // class change and apply mortality F+M
	}							 // TO DO: add the plusgroup (if necessary)
	forecast_tot_N_at_age_start_y_plus_1.at(0) = forecast_tot_N_at_age_start_y.at(0);
	// note: forecast_tot_N_at_age_y_plus_1.at(0) keeps the same=> assuming fixed recruits...

    for (unsigned int i=0; i < forecast_tot_N_at_age_start_y_plus_1.size(); i++)
	{
        dout(cout << "age" << i+1 << ": " << forecast_tot_N_at_age_start_y_plus_1.at(i) << endl);
	}

	// 4. compute the TAC (in tons) according to the forecast N (Baronovs equation)
	// TAC per age and then sum over ages....
    for (unsigned int i=1; i < forecast_tot_N_at_age_start_y_plus_1.size(); i++)
	{
		tac_y_plus_1+=   (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
			forecast_tot_N_at_age_start_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
								 // convert in tons;
			(1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))))/ 1000;

	}
    dout(cout << "so, the TAC (in tons) for y+1 will be " << tac_y_plus_1 << endl);

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


   } else{  // F-MSY ICES approach (i.e. incl. a B-MSY trigger)

        // 1. check B trigger
        dout(cout << "Biomass computed from N by age at the end of  y is " << endl);
        double biomass =0.0;
        vector <double> tot_N_at_age_end_previous_y = this->get_perceived_tot_N_at_age(); // perceived
        vector <double> tot_W_at_age_y_plus_1 = this->get_tot_W_at_age();
        vector <double> maturity_at_age_y_plus_1 = this->get_tot_Mat_at_age();
        for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
        {
            biomass+= tot_N_at_age_end_previous_y.at(i)* tot_W_at_age_y_plus_1.at(i) * maturity_at_age_y_plus_1.at(i); // SSB
        }
        biomass= biomass/1000; // SSB in tons

        double fmultiplier=1;
        // 2. compare with the target
            fmultiplier = 1.0 *FMSY /fbar_py;
          if(biomass<Btrigger)
          {
          fmultiplier = 1.0 *FMSY*(biomass/Btrigger) /fbar_py;
          cout << "Btrigger apply! " << endl;
          }

         cout << "the fmultiplier for this pop " << this->get_name() <<" is then " << fmultiplier <<
            " given the target F " <<  FMSY << " in the plan..." << endl;

        // 3. perform a short-term forecast on N with F * fmultipier * fmultiplier

        //a. for year y from y-1
        dout(cout << "the  N by age at the end of  y is " << endl);
        for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
        {
            dout(cout << "age" << i+1 << ": " << tot_N_at_age_end_previous_y.at(i) << endl);
        }

        dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                     // init
        vector <double> forecast_tot_N_at_age_start_y(tot_N_at_age_end_previous_y.size());
        for (unsigned int i=1; i < tot_N_at_age_end_previous_y.size(); i++)
        {
                                     // class change from 31th of Dec to 1st of Jan
            forecast_tot_N_at_age_start_y.at(i) = tot_N_at_age_end_previous_y.at(i-1);
        }
                                     // note: forecast_tot_N_at_age_y.at(0) keeps the same=> assuming fixed recruits...
        forecast_tot_N_at_age_start_y.at(0) = tot_N_at_age_end_previous_y.at(0);
        for (unsigned int i=0; i < forecast_tot_N_at_age_start_y.size(); i++)
        {
            dout(cout << "age" << i+1 << ": " << forecast_tot_N_at_age_start_y.at(i) << endl);
        }

        //b. for year y+1 from y
        dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                     //init
        vector <double> forecast_tot_N_at_age_start_y_plus_1 (forecast_tot_N_at_age_start_y.size());
        vector <double> tot_F_at_age_end_previous_y = this->get_tot_F_at_age();
                                     //init
        vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
        for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
        {
                                     // target for y+1
            tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_end_previous_y.at(i)*fmultiplier;
        }
        for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
        {
            dout(cout << "age" << i+1 << ": " << tot_F_at_age_y_plus_1.at(i) << endl);
        }

        dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
        vector <double> tot_M_at_age_y_plus_1 = this->get_tot_M_at_age();
        for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
        {
            dout(cout << "age" << i+1 << ": " << tot_M_at_age_y_plus_1.at(i) << endl);
        }

        dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
        for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
        {
            dout(cout << "age" << i+1 << ": " << tot_W_at_age_y_plus_1.at(i) << endl);
        }

        dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
        for (unsigned int i=1; i < forecast_tot_N_at_age_start_y.size(); i++)
        {
            forecast_tot_N_at_age_start_y_plus_1.at(i) =
                forecast_tot_N_at_age_start_y.at(i-1)*
                exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
            ;						 // class change and apply mortality F+M
        }							 // TO DO: add the plusgroup (if necessary)
        forecast_tot_N_at_age_start_y_plus_1.at(0) = forecast_tot_N_at_age_start_y.at(0);
        // note: forecast_tot_N_at_age_y_plus_1.at(0) keeps the same=> assuming fixed recruits...

        for (unsigned int i=0; i < forecast_tot_N_at_age_start_y_plus_1.size(); i++)
        {
            dout(cout << "age" << i+1 << ": " << forecast_tot_N_at_age_start_y_plus_1.at(i) << endl);
        }

        // 4. compute the TAC (in tons) according to the forecast N (Baronovs equation)
        // TAC per age and then sum over ages....
        for (unsigned int i=1; i < forecast_tot_N_at_age_start_y_plus_1.size(); i++)
        {
            tac_y_plus_1+=   (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
                forecast_tot_N_at_age_start_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
                                     // convert in tons;
                (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))))/ 1000;

        }
        cout << "so, the TAC (in tons) for y+1 will be " << tac_y_plus_1 << endl;



   }

    this->get_tac()->add_tac_y_plus_1(tac_y_plus_1);

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



void Population::export_popdyn_annual_indic(ofstream& popdyn_annual_indic, int tstep)
{

	popdyn_annual_indic << setprecision(4) << fixed;
	// tstep / pop / F at szgroup / tot landings on pop i.e. including oth landings as well
	popdyn_annual_indic << tstep << " " << this->get_name() << " ";
	double oth_mult   =this->get_oth_land_multiplier();
	double cpue_mult  =this->get_cpue_multiplier();

								 // output the annual multiplier
	popdyn_annual_indic  << oth_mult << " " << cpue_mult << " ";

	double fbar_py= this->compute_fbar();
	popdyn_annual_indic  << fbar_py << " ";

								 //...also including the oth land
	double so_far    =this->get_landings_so_far();
	popdyn_annual_indic  << so_far << " ";

	double current_tac=this->get_tac()->get_current_tac();
	popdyn_annual_indic  << current_tac << " ";

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

}
