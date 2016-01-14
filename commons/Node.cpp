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

#include "Node.h"
#include<iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <helpers.h>

#define NBSZGROUP 14








Node::Node(int idx, double xval, double yval,  int _harbour, int _code_area, int _marine_landscape, int nbpops,int nbbenthospops, int nbszgroups)
{
    pthread_mutex_init(&mutex, 0);
	idx_node= idx;
	x=xval;
	y=yval;
	cumftime=0;
    cumsweptarea=0;
    cumcatches=0;
    harbour=_harbour;
	code_area=_code_area;
	marine_landscape=_marine_landscape;
	if(_harbour!=0)
	{
		is_harbour = true;
	}
	else
	{
		is_harbour=false;
	}

    m_nbpops = nbpops;
    m_nbbenthospops = nbbenthospops;
    m_nszgrp = nbszgroups;

    // length 2 for type 0 and 1
    tariffs.push_back(0);
    tariffs.push_back(0);


}


Node::Node(int idx, const vector<double> &graph_coord_x, const vector<double> &graph_coord_y,
           const vector<int> &graph_coord_harbour, const vector<int> &graph_point_code_area,
           const vector<int> &graph_point_marine_landscape, int nbpops, int nbbenthospops, int nbszgroups)
{
    pthread_mutex_init(&mutex, 0);

    UNUSED(nbpops);
    UNUSED(nbbenthospops);
    UNUSED(nbszgroups);

    idx_node= idx;
	x=graph_coord_x[idx];
	y=graph_coord_y[idx];
    cumftime=0;
    cumsweptarea=0;
    cumcatches=0;
    harbour=graph_coord_harbour[idx];
	code_area=graph_point_code_area[idx];
	marine_landscape=graph_point_marine_landscape[idx];
	if(harbour!=0)
	{
		is_harbour = true;
	}
	else
	{
		is_harbour=false;
	}

    // length 2 for type 0 and 1
    tariffs.push_back(0);
    tariffs.push_back(0);


}


Node::Node()
    : idx_node(0),
      x(0),
      y(0),
      harbour(0),
      code_area(0),
      marine_landscape(0),
      is_harbour(false),
      cumftime(0),
      cumsweptarea(0),
      cumcatches(0),
      Ns_pops_at_szgroup(),
      Ns_pops_at_szgroup_at_month_start(),
      removals_pops_at_szgroup(),
      pressure_pops_at_szgroup(),
      avai_pops_at_selected_szgroup(),
      impact_on_pops(),
      cumcatches_per_pop(),
      vid(),
      pop_names_on_node(),
      benthos_tot_biomass(),
      tariffs(),
      m_nbpops(0),
      m_nbbenthospops(0),
      m_nszgrp(0)
{



    pthread_mutex_init(&mutex, 0);
}


Node::~Node()
{

}


int Node::get_idx_node() const
{
	return(idx_node);
}

void Node::set_idx_node(int idx)
{
    idx_node = idx;
}

int Node::get_code_area() const
{
	return(code_area);
}


int Node::get_marine_landscape() const
{
	return(marine_landscape);
}


string Node::get_name() const
{
	return("at_sea");
}


/*
double Node::get_prices(string met, int pop)
{
	return(0);
}
*/

double Node::get_prices_per_cat(int pop, int cat)
{
    UNUSED(pop);
    UNUSED(cat);

	return(0);
}


double Node::get_fuelprices(int vsize)
{
    UNUSED(vsize);

	return(0);
}


const vector<int> &Node::get_usual_fgrounds() const
{
    return(vector<int> (0));
}


const vector<double> &Node::get_freq_usual_fgrounds() const
{
    return(vector<double> (0));
}

void Node::set_usual_fgrounds(vector <int> _usual_fgrounds)
{
}


void Node::set_freq_usual_fgrounds(vector <double> _freq_usual_fgrounds)
{
}



vector<int> Node::get_usual_fgrounds_this_met(int met)
{
    return(vector<int> (0));
}


vector<double> Node::get_freq_usual_fgrounds_this_met(int met)
{
    return(vector<double> (0));
}

void Node::set_usual_fgrounds_per_met(multimap <int,int> _usual_fgrounds_per_met)
{
}


void Node::set_freq_usual_fgrounds_per_met(multimap <int,double> _freq_usual_fgrounds_per_met)
{
}





const multimap<int,int> &Node::get_usual_metiers() const
{
    multimap<int,int> zero;
     return(zero);

}


const multimap<int,double> &Node::get_freq_usual_metiers() const
{
   multimap<int,double> zero;
    return(zero);
}

void Node::set_usual_metiers(multimap<int,int> _usual_metiers)
{
}


void Node::set_freq_usual_metiers(multimap<int,double> _freq_usual_metiers)
{
}





double Node::get_x() const
{
	return(x);
}


double Node::get_y () const
{
	return(y);
}


int Node::get_harbour () const
{
	return(harbour);
}


bool Node::get_is_harbour () const
{
	return(is_harbour);
}


const vector<double> &Node::get_Ns_pops_at_szgroup(int name_pop) const
{
#if 0
	vector<double> a_Ns_pops_at_szgroup;

	for(unsigned int j = 0; j < Ns_pops_at_szgroup[name_pop].size(); j++)
	{
		a_Ns_pops_at_szgroup.push_back(Ns_pops_at_szgroup[name_pop] [j]);
	}

	return(a_Ns_pops_at_szgroup);
#endif

    return Ns_pops_at_szgroup[name_pop];
}


vector<double> Node::get_Ns_pops_at_szgroup_at_month_start(int name_pop) const
{

	vector<double> a_Ns_pops_at_szgroup_at_month_start;

	for(unsigned int j = 0; j < Ns_pops_at_szgroup_at_month_start[name_pop].size(); j++)
	{
		a_Ns_pops_at_szgroup_at_month_start.push_back(Ns_pops_at_szgroup_at_month_start[name_pop] [j]);
	}

	return(a_Ns_pops_at_szgroup_at_month_start);
}


vector<double> Node::get_removals_pops_at_szgroup(int name_pop) const
{

	vector<double> a_removals_pops_at_szgroup;

	for(unsigned int j = 0; j < removals_pops_at_szgroup[name_pop].size(); j++)
	{
		a_removals_pops_at_szgroup.push_back(removals_pops_at_szgroup[name_pop] [j]);
	}

	return(a_removals_pops_at_szgroup);
}


vector<double>  Node::get_pressure_pops_at_szgroup(int name_pop) const
{

	vector<double> a_pressure_pops_at_szgroup;

	for(unsigned int j = 0; j < pressure_pops_at_szgroup[name_pop].size(); j++)
	{
		a_pressure_pops_at_szgroup.push_back(pressure_pops_at_szgroup[name_pop] [j]);
	}

	return(a_pressure_pops_at_szgroup);
}


vector<double>  Node::get_avai_pops_at_selected_szgroup(int name_pop) const
{

	vector<double> a_avai_pops_at_selected_szgroup;

	for(unsigned int j = 0; j < avai_pops_at_selected_szgroup[name_pop].size(); j++)
	{
		a_avai_pops_at_selected_szgroup.push_back(avai_pops_at_selected_szgroup[name_pop] [j]);
	}

	return(a_avai_pops_at_selected_szgroup);
}


double  Node::get_benthos_tot_biomass(int funcgr) const
{

	return(benthos_tot_biomass.at(funcgr));
}


const vector <double> &Node::get_benthos_tot_biomass() const
{

	return(benthos_tot_biomass);
}

double  Node::get_tariffs(int type) const
{

    return(tariffs.at(type));
}


const vector <double> &Node::get_tariffs() const
{

    return(tariffs);
}

vector <vector <double> > Node::get_Ns_pops_at_szgroup() const
{

	return(Ns_pops_at_szgroup);
}


vector<int> Node::get_vid ()
{
    lock();
    vector<int> r = vid;
    unlock();
    return r;
}


int Node::get_cumftime () const
{
	return(cumftime);
}

double Node::get_cumsweptarea() const
{
    return(cumsweptarea);
}

double Node::get_cumcatches() const
{
    return(cumcatches);
}

vector<int> Node::get_pop_names_on_node ()
{
	return(pop_names_on_node);
}


const vector<double>& Node::get_impact_on_pops ()
{
	return(impact_on_pops);
}

const vector<double>& Node::get_cumcatches_per_pop ()
{
    return(cumcatches_per_pop);
}

void Node::set_vid (int val)
{
    pthread_mutex_lock(&mutex);
    vid.push_back(val);
    pthread_mutex_unlock(&mutex);
}

void Node::set_cumftime(int tot)
{
    cumftime = tot;
}

void Node::set_cumsweptarea(double tot)
{
    cumsweptarea = tot;
}

void Node::set_cumcatches(double tot)
{
    cumcatches = tot;
}


void Node::set_xy(double xval, double yval)
{
	x = xval;
	y = yval;
}


void Node::add_to_cumftime(int delta_time)
{
    lock();
	cumftime+=delta_time;
    unlock();
}

void Node::add_to_cumsweptarea(double sweptarea)
{
    lock();
    cumsweptarea+=sweptarea;
    unlock();
}

void Node::add_to_cumcatches(double catches)
{
    lock();
    cumcatches+=catches;
    unlock();
}


void Node::reinit(vector<double> &vec, unsigned int sz)
{
    if (vec.size() != sz)
        vec.resize(sz, 0);
    for (unsigned int i = 0; i < sz; ++i)
        vec[i] = sz;
}

void Node::reinit(vector<vector<double> > &vec, unsigned int sz, unsigned int subsz)
{
    if (vec.size() != sz)
        vec.resize(sz);
    for (unsigned int i = 0; i < sz; ++i)
        reinit(vec[i], subsz);
}

void Node::init_Ns_pops_at_szgroup(int nbpops, int nbszgroups)
{
    reinit (Ns_pops_at_szgroup, nbpops, nbszgroups);
    reinit (Ns_pops_at_szgroup_at_month_start, nbpops, nbszgroups);
    reinit (removals_pops_at_szgroup, nbpops, nbszgroups);
    reinit (pressure_pops_at_szgroup, nbpops, nbszgroups);
    reinit (impact_on_pops, nbpops);
    reinit (cumcatches_per_pop, nbpops);

#if 0
	// init at 0 the matrix of Ns
    dout(cout  << "init matrix of Ns" << endl);
	vector< vector<double> > init_Ns_pops_at_szgroup(nbpops, vector<double>(nbszgroups));
	Ns_pops_at_szgroup= init_Ns_pops_at_szgroup;
	Ns_pops_at_szgroup_at_month_start= init_Ns_pops_at_szgroup;
	removals_pops_at_szgroup= init_Ns_pops_at_szgroup;
	pressure_pops_at_szgroup= init_Ns_pops_at_szgroup;
    for(int i = 0; i < nbpops; i++)
	{
        for(int j = 0; j < nbszgroups; j++)
		{
			Ns_pops_at_szgroup.at(i).at(j) = 0;
			Ns_pops_at_szgroup_at_month_start.at(i).at(j) = 0;
            dout(cout  << Ns_pops_at_szgroup[i][j] << " ");
			pressure_pops_at_szgroup.at(i).at(j) = 0;
			removals_pops_at_szgroup.at(i).at(j) = 0;
		}
        dout(cout  << endl);

		impact_on_pops.push_back(0);
        cumcatches_per_pop.push_back(0);
    }
#endif
}


void Node::init_avai_pops_at_selected_szgroup(int nbpops, int selected_nbszgroups)
{
    reinit(avai_pops_at_selected_szgroup, nbpops, selected_nbszgroups);
#if 0
	// init at 0 the matrix of Ns
    dout(cout  << "init matrix of Ns" << endl);
	vector< vector<double> > init_avai_pops_at_selected_szgroup(nbpops, vector<double>(selected_nbszgroups));
	avai_pops_at_selected_szgroup= init_avai_pops_at_selected_szgroup;
    for(int i = 0; i < nbpops; i++)
	{
        for(int j = 0; j < selected_nbszgroups; j++)
		{
			avai_pops_at_selected_szgroup.at(i).at(j) = 0;
            dout(cout  << avai_pops_at_selected_szgroup[i][j] << " ");
		}
        dout(cout  << endl);
	}
#endif
}


void Node::set_Ns_pops_at_szgroup(int name_pop, const vector<double>& newval)
{
	for(unsigned int j = 0; j < Ns_pops_at_szgroup[name_pop].size(); j++)
	{
		Ns_pops_at_szgroup[name_pop][j]= newval[j];
	}

}


void Node::set_Ns_pops_at_szgroup_at_month_start(int name_pop, const vector<double>& newval)
{
	for(unsigned int j = 0; j < Ns_pops_at_szgroup_at_month_start[name_pop].size(); j++)
	{
		Ns_pops_at_szgroup_at_month_start[name_pop][j]= newval[j];
	}

}


void Node::set_removals_pops_at_szgroup(int name_pop, const vector<double>& newval)
{
	for(unsigned int j = 0; j < removals_pops_at_szgroup[name_pop].size(); j++)
	{
		removals_pops_at_szgroup[name_pop][j]= newval[j];
	}

}


void Node::set_pressure_pops_at_szgroup(int name_pop, const vector<double>& newval)
{
	for(unsigned int j = 0; j < pressure_pops_at_szgroup[name_pop].size(); j++)
	{
		pressure_pops_at_szgroup[name_pop][j]= newval[j];
	}

}


void Node::set_avai_pops_at_selected_szgroup(int name_pop, const vector<double>& newval)
{
	for(unsigned int j = 0; j < avai_pops_at_selected_szgroup[name_pop].size(); j++)
	{
		avai_pops_at_selected_szgroup[name_pop][j]= newval[j];
	}

}


void Node::set_impact_on_pops(int name_pop, double newval)
{

	impact_on_pops.at(name_pop)=newval;

}

void Node::set_cumcatches_per_pop(int name_pop, double newval)
{

    cumcatches_per_pop.at(name_pop)=newval;

}

void Node::set_pop_names_on_node(int name_pop)
{
	pop_names_on_node.push_back(name_pop);
}


void Node::clear_pop_names_on_node()
{
	pop_names_on_node.clear();
}


void  Node::set_benthos_tot_biomass(int funcgr, double value)
{

	benthos_tot_biomass.at(funcgr)= value;
}

void  Node::set_tariffs(int type, double value)
{

    tariffs.at(type)= value;
}

void  Node::set_tariffs(vector<double> values)
{

    tariffs= values;
}


void Node::clear_Ns_pops_at_szgroup()
{
    dout(cout  << "clear pop on nodes..." << endl);
	for(unsigned int i = 0; i < Ns_pops_at_szgroup.size(); i++)
	{
		for(unsigned int j = 0; j < Ns_pops_at_szgroup[i].size(); j++)
		{
			Ns_pops_at_szgroup[i][j] = 0;
		}
	}
}


void Node::clear_removals_pops_at_szgroup()
{
    dout(cout  << "clear removals on nodes..." << endl);
	for(unsigned int i = 0; i < removals_pops_at_szgroup.size(); i++)
	{
		for(unsigned int j = 0; j < removals_pops_at_szgroup[i].size(); j++)
		{
			removals_pops_at_szgroup[i][j] = 0;
		}
	}
}


void Node::clear_impact_on_pops()
{
    for(unsigned int i=0; i<impact_on_pops.size(); i++)
	{
		impact_on_pops.at(i)=0;
	}

}

void Node::clear_cumcatches_per_pop()
{
    for(unsigned int i=0; i<cumcatches_per_pop.size(); i++)
    {
        cumcatches_per_pop.at(i)=0;
    }

}

void Node::clear_avai_pops_at_selected_szgroup()
{
    dout(cout  << "clear pop on nodes..." << endl);
	for(unsigned int i = 0; i < avai_pops_at_selected_szgroup.size(); i++)
	{
		for(unsigned int j = 0; j < avai_pops_at_selected_szgroup[i].size(); j++)
		{
			avai_pops_at_selected_szgroup[i][j] = 0;
		}
	}
}


void Node::apply_natural_mortality_at_node(int name_pop, const vector<double>& M_at_szgroup)
{
    //dout(cout  << "BEGIN: apply_natural_mortality_at_node()" << endl);

	vector <double> a_Ns_at_szgroup = get_Ns_pops_at_szgroup(name_pop);
	for(unsigned int i=0; i<a_Ns_at_szgroup.size(); i++)
	{
								 // divide according to tstep
		a_Ns_at_szgroup[i] =  a_Ns_at_szgroup[i]  *exp(-M_at_szgroup[i]/12);
		//this is assuming that the M is uniformly applied to the pop
		// e.g. 1000*exp(-0.2) = 225*exp(-0.2)+ 775*exp(-0.2)
		// (the pble with spatial scale is that we cannot do e.g. 225*exp(-0.1)+ 775*exp(-0.3) because = 1000*exp(-x) and need to solve for x)
	}

	set_Ns_pops_at_szgroup(name_pop, a_Ns_at_szgroup);

    //dout(cout  << "END: apply_natural_mortality_at_node()" << endl);
}


void Node::apply_oth_land(int name_pop, double &oth_land_this_pop_this_node,
                          const vector<double>&  weight_at_szgroup, const vector<double>& totN)
{
	//cout << "BEGIN: apply_oth_land()" << endl;

	// DISSAGREGATE TOTAL CATCH (FROM OTHERS) IN WEIGHT INTO SZGROUP
	// AND CONVERT INTO REMOVALS IN NUMBERS
	// NOTICE THAT THIS IS THE SAME PROCEDURE THAN THE ONE IN do.catch()
	vector <double> Ns_at_szgroup_pop = this->get_Ns_pops_at_szgroup(name_pop);
								 // ...input to be compared with output!
	vector <double> init_Ns_at_szgroup_pop = Ns_at_szgroup_pop;

								 // init
	vector <double>avail_biomass         = Ns_at_szgroup_pop;
								 // init
	vector <double>alloc_key             = Ns_at_szgroup_pop;
								 // init
	vector <double>catch_per_szgroup     = Ns_at_szgroup_pop;
								 // init
	vector <double>removals_per_szgroup  = Ns_at_szgroup_pop;
								 // init
	vector <double>new_Ns_at_szgroup_pop = Ns_at_szgroup_pop;
	vector <double>cumul_removals_at_szgroup_pop=this->get_removals_pops_at_szgroup(name_pop);
								 // init
	vector <double>new_removals_at_szgroup_pop=Ns_at_szgroup_pop;

	double tot            = 0;	 //init

	// (rough) assumption for selectivity ogive of others => 1,1,1,1,1,1,etc.
	//vector <double> sel_ogive (NBSZGROUP, 1);
    // MAGIC NUMBERS
	double clupeid_sel_ogive [ ] =
	{
		0.000601409471228288, 0.00929350798322286, 0.127575084407118,
		0.695076124968439, 0.972628235474595, 0.998197939139254, 0.999884202454116, 0.99999257077887,1,1,1,1,1,1
	};							 // same as for OTM_SPF
	double gadoid_sel_ogive [ ] =
	{
		0.00000225976, 0.00001410136387, 0.00008798955470,
		0.000548823892218543, 0.00341497770850069, 0.0209356687119669, 0.117728102864771,
		0.45435195117619,0.838609522203591,0.970082540287596,0.995082177119589,0.999208651051219,0.999873102040564,1
	};							 // same as for OTB_DEF
	vector<double> clup_sel_ogive (clupeid_sel_ogive, clupeid_sel_ogive + sizeof(clupeid_sel_ogive) / sizeof(double) );
	vector<double> gad_sel_ogive (gadoid_sel_ogive, gadoid_sel_ogive + sizeof(gadoid_sel_ogive) / sizeof(double) );
	vector<double> sel_ogive;
    if(name_pop==10 || name_pop==11) // HARDCODING
	{
		sel_ogive =gad_sel_ogive;
	}
	else						 // i.e. 3 and 7
	{
		sel_ogive =clup_sel_ogive;
	}

	//vector <double> dis_ogive (NBSZGROUP, 0);
	//double default_dis_ogive [ ] = {0.5,0.1,0.012,0.001,0,0,0,0,0,0,0,0,0,0};
	double default_dis_ogive [ ] = {0.634,0.366,0.161,0.06,0.021,0.007,0.002,0.001,0,0,0,0,0,0};
	vector<double> dis_ogive (default_dis_ogive, default_dis_ogive + sizeof(default_dis_ogive) / sizeof(double) );

	// compute available biomass via selectivity
	for(unsigned int szgroup=0; szgroup <avail_biomass.size(); szgroup++)
	{
		avail_biomass[szgroup]=  Ns_at_szgroup_pop[szgroup]*weight_at_szgroup[szgroup]*sel_ogive[szgroup];
								 // cumul
		tot = tot+avail_biomass[szgroup];
        dout(cout  << "avail_biomass[szgroup] " <<avail_biomass[szgroup] << endl);
	}
    dout(cout  << "tot biomass available" << tot << endl);

	if(tot!=0)
	{
								 // init
		vector <double> new_avai_pops_at_selected_szgroup = this->get_avai_pops_at_selected_szgroup(name_pop);

		for(unsigned int szgroup=0; szgroup <avail_biomass.size(); szgroup++)
		{
			if(avail_biomass[szgroup] !=0)
			{
				// compute alloc key
								 // proportion
				alloc_key[szgroup]=avail_biomass[szgroup] /(tot);
                dout(cout  << "alloc_key[szgroup] " <<alloc_key[szgroup] << endl);

				// disaggregate total catch (in weight) for this pop according to the alloc key
				catch_per_szgroup[szgroup]= oth_land_this_pop_this_node * alloc_key[szgroup];
                if(idx_node==2436 && name_pop==9) dout(cout << " catch_per_szgroup[szgroup] from oth land " << catch_per_szgroup[szgroup] << endl);

				// catch_per_szgroup at this point are actually landings (see parameterisation)
				// so we need to add the part of removals that are actually discarded by the sorting on board.
				// discard ogive over size e.g. 98%, 98%, 50%, 10%, 0%, 0%...........
				catch_per_szgroup[szgroup]=catch_per_szgroup[szgroup] /(1-dis_ogive[szgroup]);

				// then get the removals in terms of N
				removals_per_szgroup[szgroup]= catch_per_szgroup[szgroup]/weight_at_szgroup[szgroup];
                if(idx_node==2436 && name_pop==9) dout(cout << " removals_per_szgroup[szgroup] from oth land" << removals_per_szgroup[szgroup] << endl);

				// do not allow negative abundance!
				if(removals_per_szgroup[szgroup] > new_Ns_at_szgroup_pop[szgroup])
				{

					catch_per_szgroup[szgroup]=(Ns_at_szgroup_pop[szgroup])*weight_at_szgroup[szgroup];
                    dout  (cout << "the szgroup " << szgroup <<
						"for this pop " << name_pop << " is fully depleted on this node by other land. " <<
						idx_node << "! catch is "<<
                        catch_per_szgroup[szgroup] << endl);

								 // take all...
					removals_per_szgroup[szgroup]=new_Ns_at_szgroup_pop[szgroup];
								 // nothing left.
					new_Ns_at_szgroup_pop[szgroup]=0;
								 // just right after the calculation of removals, reverse back to get the landings only
					catch_per_szgroup[szgroup]=catch_per_szgroup[szgroup] *(1-dis_ogive[szgroup]);

				}
				else
				{
					// finally, impact the N
					new_Ns_at_szgroup_pop[szgroup]=Ns_at_szgroup_pop[szgroup]-removals_per_szgroup[szgroup];
								 // reverse back to get the landings only
					catch_per_szgroup[szgroup]=catch_per_szgroup[szgroup] *(1-dis_ogive[szgroup]);
                    //if(idx_node==430&& name_pop==3) dout(cout << " new_Ns_at_szgroup_pop[szgroup] " << new_Ns_at_szgroup_pop[szgroup] << endl);

					// update the availability to impact the future vessel cpue
					double val=0;// init
					if(szgroup==0 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
					{
						val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
						new_avai_pops_at_selected_szgroup.at(0)=val;
					}
					if(szgroup==2 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
					{
						val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
						new_avai_pops_at_selected_szgroup.at(1)=val;
					}
					if(szgroup==3 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
					{
						val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
						new_avai_pops_at_selected_szgroup.at(2)=val;
					}
					if(szgroup==5 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
					{
						val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
						new_avai_pops_at_selected_szgroup.at(3)=val;
					}
					if(szgroup==7 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
					{
						val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
						new_avai_pops_at_selected_szgroup.at(4)=val;
					}
					this->set_avai_pops_at_selected_szgroup(name_pop, new_avai_pops_at_selected_szgroup);

				}

				// new removals cumul on this node
                //if(idx_node==430&& name_pop==3) dout(cout << " cumul_removals_at_szgroup_pop[szgroup] " << cumul_removals_at_szgroup_pop[szgroup] << endl);
                //if(idx_node==430&& name_pop==3) dout(cout << " removals_per_szgroup[szgroup] " << removals_per_szgroup[szgroup] << endl);

				new_removals_at_szgroup_pop[szgroup]=cumul_removals_at_szgroup_pop[szgroup]+removals_per_szgroup[szgroup];

                //if(idx_node==430&& name_pop==3) dout(cout << " new_removals_at_szgroup_pop[szgroup] after oth land" << new_removals_at_szgroup_pop[szgroup] << endl);

			}
			else
			{
				new_Ns_at_szgroup_pop[szgroup]=0;
				removals_per_szgroup[szgroup]=0;
				new_removals_at_szgroup_pop[szgroup]=cumul_removals_at_szgroup_pop[szgroup]+removals_per_szgroup[szgroup];

			}
		}

		// additionally, impact the catches realized on this node
        dout(cout  << "oth_land this pop this node, before: "<<  oth_land_this_pop_this_node << endl);
		oth_land_this_pop_this_node=0;
        for(unsigned int szgroup=0; szgroup < catch_per_szgroup.size(); szgroup++)
		{
			oth_land_this_pop_this_node+=catch_per_szgroup.at(szgroup);
		}
        dout(cout  << "oth_land this pop this node, after potential correction (when total depletion): "<<  oth_land_this_pop_this_node << endl);

		this->set_Ns_pops_at_szgroup(name_pop, new_Ns_at_szgroup_pop);
		this->set_removals_pops_at_szgroup(  name_pop, new_removals_at_szgroup_pop);

	}
	else
	{

	}

	// check for consistency i.e. no gain in N !!
	//for(unsigned int szgroup=0; szgroup <init_Ns_at_szgroup_pop.size(); szgroup++)
	//{
	//    if(new_Ns_at_szgroup_pop.at(szgroup) > init_Ns_at_szgroup_pop.at(szgroup))
	//    {
	//        cout << "inconsistency in node->apply_oth_land() for this pop " << name_pop <<
	//                  " on this node " << idx_node <<
	//                    " for this szgroup " <<  szgroup <<endl;
    //        dout(cout << "new_Ns_at_szgroup_pop is " << new_Ns_at_szgroup_pop.at(szgroup) << endl);
    //        dout(cout << "while init_Ns_at_szgroup_pop is " << init_Ns_at_szgroup_pop.at(szgroup) << endl);
	//        int a_int;
	//        cin >> a_int; // pause
	//    }
	//}

	//cout << "END: apply_oth_land()" << endl;
}


void Node::export_popnodes(ofstream& popnodes,  multimap<int,double> weight_at_szgroup, int tstep)
{
	// note : pble with c++: circularity impossible then need to call init_weight_atszgroup
	// instead of using population->get_weight_at_szgroup()

    dout(cout  << "export biomass on nodes for use in e.g. a GIS engine" << endl);
    // note that this file will also be used by the ui for displaying the statistics on node

	popnodes << setprecision(3) << fixed;
	// tstep / node / long / lat / tot N sp0 / tot N sp1 /...
	popnodes << tstep << " " << this->get_idx_node() << " "<<
		" " << this->get_x() << " " << this->get_y();

    double totN_this_pop, totW_this_pop;
    for(unsigned int name_pop = 0; name_pop < Ns_pops_at_szgroup.size(); name_pop++)
	{
		totN_this_pop=0;		 // re-init
        totW_this_pop= 0.0;
		multimap<int,double>::iterator lower_init_we = weight_at_szgroup.lower_bound(name_pop);
		multimap<int,double>::iterator upper_init_we = weight_at_szgroup.upper_bound(name_pop);
		vector<double> weight_at_szgroup;
		for (multimap<int, double>::iterator pos=lower_init_we; pos != upper_init_we; pos++)
			weight_at_szgroup.push_back(pos->second);

        for(unsigned int sz = 0; sz < Ns_pops_at_szgroup[name_pop].size(); sz++)
		{
            totN_this_pop+= Ns_pops_at_szgroup[name_pop] [sz];
            totW_this_pop+= Ns_pops_at_szgroup[name_pop] [sz] * weight_at_szgroup.at(sz);
        }
        popnodes << " " << totN_this_pop << " " << totW_this_pop;
	}

	popnodes << " " <<  endl;

}


void Node::export_popnodes_impact(ofstream& popnodes, int tstep, int pop)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << endl);
    // note that this file will also be used by the ui for displaying the statistics on node

	popnodes << setprecision(8) << fixed;
	// tstep / node / long / lat /  tot impact pop
	popnodes << pop << " " << tstep << " " << this->get_idx_node() << " "<<
		" " << this->get_x() << " " << this->get_y() << " " <<
		impact_on_pops.at(pop) << " " <<  endl;

}

void Node::export_popnodes_cumcatches_per_pop(ofstream& popnodes, int tstep, int pop)
{

    dout(cout  << "export accumulated catches (i.e. landings) on nodes for use in e.g. a GIS engine" << endl);
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  tot cumcatches pop
    popnodes << pop << " " << tstep << " " << this->get_idx_node() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        cumcatches_per_pop.at(pop) << " " <<  endl;

}

void Node::export_popnodes_impact_per_szgroup(ofstream& popnodes, int tstep, int pop)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << endl);
    // note that this file will also be used by the ui for displaying the statistics on node

	vector<double> impact_per_szgroup=get_pressure_pops_at_szgroup(pop);

	popnodes << setprecision(3) << fixed;
	// pop/ tstep / node / long / lat /  impact sz0 / impact sz1 /...
	popnodes << pop << " " << tstep << " " << this->get_idx_node() << " "<<
		" " << this->get_x() << " " << this->get_y() ;

    for(unsigned int sz = 0; sz < impact_on_pops.size(); sz++)
	{
		popnodes << " " << impact_on_pops.at(sz);
	}

	popnodes << " " <<  endl;

}


void Node::export_popnodes_cumftime(ofstream& popnodes, int tstep)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << endl);
    // note that this file will also be used by the ui for displaying the statistics on node

	popnodes << setprecision(8) << fixed;
	// tstep / node / long / lat /  tot impact pop
	popnodes << " " << tstep << " " << this->get_idx_node() << " "<<
		" " << this->get_x() << " " << this->get_y() << " " <<
		cumftime << " " <<  endl;

}

void Node::export_popnodes_cumsweptarea(ofstream& popnodes, int tstep)
{

    dout(cout  << "export swept area on nodes for use in e.g. a GIS engine" << endl);
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  swept area
    popnodes << " " << tstep << " " << this->get_idx_node() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        cumsweptarea << " " <<  endl;

}

void Node::export_popnodes_cumcatches(ofstream& popnodes, int tstep)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << endl);
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  tot impact pop
    popnodes << " " << tstep << " " << this->get_idx_node() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        cumcatches << " " <<  endl;

}

void Node::export_popnodes_tariffs(ofstream& popnodes, int tstep)
{

    dout(cout  << "export tariffs on nodes for use in e.g. a GIS engine" << endl);
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  tariffs
    popnodes << " " << tstep << " " << this->get_idx_node() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        tariffs.at(0) << " " <<  endl;

}

void Node::add_benthos_tot_biomass_on_node(double tot_biomass_this_group)
{

	benthos_tot_biomass.push_back(tot_biomass_this_group);

}

void Node::set_is_harbour(int id)
{
    harbour = id;
    is_harbour = true;
}


void Node::export_benthos_tot_biomass_per_funcgroup(ofstream& benthosnodes, int tstep, int funcgr)
{

    dout(cout  << "export benthos on nodes for use in e.g. a GIS engine" << endl);

	benthosnodes << setprecision(3) << fixed;
    // pop/ tstep / node / long / lat / biomass func group id
    benthosnodes << funcgr << " " << tstep << " " << this->get_idx_node() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " << benthos_tot_biomass.at(funcgr) << " " <<  endl;

}



void Node::recover_benthos_tot_biomass_per_funcgroup()
{
	// A MAGIC NUMBER HERE FOR THE TIME BEING...
	// i.e. 1% per month
	// TO DO: INFORM WITH DATA.

    dout(cout  << "the benthos recovering...." << endl);

    for(unsigned int funcgr = 0; funcgr < benthos_tot_biomass.size(); funcgr++)
	{
		benthos_tot_biomass.at(funcgr)=benthos_tot_biomass.at(funcgr)*1.01;
	}

}


int Node::setAreaType(int _area_type)
{
area_type=_area_type;
}


int Node::evaluateAreaType()
{
return area_type;
}

