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

#include "Node.h"
#include<iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <helpers.h>

#define NBSZGROUP 14



const vector<types::NodeId> Node::mUsualFGrounds;
const vector<double> Node::mFreqUsualFGrounds;
const vector<types::NodeId> Node::mUsualFGroundsMet;
const vector<double> Node::mFreqUsualFGroundsMet;
const multimap<types::NodeId,int> Node::mUsualMetiers;
const multimap<int,double> Node::mFreqUsualMetiers;


Node::Node(types::NodeId idx, double xval, double yval,  int _harbour, int _code_area,
           int _marine_landscape, double _marine_landscape_norm, double _marine_landscape_alpha,
           double _wind, double _wind_norm, double _wind_alpha,
           double _sst, double _sst_norm, double _sst_alpha,
           double _salinity, double _salinity_norm, double _salinity_alpha,
           double _Nitrogen, double _Nitrogen_norm, double _Nitrogen_alpha,
           double _Phosphorus,double _Phosphorus_norm, double _Phosphorus_alpha,
           double _Oxygen, double _Oxygen_norm, double _Oxygen_alpha,
           double _DissolvedCarbon, double _DissolvedCarbon_norm, double _DissolvedCarbon_alpha,
           double _bathymetry, double _shippingdensity, double _siltfraction, double _icesrectanglecode,
           double _benthos_biomass, double _benthos_number, double _benthos_meanweight,
           double _benthos_biomass_K, double _benthos_number_K,
           int nbpops, int nbmets, int nbbenthospops, int nbszgroups)
    : idx_node(idx)
{
	x=xval;
	y=yval;
	cumftime=0;
    cumsweptarea=0;
    cumsubsurfacesweptarea=0;
    cumcatches=0;
    cumcatches_with_threshold=0;
    cumdiscards=0;
    cumdiscardsratio=0;
    nbchoked=0;
    harbour=_harbour;
	code_area=_code_area;
    benthos_id=0;
    marine_landscape=_marine_landscape;
    marine_landscape_norm=_marine_landscape_norm;
    marine_landscape_alpha=_marine_landscape_alpha;
    wind=_wind;
    wind_norm=_wind_norm;
    wind_alpha=_wind_alpha;
    sst=_sst;
    sst_norm=_sst_norm;
    sst_alpha=_sst_alpha;
    salinity=_salinity;
    salinity_norm=_salinity_norm;
    salinity_alpha=_salinity_alpha;
    Nitrogen=_Nitrogen;
    Nitrogen_norm=_Nitrogen_norm;
    Nitrogen_alpha=_Nitrogen_alpha;
    Phosphorus=_Phosphorus;
    Phosphorus_norm=_Phosphorus_norm;
    Phosphorus_alpha=_Phosphorus_alpha;
    Oxygen=_Oxygen;
    Oxygen_norm=_Oxygen_norm;
    Oxygen_alpha=_Oxygen_alpha;
    DissolvedCarbon=_DissolvedCarbon;
    DissolvedCarbon_norm=_DissolvedCarbon_norm;
    DissolvedCarbon_alpha=_DissolvedCarbon_alpha;
    bathymetry=_bathymetry;
    shippingdensity=_shippingdensity;
    siltfraction=_siltfraction;
    icesrectanglecode= _icesrectanglecode;
    benthos_biomass=_benthos_biomass;
    benthos_number=_benthos_number;
    benthos_meanweight=_benthos_meanweight;
    benthos_biomass_K=_benthos_biomass_K;
    benthos_number_K=_benthos_number_K;
    area_type=0;

    if(_harbour>0)
	{
		is_harbour = true;
        cout << "Node " << idx.toIndex() << " is an harbour because _harbour is " << _harbour << "\n";
	}
	else
	{
		is_harbour=false;
	}

    m_nbpops = nbpops;
    m_nbmets = nbmets;
    m_nbbenthospops = nbbenthospops;
    m_nszgrp = nbszgroups;

    // a tariff map per met
    for (int i = 0; i < nbmets; i++) tariffs.push_back(0);
   
    // initialize the vectors
    totNs_per_pop.resize(m_nbpops);
    totWs_per_pop.resize(m_nbpops);
    impact_per_pop.resize(m_nbpops);
    cumcatches_per_pop.resize(m_nbpops);
    cumdiscards_per_pop.resize(m_nbpops);

}

/*
Node::Node(types::NodeId idx, const vector<double> &graph_coord_x, const vector<double> &graph_coord_y,
           const vector<int> &graph_coord_harbour,
           const vector<int> &graph_point_code_area,
           const vector<int> &graph_point_marine_landscape,
           const vector<double> &graph_point_wind,
           const vector<double> &graph_point_sst,
           const vector<double> &graph_point_salinity,
           const vector<double> &graph_benthos_biomass,
           const vector<double> &graph_benthos_number, double initmw,
           int nbpops, int nbbenthospops, int nbszgroups)
{
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
    wind=graph_point_wind[idx];
    sst=graph_point_sst[idx];
    salinity=graph_point_salinity[idx];
    benthos_biomass=graph_benthos_biomass[idx];
    benthos_number=graph_benthos_number[idx];
    benthos_meanweight=initmw;
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
*/

Node::Node()
    : idx_node(0),
      x(0),
      y(0),
      harbour(0),
      code_area(0),
      marine_landscape(0),
      wind(0),
      sst(0),
      salinity(0),
      Nitrogen(0),
      Phosphorus(0),
      Oxygen(0),
      DissolvedCarbon(0),
      bathymetry(0),
      shippingdensity(0), 
      siltfraction(0),
      icesrectanglecode(0),
      benthos_id(0),
      benthos_biomass(1),
      benthos_number(1),
      is_harbour(false),
      cumftime(0),
      cumsweptarea(0),
      cumsubsurfacesweptarea(0),
      cumcatches(0),
      cumdiscards(0),
      cumdiscardsratio(0),
      nbchoked(0),
      Ns_pops_at_szgroup(),
      Ns_pops_at_szgroup_at_month_start(),
      removals_pops_at_szgroup(),
      last_oth_catch_pops_at_szgroup(),
      last_oth_disc_pops_at_szgroup(),
      pressure_pops_at_szgroup(),
      avai_pops_at_selected_szgroup(),
      totNs_per_pop(),
      totWs_per_pop(),
      impact_per_pop(),
      cumcatches_per_pop(),
      cumcatches_per_pop_this_month(),
      cumdiscards_per_pop(),
      vid(),
      pop_names_on_node(),
      benthos_tot_biomass(),
      benthos_tot_number(),
      benthos_tot_meanweight(),
      benthos_tot_biomass_K(0),
      benthos_tot_number_K(0),
      tariffs(),
      m_nbpops(0),
      m_nbmets(0),
      m_nbbenthospops(0),
      m_nszgrp(0)
{
}


Node::~Node()
{

}


types::NodeId Node::get_idx_node() const
{
    return (idx_node);
}

void Node::set_idx_node(types::NodeId idx)
{
    idx_node = idx;
}

int Node::get_code_area() const
{
    return (code_area);
}


int Node::get_benthos_id() const
{
    return (benthos_id);
}

void Node::set_benthos_id(int _benthos_id)
{
    benthos_id=_benthos_id;
}

int Node::get_marine_landscape() const
{
    return(marine_landscape);
}

double Node::get_marine_landscape_norm() const
{
    return(marine_landscape_norm);
}

double Node::get_marine_landscape_alpha() const
{
    return(marine_landscape_alpha);
}

double Node::get_wind() const
{
    return(wind);
}

double Node::get_wind_norm() const
{
    return(wind_norm);
}

double Node::get_wind_alpha() const
{
    return(wind_alpha);
}

double Node::get_sst() const
{
    return(sst);
}

double Node::get_sst_norm() const
{
    return(sst_norm);
}

double Node::get_sst_alpha() const
{
    return(sst_alpha);
}

double Node::get_salinity() const
{
    return(salinity);
}

double Node::get_salinity_norm() const
{
    return(salinity_norm);
}

double Node::get_salinity_alpha() const
{
    return(salinity_alpha);
}

double Node::get_Nitrogen() const
{
    return(Nitrogen);
}

double Node::get_Nitrogen_norm() const
{
    return(Nitrogen_norm);
}

double Node::get_Nitrogen_alpha() const
{
    return(Nitrogen_alpha);
}

double Node::get_Phosphorus() const
{
    return(Phosphorus);
}

double Node::get_Phosphorus_norm() const
{
    return(Phosphorus_norm);
}

double Node::get_Phosphorus_alpha() const
{
    return(Phosphorus_alpha);
}

double Node::get_Oxygen() const
{
    return(Oxygen);
}

double Node::get_Oxygen_norm() const
{
    return(Oxygen_norm);
}

double Node::get_Oxygen_alpha() const
{
    return(Oxygen_alpha);
}

double Node::get_DissolvedCarbon() const
{
    return(DissolvedCarbon);
}

double Node::get_DissolvedCarbon_norm() const
{
    return(DissolvedCarbon_norm);
}

double Node::get_DissolvedCarbon_alpha() const
{
    return(DissolvedCarbon_alpha);
}

double Node::get_bathymetry() const
{
    return(bathymetry);
}

double Node::get_shippingdensity() const
{
    return(shippingdensity);
}

double Node::get_siltfraction() const
{
    return(siltfraction);
}

double Node::get_icesrectanglecode() const
{
    return(icesrectanglecode);
}


double Node::get_init_benthos_biomass() const
{
    return(benthos_biomass);
}

double Node::get_init_benthos_number() const
{
    return(benthos_number);
}

double Node::get_init_benthos_meanweight() const
{
    return(benthos_meanweight);
}

vector<double> Node::get_benthos_biomass_per_funcgr() const
{
    return(benthos_tot_biomass);
}

vector<double> Node::get_benthos_number_per_funcgr() const
{
    return(benthos_tot_number);
}

vector<double> Node::get_benthos_meanweight_per_funcgr() const
{
    return(benthos_tot_meanweight);
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

const vector<types::NodeId> &Node::get_usual_fgrounds() const
{
    return mUsualFGrounds;
}

const vector<double> &Node::get_freq_usual_fgrounds() const
{
    return mFreqUsualFGrounds;
}

void Node::set_usual_fgrounds(const vector<types::NodeId> &_usual_fgrounds)
{
    UNUSED(_usual_fgrounds);
//    mUsualFGrounds = _usual_fgrounds;
}

void Node::set_freq_usual_fgrounds(const vector <double> &_freq_usual_fgrounds)
{
    UNUSED(_freq_usual_fgrounds);
//    mFreqUsualFGrounds = _freq_usual_fgrounds;
}

vector<types::NodeId> Node::get_usual_fgrounds_this_met(int met)
{
    UNUSED(met);
    return mUsualFGroundsMet;
}


vector<double> Node::get_freq_usual_fgrounds_this_met(int met)
{
    UNUSED(met);
    return mFreqUsualFGroundsMet;
}

void Node::set_usual_fgrounds_per_met(multimap<int, types::NodeId> _usual_fgrounds_per_met)
{
    UNUSED(_usual_fgrounds_per_met);
}


void Node::set_freq_usual_fgrounds_per_met(multimap<int, double> _freq_usual_fgrounds_per_met)
{
    UNUSED(_freq_usual_fgrounds_per_met);
}


const multimap<types::NodeId,int> &Node::get_usual_metiers() const
{
     return mUsualMetiers;
}


const multimap<int, double> &Node::get_freq_usual_metiers() const
{
    return mFreqUsualMetiers;
}

void Node::set_usual_metiers(multimap<types::NodeId, int> _usual_metiers)
{
    UNUSED(_usual_metiers);
}


void Node::set_freq_usual_metiers(multimap<int, double> _freq_usual_metiers)
{
    UNUSED(_freq_usual_metiers);
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

std::vector<double>& Node::get_Ns_pops_at_szgroup(int name_pop)
{
    return Ns_pops_at_szgroup[name_pop];
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


vector<double> Node::get_last_oth_catch_pops_at_szgroup(int name_pop) const
{

    vector<double> a_last_oth_catch_pops_at_szgroup;

    for(unsigned int j = 0; j < last_oth_catch_pops_at_szgroup[name_pop].size(); j++)
    {
        a_last_oth_catch_pops_at_szgroup.push_back(last_oth_catch_pops_at_szgroup[name_pop] [j]);
    }

    return(a_last_oth_catch_pops_at_szgroup);
}

vector<double> Node::get_last_oth_disc_pops_at_szgroup(int name_pop) const
{

    vector<double> a_last_oth_disc_pops_at_szgroup;

    for(unsigned int j = 0; j < last_oth_disc_pops_at_szgroup[name_pop].size(); j++)
    {
        a_last_oth_disc_pops_at_szgroup.push_back(last_oth_disc_pops_at_szgroup[name_pop] [j]);
    }

    return(a_last_oth_disc_pops_at_szgroup);
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


double  Node::get_benthos_tot_number(int funcgr) const
{

    return(benthos_tot_number.at(funcgr));
}


const vector <double> &Node::get_benthos_tot_number() const
{

    return(benthos_tot_number);
}

double  Node::get_benthos_tot_meanweight(int funcgr) const
{

    return(benthos_tot_meanweight.at(funcgr));
}


const vector <double> &Node::get_benthos_tot_meanweight() const
{

    return(benthos_tot_meanweight);
}

double  Node::get_benthos_tot_biomass_K(int funcgr) const
{

    return(benthos_tot_biomass_K.at(funcgr));
}


const vector <double> &Node::get_benthos_tot_biomass_K() const
{

    return(benthos_tot_biomass_K);
}

double  Node::get_benthos_tot_number_K(int funcgr) const
{

    return(benthos_tot_number_K.at(funcgr));
}


const vector <double> &Node::get_benthos_tot_number_K() const
{

    return(benthos_tot_number_K);
}


double  Node::get_tariffs(int type) const
{

    return(tariffs.at(type));
}


const vector <double> &Node::get_tariffs() const
{

    return(tariffs);
}

const vector<vector<double> > &Node::get_Ns_pops_at_szgroup() const
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

double Node::get_cumsubsurfacesweptarea() const
{
    return(cumsubsurfacesweptarea);
}

double Node::get_cumcatches() const
{
    return(cumcatches);
}

double Node::get_cumcatches_with_threshold() const
{
    return(cumcatches_with_threshold);
}

double Node::get_cumdiscards() const
{
    return(cumdiscards);
}

double Node::get_cumdiscardsratio() const
{
    return(cumdiscardsratio);
}

double Node::get_nbchoked() const
{
    return(nbchoked);
}

vector<int> Node::get_pop_names_on_node ()
{
	return(pop_names_on_node);
}

vector<int> Node::get_ff_names_on_node ()
{
    return(ff_names_on_node);
}

const vector<double>& Node::get_totNs_per_pop ()
{
    return(totNs_per_pop);
}

const vector<double>& Node::get_totWs_per_pop ()
{
    return(totWs_per_pop);
}

const vector<double>& Node::get_impact_per_pop ()
{
    return(impact_per_pop);
}

const vector<double>& Node::get_cumcatches_per_pop ()
{
    return(cumcatches_per_pop);
}

const vector<double>& Node::get_cumcatches_per_pop_this_month()
{
    return(cumcatches_per_pop_this_month);
}

const vector<vector<double> >& Node::get_cumcatches_per_pop_per_met_this_month()
{
    return(cumcatches_per_pop_per_met_this_month);
}

const vector<vector<double> >& Node::get_cumeffort_per_pop_per_met_this_month()
{
    return(cumeffort_per_pop_per_met_this_month);
}

const vector<vector<double> >& Node::get_cpue_per_pop_per_met_this_month()
{
    return(cpue_per_pop_per_met_this_month);
}

const vector<double>& Node::get_cumdiscards_per_pop ()
{
    return(cumdiscards_per_pop);
}

void Node::set_vid (int val)
{
    std::lock_guard<std::mutex> guard(mutex);
    vid.push_back(val);
}

void Node::set_cumftime(int tot)
{
    cumftime = tot;
}

void Node::set_cumsweptarea(double tot)
{
    cumsweptarea = tot;
}

void Node::set_cumsubsurfacesweptarea(double tot)
{
    cumsubsurfacesweptarea = tot;
}


void Node::set_cumcatches(double tot)
{
    cumcatches = tot;
}

void Node::set_cumcatches_with_threshold(double tot)
{
    cumcatches_with_threshold = tot;
}

void Node::set_cumdiscards(double tot)
{
   cumdiscards = tot;
}

void Node::set_cumdiscardsratio(double rat)
{
   cumdiscardsratio = rat;
}

void Node::set_nbchoked(double val)
{
   nbchoked = val;
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

void Node::add_to_cumsubsurfacesweptarea(double subsurfacesweptarea)
{
    lock();
    cumsubsurfacesweptarea+=subsurfacesweptarea;
    unlock();
}

void Node::add_to_cumcatches(double catches)
{
    lock();
    cumcatches+=catches;
    unlock();
}

void Node::add_to_cumdiscards(double discards)
{
    lock();
    cumdiscards+=discards;
    unlock();
}


void Node::add_to_cumcatches_per_pop(double catches, int pop)
{
    lock();
    cumcatches_per_pop.at(pop)+=catches;
    unlock();
}

void Node::add_to_cumcatches_per_pop_this_month(double catches, int pop)
{
    lock();
    cumcatches_per_pop_this_month.at(pop) += catches;
    unlock();
}

void Node::add_to_cumcatches_per_pop_per_met_this_month(double catches, int pop, int met)
{
    lock();
    cumcatches_per_pop_per_met_this_month.at(pop).at(met) += catches;
    unlock();
}

void Node::add_to_cumeffort_per_pop_per_met_this_month(double effort, int pop, int met)
{
    lock();
    cumeffort_per_pop_per_met_this_month.at(pop).at(met) += effort;
    unlock();
}

void Node::compute_cpue_per_pop_per_met_this_month(int pop, int met)
{
    lock();
    // running average
    cpue_per_pop_per_met_this_month.at(pop).at(met) = 
             cumcatches_per_pop_per_met_this_month.at(pop).at(met) / cumeffort_per_pop_per_met_this_month.at(pop).at(met);
    unlock();
}

void Node::add_to_cumdiscards_per_pop(double discards, int pop)
{
    lock();
    cumdiscards_per_pop.at(pop)+=discards;
    unlock();
}


void Node::reinit(vector<double> &vec, unsigned int sz)
{
    if (vec.size() != sz)
        vec.resize(sz, 0);
    for (unsigned int i = 0; i < sz; ++i)
        vec[i] = 0.0;
}

void Node::reinit(vector<vector<double> > &vec, unsigned int sz, unsigned int subsz)
{
    if (vec.size() != sz)
        vec.resize(sz);
    for (unsigned int i = 0; i < sz; ++i)
        reinit(vec[i], subsz);
}

void Node::init_Ns_pops_at_szgroup(int nbpops, int nbszgroups, int nbmets)
{
    reinit (Ns_pops_at_szgroup, nbpops, nbszgroups);
    reinit (Ns_pops_at_szgroup_at_month_start, nbpops, nbszgroups);
    reinit (removals_pops_at_szgroup, nbpops, nbszgroups);
    reinit (last_oth_catch_pops_at_szgroup, nbpops, nbszgroups);
    reinit (last_oth_disc_pops_at_szgroup, nbpops, nbszgroups);
    reinit (pressure_pops_at_szgroup, nbpops, nbszgroups);
    reinit (totNs_per_pop, nbpops);
    reinit (totWs_per_pop, nbpops);
    reinit (impact_per_pop, nbpops);
    reinit (cumcatches_per_pop, nbpops);
    reinit (cumcatches_per_pop_this_month, nbpops);
    reinit(cumcatches_per_pop_per_met_this_month, nbpops, nbmets);
    reinit(cumeffort_per_pop_per_met_this_month, nbpops, nbmets);
    reinit(cpue_per_pop_per_met_this_month, nbpops, nbmets);
    reinit (cumdiscards_per_pop, nbpops);

}


void Node::init_avai_pops_at_selected_szgroup(int nbpops, int selected_nbszgroups)
{
    reinit(avai_pops_at_selected_szgroup, nbpops, selected_nbszgroups);
#if 0
	// init at 0 the matrix of Ns
    dout(cout  << "init matrix of Ns" << "\n");
	vector< vector<double> > init_avai_pops_at_selected_szgroup(nbpops, vector<double>(selected_nbszgroups));
	avai_pops_at_selected_szgroup= init_avai_pops_at_selected_szgroup;
    for(int i = 0; i < nbpops; i++)
	{
        for(int j = 0; j < selected_nbszgroups; j++)
		{
			avai_pops_at_selected_szgroup.at(i).at(j) = 0;
            dout(cout  << avai_pops_at_selected_szgroup[i][j] << " ");
		}
        dout(cout  << "\n");
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

void Node::set_last_oth_catch_pops_at_szgroup(int name_pop, const vector<double>& newval)
{
    for(unsigned int j = 0; j < last_oth_catch_pops_at_szgroup[name_pop].size(); j++)
    {
        last_oth_catch_pops_at_szgroup[name_pop][j]= newval[j];
    }

}

void Node::set_last_oth_disc_pops_at_szgroup(int name_pop, const vector<double>& newval)
{
    for(unsigned int j = 0; j < last_oth_disc_pops_at_szgroup[name_pop].size(); j++)
    {
        last_oth_disc_pops_at_szgroup[name_pop][j]= newval[j];
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

void Node::set_totNs_per_pop(int name_pop, double newval)
{

    totNs_per_pop.at(name_pop)=newval;

}

void Node::set_totWs_per_pop(int name_pop, double newval)
{

    totWs_per_pop.at(name_pop)=newval;

}

void Node::set_impact_per_pop(int name_pop, double newval)
{

    impact_per_pop.at(name_pop)=newval;

}

void Node::set_cumcatches_per_pop(int name_pop, double newval)
{

    cumcatches_per_pop.at(name_pop)=newval;

}

void Node::set_cumcatches_per_pop_this_month(int name_pop, double newval)
{

    cumcatches_per_pop_this_month.at(name_pop) = newval;

}

void Node::set_cumcatches_per_pop_per_met_this_month(int name_pop, int name_met, double newval)
{

    cumcatches_per_pop_per_met_this_month.at(name_pop).at(name_met) = newval;

}

void Node::set_cumeffort_per_pop_per_met_this_month(int name_pop, int name_met, double newval)
{

    cumeffort_per_pop_per_met_this_month.at(name_pop).at(name_met) = newval;

}

void Node::set_cpue_per_pop_per_met_this_month(int name_pop, int name_met, double newval)
{

    cpue_per_pop_per_met_this_month.at(name_pop).at(name_met) = newval;

}

void Node::set_cumdiscards_per_pop(int name_pop, double newval)
{

    cumdiscards_per_pop.at(name_pop)=newval;

}


void Node::set_pop_names_on_node(int name_pop)
{
    if (std::find(pop_names_on_node.begin(), pop_names_on_node.end(), name_pop) == pop_names_on_node.end()) {
        pop_names_on_node.push_back(name_pop);

    }
}

void Node::set_ff_names_on_node(int name_ff)
{
    ff_names_on_node.push_back(name_ff);
}

void Node::clear_pop_names_on_node()
{
	pop_names_on_node.clear();
}


void Node::remove_pop_names_on_node(int name_pop)
{
    pop_names_on_node.erase(std::remove(pop_names_on_node.begin(), pop_names_on_node.end(), name_pop), pop_names_on_node.end());
}


void  Node::set_benthos_tot_biomass(int funcgr, double value)
{

	benthos_tot_biomass.at(funcgr)= value;
}

void  Node::set_benthos_tot_number(int funcgr, double value)
{

    benthos_tot_number.at(funcgr)= value;
}

void  Node::set_benthos_tot_meanweight(int funcgr, double value)
{

    benthos_tot_meanweight.at(funcgr)= value;
}

void  Node::set_benthos_tot_biomass_K(int funcgr, double value)
{

    benthos_tot_biomass_K.at(funcgr)= value;
}

void  Node::set_benthos_tot_number_K(int funcgr, double value)
{

    benthos_tot_number_K.at(funcgr)= value;
}

void Node::set_benthos_biomass_per_funcgr(vector<double> _benthos_tot_biomass)
{
    benthos_tot_biomass=_benthos_tot_biomass;
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
    dout(cout  << "clear pop on nodes..." << "\n");
	for(unsigned int i = 0; i < Ns_pops_at_szgroup.size(); i++)
	{
		for(unsigned int j = 0; j < Ns_pops_at_szgroup[i].size(); j++)
		{
			Ns_pops_at_szgroup[i][j] = 0;
		}
	}
}


void Node::clear_removals_pops_at_szgroup(int namepop)
{
    dout(cout  << "clear removals on nodes..." << "\n");
        for(unsigned int sz = 0; sz < removals_pops_at_szgroup[namepop].size(); sz++)
		{
            removals_pops_at_szgroup[namepop][sz] = 0;
		}
}


void Node::clear_last_oth_catch_pops_at_szgroup(int namepop)
{
    dout(cout  << "clear oth catch on nodes..." << "\n");
        for(unsigned int sz = 0; sz < last_oth_catch_pops_at_szgroup[namepop].size(); sz++)
        {
            last_oth_catch_pops_at_szgroup[namepop][sz] = 0;
        }
}

void Node::clear_last_oth_disc_pops_at_szgroup(int namepop)
{
    dout(cout  << "clear oth disc on nodes..." << "\n");
        for(unsigned int sz = 0; sz < last_oth_disc_pops_at_szgroup[namepop].size(); sz++)
        {
            last_oth_disc_pops_at_szgroup[namepop][sz] = 0;
        }
}

void Node::clear_totNs_per_pop()
{
    for(unsigned int i=0; i<totNs_per_pop.size(); i++)
    {
        totNs_per_pop.at(i)=0;
    }

}

void Node::clear_totWs_per_pop()
{
    for(unsigned int i=0; i<totWs_per_pop.size(); i++)
    {
        totWs_per_pop.at(i)=0;
    }

}

void Node::clear_impact_per_pop()
{
    for(unsigned int i=0; i<impact_per_pop.size(); i++)
	{
        impact_per_pop.at(i)=0;
	}

}

void Node::clear_cumcatches_per_pop()
{
    for(unsigned int i=0; i<cumcatches_per_pop.size(); i++)
    {
        cumcatches_per_pop.at(i)=0;
    }

}

void Node::clear_cumcatches_per_pop_this_month()
{
    for (unsigned int i = 0; i < cumcatches_per_pop_this_month.size(); i++)
    {
        cumcatches_per_pop_this_month.at(i) = 0;
    }

}

void Node::clear_cumcatches_per_pop_per_met_this_month()
{
    for (unsigned int i = 0; i < cumcatches_per_pop_per_met_this_month.size(); i++)
    {
        for (unsigned int j = 0; j < cumcatches_per_pop_per_met_this_month.at(i).size(); j++)
        {
            cumcatches_per_pop_per_met_this_month.at(i).at(j) = 0;
        }
    }

}

void Node::clear_cumeffort_per_pop_per_met_this_month()
{
    for (unsigned int i = 0; i < cumeffort_per_pop_per_met_this_month.size(); i++)
    {
        for (unsigned int j = 0; j < cumeffort_per_pop_per_met_this_month.at(i).size(); j++)
        {
            cumeffort_per_pop_per_met_this_month.at(i).at(j) = 0;
        }
    }

}

void Node::clear_cpue_per_pop_per_met_this_month()
{
    for (unsigned int i = 0; i < cpue_per_pop_per_met_this_month.size(); i++)
    {
        for (unsigned int j = 0; j < cpue_per_pop_per_met_this_month.at(i).size(); j++)
        {
            cpue_per_pop_per_met_this_month.at(i).at(j) = 0;
        }
    }

}

void Node::clear_cumdiscards_per_pop()
{
    for(unsigned int i=0; i<cumdiscards_per_pop.size(); i++)
    {
        cumdiscards_per_pop.at(i)=0;
    }

}

void Node::clear_avai_pops_at_selected_szgroup()
{
    dout(cout  << "clear pop on nodes..." << "\n");
	for(unsigned int i = 0; i < avai_pops_at_selected_szgroup.size(); i++)
	{
		for(unsigned int j = 0; j < avai_pops_at_selected_szgroup[i].size(); j++)
		{
			avai_pops_at_selected_szgroup[i][j] = 0;
		}
	}
}

void Node::apply_natural_mortality_at_node(
    int               name_pop,
    const std::vector<double>& M_at_szgroup,
    const std::vector<double>& prop_M_from_species_interactions,
    double            multiplier_on_M_background)
{
    // -----------------------------------------------------------------
    // 0??  Early?out if there is nothing to do (empty vectors)
    // -----------------------------------------------------------------
    if (M_at_szgroup.empty() || prop_M_from_species_interactions.empty())
        return;

    // -----------------------------------------------------------------
    // 1??  Grab a **reference** to the node’s N vector (no copy!)
    // -----------------------------------------------------------------
    std::vector<double>& Ns = get_Ns_pops_at_szgroup(name_pop);
    const std::size_t n_sz = Ns.size();

    // -----------------------------------------------------------------
    // 2??  Pre?compute the weighted background mortality for each size?group
    // -----------------------------------------------------------------
    //   M_bg_i = ?_spp (prop_M[spp] * M_at_szgroup[i]) * multiplier
    //   The inner sum does not depend on the population index, only on i.
    // -----------------------------------------------------------------
    std::vector<double> M_bg(n_sz, 0.0);
    const double mult = multiplier_on_M_background;

    // raw pointers for speed
    const double* M_ptr = M_at_szgroup.data();
    const double* prop_ptr = prop_M_from_species_interactions.data();
    double* bg_ptr = M_bg.data();

    for (std::size_t i = 0; i < n_sz; ++i) {
        double sum = 0.0;
        for (std::size_t s = 0; s < prop_M_from_species_interactions.size(); ++s) {
            double v = prop_ptr[s] * M_ptr[i];
            if (!std::isnan(v))          // keep the NaN?skip if required
                sum += v;
        }
        bg_ptr[i] = sum * mult;
    }

    // -----------------------------------------------------------------
    // 3??  Apply mortality to the N vector (in?place)
    // -----------------------------------------------------------------
    const double inv_month = 1.0 / 12.0;   // constant factor

    for (std::size_t i = 0; i < n_sz; ++i) {
        double mortality = bg_ptr[i];          // already includes species?interaction weighting
        double newN = Ns[i] * std::exp(-mortality * inv_month);
        Ns[i] = (newN < 0.0) ? 0.0 : newN;     // clamp negatives to zero
    }
}


/*
void Node::apply_natural_mortality_at_node(int name_pop, const vector<double>& M_at_szgroup, vector<double>& prop_M_from_species_interactions, double multiplier_on_M_background)
{
    //dout(cout  << "BEGIN: apply_natural_mortality_at_node()" << "\n");

	vector <double> a_Ns_at_szgroup = get_Ns_pops_at_szgroup(name_pop);
	for(unsigned int i=0; i<a_Ns_at_szgroup.size(); i++)
	{

        double M_at_szgroup_i_on_node=0;
        for (unsigned int spp=0; spp<prop_M_from_species_interactions.size(); spp++)
        {
            M_at_szgroup_i_on_node +=  prop_M_from_species_interactions.at(spp)*M_at_szgroup[i]* multiplier_on_M_background;
        }

        //cout  << "this pop " << name_pop << ", this sz " << i << "M_at_szgroup[i] is " << M_at_szgroup[i] << " and a_Ns_at_szgroup[i] is" << a_Ns_at_szgroup[i] << " and  M_at_szgroup_i_on_node is " << M_at_szgroup_i_on_node << "\n";

        // divide according to tstep (month in this case)
        a_Ns_at_szgroup[i] =  a_Ns_at_szgroup[i]  *exp(-M_at_szgroup_i_on_node/12);
		//this is assuming that the M is uniformly applied to the pop
		// e.g. 1000*exp(-0.2) = 225*exp(-0.2)+ 775*exp(-0.2)
		// (the pble with spatial scale is that we cannot do e.g. 225*exp(-0.1)+ 775*exp(-0.3) because = 1000*exp(-x) and need to solve for x)
    
        if (a_Ns_at_szgroup[i] < 0)
        {
            cout << "Negative Ns detected in apply_natural_mortality()! ...set to 0!" << "\n";
            a_Ns_at_szgroup[i] = 0;
        }
    }

	set_Ns_pops_at_szgroup(name_pop, a_Ns_at_szgroup);

    //dout(cout  << "END: apply_natural_mortality_at_node()" << "\n");
}
*/


void Node::apply_natural_mortality_at_node_from_size_spectra_approach(int name_pop,
                                                                      int tstep,
                                                                      const vector<vector<double> > & Ws_at_szgroup,
                                                                      const vector<vector<vector<vector<double> > > > & predKernel,
                                                                      const vector<vector<double> > & searchVolMat,
                                                                      const vector<vector<double> > & juveniles_diet_preference,
                                                                      const vector<vector<double> > & adults_diet_preference,
                                                                      const vector<int> & mat_cats,
                                                                      const vector<double> & M_background_this_pop,
                                                                      double multiplier_on_M_background,
                                                                      double a_scaling)
{
    //dout(cout  << "BEGIN: apply_natural_mortality_at_node()" << "\n");

    vector<int> spp_on_this_node = this->get_pop_names_on_node();


    // check
    //if(this->get_idx_node().toIndex()==40)
    //{
    //    cout << "spp_on_this_node is" << "\n";
    //    for (int i=0; i<spp_on_this_node.size();++i) cout << spp_on_this_node.at(i) << " ";
    //    cout  << "\n";
    //}

    vector <double> Np = get_Ns_pops_at_szgroup(name_pop); // the prey


    vector<double> M2_on_node(Np.size(), 0.0);
    vector<double> dwpred(Np.size(), 0.0);

    
    
    // Background mortality from Andersen et al.
    // from surv<-round(exp(-(0.12*27*(l+(size_bin_cm/2))^(-1))),4)  # length dependent mortality vector using the lower bound length (+1 to ignore 0) to get survival
    // mort<-round((1-surv),4)
    //or Audric:  values corresponds to (mu_0 Winf ^ (n-1))


    vector<vector<double> >  predRate(spp_on_this_node.size(), vector<double>(NBSZGROUP));

    //if (tstep > 9500)     cout << "on node " << this->get_idx_node().toIndex() << "\n";

    for (unsigned int j=0; j<spp_on_this_node.size(); j++) // loop over predator
        {
            vector <double> Npred = get_Ns_pops_at_szgroup(spp_on_this_node.at(j));
            vector <double> Wpred = Ws_at_szgroup.at(spp_on_this_node.at(j));

           //      if (tstep > 9500)     cout << "loop over predator pop" << spp_on_this_node.at(j) << ",  j is " << j  << "\n";
            for (unsigned int kprey=0; kprey<NBSZGROUP; kprey++)  // loop over prey sizes
            {

                //if (tstep > 9500)     cout << "for prey pop" << name_pop << ", loop over prey size kprey is " << kprey << "and dwpred.size is " << 
                //                                   dwpred.size()  << " and Wpred.size is " << Wpred.size()  <<  "\n";

                dwpred.at(0) = (Wpred[0]-0)/2;
                        for (unsigned int sz=1; sz<Wpred.size(); sz++) dwpred.at(sz) = (Wpred.at(sz) + Wpred.at(sz-1))/2;
                        for (unsigned int sz=(Wpred.size()-1); sz>1; sz--) dwpred.at(sz) = dwpred.at(sz) - dwpred.at(sz-1);

                        //if (tstep > 9500)     cout << "for prey pop" << name_pop << ", loop over prey size kprey is " << kprey << "....OK!" << "\n";

                        //check
                        //for (unsigned int sz=0; sz<Wpred.size(); sz++){
                        //    if(this->get_idx_node().toIndex()==40) cout << "on node" <<  this->get_idx_node().toIndex() << " on pop" << name_pop << " sz " << sz << " dwpred[sz] " << dwpred[sz]   << "\n";
                        //}


                        //if (tstep > 9500)  cout << "mat_cat.size() is " << mat_cats.size() << "while j is " << j <<  "\n";

                        // juveniles
                        for (unsigned int k=0; k<mat_cats.at(j); k++)  // loop over PREDATOR sizes
                        {
                            //if (tstep > 9500)   cout << "loop over predator JUV size is " << k << "\n";


                                if(juveniles_diet_preference.at(spp_on_this_node.at(j)).at(name_pop)>0) predRate.at(j).at(kprey)  = predRate.at(j).at(kprey) +
                                                   predKernel.at(spp_on_this_node.at(j)).at(kprey).at(k).at(name_pop)* juveniles_diet_preference.at(spp_on_this_node.at(j)).at(name_pop) *
                                                       (1- 0.6)* searchVolMat.at(spp_on_this_node.at(j)).at(k) * 1* Npred.at(k)*dwpred.at(k);

                                //cout << "Preference of this predator " << j << " on prey " << kprey <<" is "<< diet_preference.at(j).at(kprey) << "\n";
                                //if (tstep > 9500)       cout << "loop over predator JUV size is ok" << "\n";
                        }

                        // adults
                        for (unsigned int k=mat_cats.at(j); k<NBSZGROUP; k++)  // loop over PREDATOR sizes
                        {
                            //if (tstep > 9500)         cout << "loop over predator ADULT size is " << k << "\n";
                            
                            if(adults_diet_preference.at(spp_on_this_node.at(j)).at(name_pop)>0) predRate.at(j).at(kprey)  = predRate.at(j).at(kprey) +
                                                   predKernel.at(spp_on_this_node.at(j)).at(kprey).at(k).at(name_pop)* adults_diet_preference.at(j).at(name_pop) *
                                                       (1- 0.6)* searchVolMat.at(spp_on_this_node.at(j)).at(k) * 1* Npred.at(k)*dwpred.at(k);

                                //cout << "Preference of this predator " << j << " on prey " << kprey <<" is "<< diet_preference.at(j).at(kprey) << "\n";

                           // assuming feeding level at 0.6
                           // assuming interactionMatrixThetas[prey,j] at 1 because we know the two stocks are overlapping

                            //if (tstep > 9500)     cout << "loop over predator ADULT size is ok " << "\n";
                        }

                        //if (tstep > 9500)   cout << "loop over prey size kprey is ok" << "\n";

            }

            // check
            //if(this->get_idx_node().toIndex()==40)
            //{
            //    cout << "spp_on_this_node is" << "\n";
            //    for (int i=0; i<spp_on_this_node.size();++i) cout << "on node" <<  this->get_idx_node().toIndex() << " on pop" << name_pop <<", predRate.at("<<j<<").at("<<i<<")  is "<< predRate.at(j).at(i)  << "\n";
            //    cout  << "\n";
            //}
            //if (tstep > 9500)      cout << "loop over predator j is ok " << "\n";

      }


    // so...getting the M2 mortality per size group
        for (unsigned int kprey=0; kprey<NBSZGROUP; kprey++)  // loop over prey sizes
        {
           for (unsigned int j=0; j<spp_on_this_node.size(); j++) // loop over predator
                {
                     M2_on_node.at(kprey) =  M2_on_node.at(kprey) + predRate.at(j).at(kprey);
                }
        }


       

        for(unsigned int sz=0; sz<Np.size(); sz++)
        {
           // divide according to tstep (month in this case)
           //if(this->get_idx_node().toIndex()==40) cout << "on node" << this->get_idx_node() << " and sz " << sz << ", M2_on_node.at(sz) is "<< M2_on_node.at(sz) << "\n";

            
            Np.at(sz) = Np.at(sz)  *exp(-((M2_on_node.at(sz)*a_scaling)+M_background_this_pop.at(sz)*multiplier_on_M_background)/12);

            if (Np[sz] < 0)
            {
                cout << "Negative Ns detected in apply_natural_mortality() version size spectra! ...set to 0!" << "\n";
                Np[sz] = 0;
            }

            // check for the need for a_scaling factor. The depletion from M on node should be kept in a realistic range.
            if (name_pop == 2 && exp(-((M2_on_node.at(sz) * a_scaling) + M_background_this_pop.at(sz) * multiplier_on_M_background) / 12)<0.94) {
                cout << "check her pop 2" << "\n";
            cout << "pop" << name_pop << "a_scaling " << sz << " is " << a_scaling << "\n";
            cout << "pop" << name_pop << " M2_on_node.at(sz) * a_scaling for sz "<< sz <<" is "<< M2_on_node.at(sz) * a_scaling << "\n";
            cout << "pop" << name_pop << " M_background_this_pop.at(sz)*multiplier_on_M_background for sz " << sz << " is " << M_background_this_pop.at(sz) * multiplier_on_M_background << "\n";
            cout << "pop" << name_pop << " exp(-((M2_on_node.at(sz)*a_scaling)+M_background_this_pop.at(sz)*multiplier_on_M_background)/12)  is " << exp(-((M2_on_node.at(sz) * a_scaling) + M_background_this_pop.at(sz) * multiplier_on_M_background) / 12) << "\n";
            cout << "\n";
            }


            //this is assuming that the M is uniformly applied to the pop
           // e.g. 1000*exp(-0.2) = 225*exp(-0.2)+ 775*exp(-0.2)
           // (the pblm with spatial scale is that we cannot do e.g. 225*exp(-0.1)+ 775*exp(-0.3) because = 1000*exp(-x) and need to solve for x)
        }

   
     set_Ns_pops_at_szgroup(name_pop, Np);

    //dout(cout  << "END: apply_natural_mortality_at_node()" << "\n");
}



void Node::apply_oth_land(int name_pop, int MLS_cat, double &oth_land_this_pop_this_node,
                          const vector<double>&  weight_at_szgroup, const vector<double>& totN,
                          int will_I_discard_all,  vector<double>& selectivity_per_stock_ogives_for_oth_land)
{
    dout(cout << "BEGIN: apply_oth_land()" << "\n");


    // DISAGREGATE TOTAL CATCH (FROM OTHERS) IN WEIGHT INTO SZGROUP
    // AND CONVERT INTO REMOVALS IN NUMBERS
    // NOTICE THAT THIS IS THE SAME PROCEDURE THAN THE ONE IN do.catch()
    vector <double> Ns_at_szgroup_pop = this->get_Ns_pops_at_szgroup(name_pop);
                                 // ...input to be compared with output!
    vector <double>new_Ns_at_szgroup_pop = Ns_at_szgroup_pop;

                                 // init
    vector <double>avail_biomass (Ns_at_szgroup_pop.size(), 0.0);
    vector <double> avail_biomass_to_landings (Ns_at_szgroup_pop.size(), 0.0);
    vector <double> avail_biomass_to_discards  (Ns_at_szgroup_pop.size(), 0.0);
                                 // init
    vector <double>alloc_key1(Ns_at_szgroup_pop.size(), 0.0);
    vector <double>alloc_key2(Ns_at_szgroup_pop.size(), 0.0);
                                 // init
    vector <double>catch_per_szgroup(Ns_at_szgroup_pop.size(), 0.0);
    vector <double>land_per_szgroup(Ns_at_szgroup_pop.size(), 0.0);
    vector <double>disc_per_szgroup(Ns_at_szgroup_pop.size(), 0.0);
                                 // init
    vector <double>removals_per_szgroup(Ns_at_szgroup_pop.size(), 0.0);
                                 // init
    vector <double>cumul_removals_at_szgroup_pop=this->get_removals_pops_at_szgroup(name_pop);
                                 // init
    vector <double>new_removals_at_szgroup_pop(Ns_at_szgroup_pop.size(), 0.0);

    double tot            = 0;	 // init



    vector<double> sel_ogive;
    if(selectivity_per_stock_ogives_for_oth_land.empty())
    {
        double gadoid_sel_ogive [ ] =
        {
            0.00000225976, 0.00001410136387, 0.00008798955470,
            0.000548823892218543, 0.00341497770850069, 0.0209356687119669, 0.117728102864771,
            0.45435195117619,0.838609522203591,0.970082540287596,0.995082177119589,0.999208651051219,0.999873102040564,1
        };							 // same as for OTB_DEF
        vector<double> gad_sel_ogive (gadoid_sel_ogive, gadoid_sel_ogive + sizeof(gadoid_sel_ogive) / sizeof(double) );
        sel_ogive=gad_sel_ogive; // choose the hardcoded...

    }
    else
    {
            sel_ogive=selectivity_per_stock_ogives_for_oth_land;  // ...or read from param
    }


//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "OTH_LAND the sel_ogive in use is " << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) for (int i=0; i <sel_ogive.size();++i) cout << sel_ogive.at(i) << " " ;
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "\n";


    // compute available biomass via selectivity
    double tot_avai_for_land=0.0;
    double tot_avai_for_disc=0.0;
    for(unsigned int szgroup=0; szgroup <avail_biomass.size(); szgroup++)
    {
        avail_biomass[szgroup]=  Ns_at_szgroup_pop[szgroup]*weight_at_szgroup[szgroup]*sel_ogive[szgroup];
        if(szgroup >=MLS_cat) {
            tot_avai_for_land+= avail_biomass[szgroup];
            avail_biomass_to_landings[szgroup] =avail_biomass[szgroup];
            avail_biomass_to_discards[szgroup]=0.0;
        } else{
             tot_avai_for_disc+= avail_biomass[szgroup];
             avail_biomass_to_discards[szgroup]=avail_biomass[szgroup];
             avail_biomass_to_landings[szgroup] =0.0;
        }
        tot = tot+avail_biomass[szgroup];   // cumul
        dout(cout  << "avail_biomass[szgroup] " <<avail_biomass[szgroup] << "\n");



    }
    dout(cout  << "tot biomass available" << tot << "\n");
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout  << "tot biomass available" << tot << "\n";

    if(tot>1.0)
    {


        // compute the landings vs. discard part
        // in function of MLS and gear selectivity ogive and the N
        vector<double> Ns_at_szgroup_pop_scaled = Ns_at_szgroup_pop; // init
        for(int sizgroup=0; sizgroup<(int)Ns_at_szgroup_pop.size(); sizgroup++)
        {
            Ns_at_szgroup_pop_scaled.at(sizgroup)=Ns_at_szgroup_pop_scaled.at(sizgroup)/
                    *(max_element(Ns_at_szgroup_pop.begin(), Ns_at_szgroup_pop.end()));
        }
        int inter=0;
        int a_szgroup=0;
        while(a_szgroup<=MLS_cat){
            inter = a_szgroup;
            a_szgroup+=1;
        }
        dout(if(inter>Ns_at_szgroup_pop_scaled.size()) cout<< "MLS categories cannot be > 13" << "\n";)
        double left_to_MLS=0;
        double right_to_MLS=0;
        if(sel_ogive.at(inter)>Ns_at_szgroup_pop_scaled.at(inter)){
            left_to_MLS  = trapezoidal(0, inter, sel_ogive) + trapezoidal(inter, MLS_cat, Ns_at_szgroup_pop_scaled); // discards
            right_to_MLS = trapezoidal(MLS_cat, NBSZGROUP-1, Ns_at_szgroup_pop_scaled); // landings
        } else{
            left_to_MLS  = trapezoidal(0, MLS_cat, sel_ogive); // discards
            right_to_MLS = trapezoidal(MLS_cat, inter, sel_ogive)+trapezoidal(inter, NBSZGROUP-1, Ns_at_szgroup_pop_scaled); // landings

        }

        double discardfactor;
        discardfactor = left_to_MLS / right_to_MLS; // (dis/lan)
        if (isinf(discardfactor)) discardfactor = 0.05; // assign a default value if inf
        //cout << " here again, in oth_land, discardfactor this pop " << this->get_name() << " is " << discardfactor << "\n";

        //  discardfactor = dis/lan != discard rate...btw, converting a discard rate into discardratio is disc/land=x/(1-x) with x=disc/(disc+land)
        //discardfactor = min( discardratio_limits[pop] , discardfactor); // metier and pop specific limit
        discardfactor = min( 0.05, discardfactor); // HARDCODED THRESHOLD (if 0.1, it means discards is 9% of the catch i.e. land+disc)
        // => caution: discard factor bounded to not exceed a value, otherwise high unrealistic discards will be produced when no adult left on zones
        double tot_landings_this_pop=oth_land_this_pop_this_node;
        double tot_discards_this_pop=oth_land_this_pop_this_node*discardfactor ;
        //cout << "in oth_land, tot_discards_this_pop  " << this->get_name() << " is " << tot_discards_this_pop << "\n";

//if(name_pop==2 && this->get_idx_node().toIndex()==19290)  cout  << "oth_land_this_pop_this_node in kg is " << oth_land_this_pop_this_node << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)  cout  << "discardfactor" << discardfactor << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)  cout  << "tot_landings_this_pop in kg is " << tot_landings_this_pop << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)  cout  << "tot_discards_this_pop in kg is " << tot_discards_this_pop << "\n";


        // init
        vector <double> new_avai_pops_at_selected_szgroup;

        for(unsigned int szgroup=0; szgroup <avail_biomass.size(); szgroup++)
        {
            if(avail_biomass[szgroup] >1.0)
            {
                new_avai_pops_at_selected_szgroup = this->get_avai_pops_at_selected_szgroup(name_pop);

                // compute alloc key
                // proportion
                if(szgroup>=MLS_cat){
                    alloc_key1[szgroup]=avail_biomass_to_landings[szgroup] /(tot_avai_for_land);
                    alloc_key2[szgroup]=0;
                    discardfactor=0; // no discard if >MLS
                } else{
                    alloc_key1[szgroup]=0;
                    alloc_key2[szgroup]=avail_biomass_to_discards[szgroup] /(tot_avai_for_disc);
                }

//if(name_pop==2 && this->get_idx_node().toIndex()==19290)   cout << "alloc_key1[" << szgroup<< "] is " << alloc_key1[szgroup] << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)   cout << "alloc_key2[" << szgroup<< "] is " << alloc_key2[szgroup] << "\n";


                // disaggregate total catch (in weight) for this pop according to the alloc key
                catch_per_szgroup[szgroup]= (tot_landings_this_pop * alloc_key1[szgroup]) +(tot_discards_this_pop * alloc_key2[szgroup]);
                // then get the removals in terms of N
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)   cout << "total catch catch_per_szgroup[" << szgroup<< "] is " << catch_per_szgroup[szgroup] << "\n";
                removals_per_szgroup[szgroup]= catch_per_szgroup[szgroup]/weight_at_szgroup[szgroup];
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)   cout << "removals_per_szgroup[" << szgroup<< "] is " << removals_per_szgroup[szgroup] << "\n";



                // do not allow negative abundance!
                if(removals_per_szgroup[szgroup] > new_Ns_at_szgroup_pop[szgroup])
                {

                    catch_per_szgroup[szgroup]=(Ns_at_szgroup_pop[szgroup])*weight_at_szgroup[szgroup];
                    dout  (cout << "the szgroup " << szgroup <<
                        "for this pop " << name_pop << " is fully depleted on this node by other land. " <<
                        idx_node << "! catch is "<<
                        catch_per_szgroup[szgroup] << "\n");
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "total catch corrected catch_per_szgroup[" << szgroup<< "] is " << catch_per_szgroup[szgroup] << "\n";

                                 // take all...
                    removals_per_szgroup[szgroup]=new_Ns_at_szgroup_pop[szgroup];
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "corrected removals_per_szgroup[" << szgroup<< "] is " << removals_per_szgroup[szgroup] << "\n";
                                 // nothing left.
                    new_Ns_at_szgroup_pop[szgroup]=0;
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "1 new_Ns_at_szgroup_pop[" << szgroup<< "] is " << new_Ns_at_szgroup_pop[szgroup] << "\n";
                                 // just right after the calculation of removals, reverse back to get the landings only
                  land_per_szgroup[szgroup] = catch_per_szgroup[szgroup] / (1+discardfactor); // first...
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "1 land_per_szgroup[" << szgroup<< "] is " << land_per_szgroup[szgroup] << "\n";
                  disc_per_szgroup[szgroup] = catch_per_szgroup[szgroup] - land_per_szgroup[szgroup] ; // first...
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "1 disc_per_szgroup[" << szgroup<< "] is " << disc_per_szgroup[szgroup] << "\n";
                  catch_per_szgroup[szgroup]= land_per_szgroup[szgroup]; //..second
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "1 catch_per_szgroup[" << szgroup<< "] is " << catch_per_szgroup[szgroup] << "\n";

                    if(will_I_discard_all==1)
                    {
                    disc_per_szgroup[szgroup] = disc_per_szgroup[szgroup]  + catch_per_szgroup[szgroup];
                    catch_per_szgroup[szgroup]= 0; // DISCARD ALL!
                    }

                }
                else
                {
                    // finally, impact the N
                    new_Ns_at_szgroup_pop[szgroup]=Ns_at_szgroup_pop[szgroup]-removals_per_szgroup[szgroup];
                    if (removals_per_szgroup[szgroup] < 0)
                    {
                        cout << "Negative removals detected in oth_land! in removals_per_szgroup[szgroup] ...set to 0!" << "\n";
                        removals_per_szgroup[szgroup] = 0;
                    }

                                 // reverse back to get the landings only
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "2 new_Ns_at_szgroup_pop[" << szgroup<< "] is " << new_Ns_at_szgroup_pop[szgroup] << "\n";
                    land_per_szgroup[szgroup] = catch_per_szgroup[szgroup] / (1+discardfactor) ; // first...
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "2 land_per_szgroup[" << szgroup<< "] is " << land_per_szgroup[szgroup] << "\n";
                    disc_per_szgroup[szgroup] = catch_per_szgroup[szgroup] - land_per_szgroup[szgroup] ; // first...
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)cout << "2 disc_per_szgroup[" << szgroup<< "] is " << disc_per_szgroup[szgroup] << "\n";
                    catch_per_szgroup[szgroup]= land_per_szgroup[szgroup]; //..second
//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "2 catch_per_szgroup[" << szgroup<< "] is " << catch_per_szgroup[szgroup] << "\n";

                    if(will_I_discard_all==1)
                    {
                    disc_per_szgroup[szgroup] = disc_per_szgroup[szgroup]  + catch_per_szgroup[szgroup];
                    catch_per_szgroup[szgroup]= 0; // DISCARD ALL!
                    }

                   // update the availability to impact the future vessel cpue
                    /*
                    double val=0;// init
                    if(szgroup==0 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
                    {
                        val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
                        new_avai_pops_at_selected_szgroup.at(0)=val;
                    }
                    if(szgroup==2 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]) && new_avai_pops_at_selected_szgroup.size()>1)
                    {
                        val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
                        new_avai_pops_at_selected_szgroup.at(1)=val;
                    }
                    if(szgroup==3 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]) && new_avai_pops_at_selected_szgroup.size()>2)
                    {
                        val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
                        new_avai_pops_at_selected_szgroup.at(2)=val;
                    }
                    if(szgroup==5 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]) && new_avai_pops_at_selected_szgroup.size()>3)
                    {
                        val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
                        new_avai_pops_at_selected_szgroup.at(3)=val;
                    }
                    if(szgroup==7 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]) && new_avai_pops_at_selected_szgroup.size()>4)
                    {
                        val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
                        new_avai_pops_at_selected_szgroup.at(4)=val;
                    }

                    this->set_avai_pops_at_selected_szgroup(name_pop, new_avai_pops_at_selected_szgroup);
                    */

                }


                new_removals_at_szgroup_pop[szgroup]=cumul_removals_at_szgroup_pop[szgroup]+removals_per_szgroup[szgroup];


            }
            else
            {
                new_Ns_at_szgroup_pop[szgroup]=0;
                removals_per_szgroup[szgroup]=0;
                new_removals_at_szgroup_pop[szgroup]=cumul_removals_at_szgroup_pop[szgroup]+removals_per_szgroup[szgroup];

            }


            // add to cumul per node
            this->add_to_cumcatches_per_pop(catch_per_szgroup[szgroup], name_pop);
            this->add_to_cumdiscards_per_pop(disc_per_szgroup[szgroup], name_pop);

        } // end szgroup

        // check catches realized on this node
        //dout(cout  << "oth_land this pop this node, before: "<<  oth_land_this_pop_this_node << "\n");
        //oth_land_this_pop_this_node=0;
        //for(unsigned int szgroup=0; szgroup < catch_per_szgroup.size(); szgroup++)
        //{
        //	oth_land_this_pop_this_node+=catch_per_szgroup.at(szgroup);
        //}
        //dout(cout  << "oth_land this pop this node, after potential correction (when total depletion): "<<  oth_land_this_pop_this_node << "\n");

//if(name_pop==2 && this->get_idx_node().toIndex()==19290) cout << "OTH_LAND FINALLY new_Ns_at_szgroup_pop " << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)        for (int i=0; i <new_Ns_at_szgroup_pop.size();++i) cout << new_Ns_at_szgroup_pop.at(i) << " " ;
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)        cout << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)cout << "OTH_LAND FINALLY new_removals_at_szgroup_pop " << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)               for (int i=0; i <new_removals_at_szgroup_pop.size();++i) cout << new_removals_at_szgroup_pop.at(i) << " " ;
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)               cout << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)cout << "OTH_LAND FINALLY catch_per_szgroup " << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)                     for (int i=0; i <catch_per_szgroup.size();++i) cout << catch_per_szgroup.at(i) << " " ;
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)                     cout << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)cout << "OTH_LAND FINALLY disc_per_szgroup " << "\n";
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)                            for (int i=0; i <disc_per_szgroup.size();++i) cout << disc_per_szgroup.at(i) << " " ;
//if(name_pop==2 && this->get_idx_node().toIndex()==19290)                           cout << "\n";

        // updates
        this->set_Ns_pops_at_szgroup(name_pop, new_Ns_at_szgroup_pop);
        this->set_removals_pops_at_szgroup(  name_pop, new_removals_at_szgroup_pop);
        this->set_last_oth_catch_pops_at_szgroup(  name_pop, catch_per_szgroup); // for tracking in pop stat panel
        this->set_last_oth_disc_pops_at_szgroup(  name_pop, disc_per_szgroup);   // for tracking in pop stat panel



    }
    else
    {
        dout(cout  << "no biomass available here...." << tot << "\n");

    }

    // check for consistency i.e. no gain in N !!
    //for(unsigned int szgroup=0; szgroup <init_Ns_at_szgroup_pop.size(); szgroup++)
    //{
    //    if(new_Ns_at_szgroup_pop.at(szgroup) > init_Ns_at_szgroup_pop.at(szgroup))
    //    {
    //        cout << "inconsistency in node->apply_oth_land() for this pop " << name_pop <<
    //                  " on this node " << idx_node <<
    //                    " for this szgroup " <<  szgroup <<"\n";
    //        dout(cout << "new_Ns_at_szgroup_pop is " << new_Ns_at_szgroup_pop.at(szgroup) << "\n");
    //        dout(cout << "while init_Ns_at_szgroup_pop is " << init_Ns_at_szgroup_pop.at(szgroup) << "\n");
    //        int a_int;
    //        cin >> a_int; // pause
    //    }
    //}
    dout(cout << "END: apply_oth_land()" << "\n");
}



void Node::export_popnodes(ofstream& popnodes,  multimap<int,double> weight_at_szgroup, int tstep)
{
	// note : pble with c++: circularity impossible then need to call init_weight_atszgroup
	// instead of using population->get_weight_at_szgroup()

    dout(cout  << "export biomass on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

	popnodes << setprecision(3) << fixed;
	// tstep / node / long / lat / tot N sp0 / tot N sp1 /...
    popnodes << tstep << " " << this->get_idx_node().toIndex() << " "<<
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

	popnodes << " " <<  "\n";

}


void Node::export_popnodes_impact(ofstream& popnodes, int tstep, int pop)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

	popnodes << setprecision(8) << fixed;
	// tstep / node / long / lat /  tot impact pop
    if(impact_per_pop.at(pop)>1e-6) popnodes << pop << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
		" " << this->get_x() << " " << this->get_y() << " " <<
        impact_per_pop.at(pop) << " " <<  "\n";

}

void Node::export_popnodes_cumulcatches_per_pop(ofstream& popnodes, int tstep, int pop)
{

    dout(cout  << "export accumulated catches (i.e. landings) on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  tot cumcatches pop
    if(cumcatches_per_pop.at(pop)>1e-6) popnodes << pop << " " << tstep << " " << this->get_idx_node().toIndex() <<
        " " << this->get_x() << " " << this->get_y() << " " <<
        cumcatches_per_pop.at(pop) << " " <<  "\n";

}

void Node::export_nodes_envt(ofstream& nodes_envt, int tstep)
{

    dout(cout  << "export nodes_envt" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    nodes_envt << setprecision(8) << fixed;
    // tstep / node / long / lat /  tot cumcatches pop
    nodes_envt << tstep << " " << this->get_idx_node().toIndex() << " "<<
        this->get_marine_landscape() << " " << this->get_salinity() << " " <<
                  " " << this->get_sst() << " " << this->get_wind() << " " <<
                  " " << this->get_Nitrogen() << " " << this->get_Phosphorus() << " " <<
                  " " << this->get_Oxygen() << " " << this->get_DissolvedCarbon() << " " <<
                     this->get_bathymetry()  << " " << this->get_shippingdensity() << " " << this->get_siltfraction() <<  "\n";

}


void Node::export_popnodes_impact_per_szgroup(ofstream& popnodes, int tstep, int pop)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

	vector<double> impact_per_szgroup=get_pressure_pops_at_szgroup(pop);

	popnodes << setprecision(3) << fixed;
	// pop/ tstep / node / long / lat /  impact sz0 / impact sz1 /...
    popnodes << pop << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
		" " << this->get_x() << " " << this->get_y() ;

    for(unsigned int sz = 0; sz < impact_per_pop.size(); sz++)
	{
        popnodes << " " << impact_per_pop.at(sz);
	}

	popnodes << " " <<  "\n";

}


void Node::export_popnodes_cumftime(ofstream& popnodes, int tstep)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

	popnodes << setprecision(8) << fixed;
	// tstep / node / long / lat /  tot impact pop
    if(cumftime>1e-6) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
		" " << this->get_x() << " " << this->get_y() << " " <<
		cumftime << " " <<  "\n";

}

void Node::export_popnodes_cumsweptarea(ofstream& popnodes, int tstep)
{

    dout(cout  << "export swept area on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  swept area
    if(cumsweptarea>1e-6) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        cumsweptarea << " " <<  cumsubsurfacesweptarea << "\n";

}

void Node::export_popnodes_cumcatches(ofstream& popnodes, int tstep)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  tot impact pop
   if(cumcatches>1e-6) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        cumcatches << " " <<  "\n";

}

void Node::export_popnodes_cumcatches_with_threshold(ofstream& popnodes, int tstep, int threshold)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  tot impact pop
   if(cumcatches>1e-6) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        cumcatches << " " << threshold << " " << "\n";

}

void Node::export_popnodes_cumdiscards(ofstream& popnodes, int tstep)
{

    dout(cout  << "export impact on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  tot cumdiscards pop
   if(cumdiscards>1e-6) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        cumdiscards << " "  << "\n";

}

void Node::export_popnodes_cumdiscardsratio(ofstream& popnodes, int tstep)
{

    dout(cout  << "export cumdiscardratio on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  value
   if(this->get_cumdiscardsratio()>1e-6 && this->get_cumdiscards()>1) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        this->get_cumdiscardsratio() << " "  << "\n";

}

void Node::export_popnodes_nbchoked(ofstream& popnodes, int tstep)
{

    dout(cout  << "export nbchoked on nodes for use in e.g. a GIS engine" << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  value
   if(this->get_nbchoked()>=1) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " "<<
        " " << this->get_x() << " " << this->get_y() << " " <<
        this->get_nbchoked() << " "  << "\n";

}

void Node::export_popnodes_metrealtimeclosed(ofstream& popnodes, int tstep)
{
    // CAUTION: EXPORTING ALL CLOSED NODE FOR NOW.....I.E. NOT ONLY THE ONES TRIGGERED BY RTC....


    dout(cout << "export realtime closed nodes for use in e.g. a GIS engine" << "\n");
    // note that this file could also be used by the ui for displaying the statistics on node

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  value
    int i = 0;
    int count = -1;
    vector<bool> banned = this->mBannedMetiers;
    while (i < banned.size()) {
        count++;
        if(banned.at(i)!=0) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " " <<
            " " << this->get_x() << " " << this->get_y() << " " <<
            count << " " << "\n";
    }
}

void Node::export_popnodes_tariffs(ofstream& popnodes, int tstep)
{

    dout(cout  << "export tariffs on nodes...." << "\n");
    // note that this file will also be used by the ui for displaying the statistics on node

    int met = 0;

    popnodes << setprecision(8) << fixed;
    // tstep / node / long / lat /  tariffs
    if (tariffs.at(0) > 1e-6) popnodes << " " << tstep << " " << this->get_idx_node().toIndex() << " " <<
        " " << this->get_x() << " " << this->get_y() << " ";
    while (met < tariffs.size()) {
        popnodes << tariffs.at(met) << " ";
        ++met;
    }
    popnodes << "\n";
}


void Node::init_benthos_tot_biomass_on_node(int funcgrp)
{
    benthos_tot_biomass.clear();
    for (int gr = 0; gr < funcgrp; ++gr) benthos_tot_biomass.push_back(0);

}

void Node::init_benthos_tot_number_on_node(int funcgrp)
{

    benthos_tot_number.clear();
    for (int gr = 0; gr < funcgrp; ++gr) benthos_tot_number.push_back(0);

}

void Node::init_benthos_tot_meanweight_on_node(int funcgrp)
{

    benthos_tot_meanweight.clear();
    for (int gr = 0; gr < funcgrp; ++gr) benthos_tot_meanweight.push_back(0);

}

void Node::init_benthos_tot_biomass_K_on_node(int funcgrp)
{

    benthos_tot_biomass_K.clear();
    for (int gr = 0; gr < funcgrp; ++gr) benthos_tot_biomass_K.push_back(0);

}

void Node::init_benthos_tot_number_K_on_node(int funcgrp)
{

    benthos_tot_number_K.clear();
    for (int gr = 0; gr < funcgrp; ++gr) benthos_tot_number_K.push_back(0);

}


void Node::set_is_harbour(int id)
{
    harbour = id;
    is_harbour = (id != 0); // reading 0 in entry means the node is at_sea
}


void Node::export_benthos_tot_biomass_per_funcgroup(ofstream& benthosbiomassnodes, int tstep, int funcgr)
{

    dout(cout  << "export benthos on nodes for use in e.g. a GIS engine" << "\n");

    double benthosnumber=0;
    if(!benthos_tot_meanweight.empty() && benthos_tot_meanweight.at(funcgr)!=0)  benthosnumber = benthos_tot_biomass.at(funcgr)/benthos_tot_meanweight.at(funcgr);
    
    double benthosnumberoverK=0;
    if(!benthos_tot_number_K.empty() && benthos_tot_number_K.at(funcgr)!=0)  benthosnumberoverK = benthosnumber/benthos_tot_number_K.at(funcgr);
    
    double benthosbiomassoverK=0;
    if(!benthos_tot_biomass_K.empty() && benthos_tot_biomass_K.at(funcgr)!=0)  benthosbiomassoverK = benthos_tot_biomass.at(funcgr)/benthos_tot_biomass_K.at(funcgr);
    
 
    // pop/ tstep / node / long / lat / number func group id /biomass func group id/ mean weight func group id / benthosbiomassoverK / benthosnumberoverK /benthos_tot_biomass_K.at(funcgr)
    if(!benthos_tot_biomass.empty() && benthos_tot_biomass.at(funcgr)>1e-5 && (tstep==0 ||
                                               tstep==8761 ||
                                               tstep==17521 ||
                                               tstep==26281 ||
                                               tstep==35065 ||
                                               tstep==43825 ||
                                               tstep==52585))
         benthosbiomassnodes << setprecision(0) << fixed << funcgr << " " << tstep << " " <<
          setprecision(3) << fixed <<  this->get_idx_node().toIndex() << " " << this->get_x() << " " << this->get_y() << " " <<
          setprecision(0) << fixed << benthosnumber << " " << benthos_tot_biomass.at(funcgr) << " " <<
          setprecision(2) << fixed << benthos_tot_meanweight.at(funcgr) << " " <<
          setprecision(3) << fixed << benthosbiomassoverK  << " " << benthosnumberoverK <<  " " <<
          setprecision(0) << fixed << benthos_tot_biomass_K.at(funcgr) << "\n";

}

void Node::export_benthos_tot_number_per_funcgroup(ofstream& benthosnumbernodes, int tstep, int funcgr)
{

    dout(cout  << "export benthos on nodes for use in e.g. a GIS engine" << "\n");

    double benthosbiomass = 0;
    if (!benthos_tot_number.empty() && benthos_tot_meanweight.at(funcgr) != 0)  benthosbiomass = benthos_tot_number.at(funcgr)*benthos_tot_meanweight.at(funcgr);

    double benthosnumberoverK=0;
    if(!benthos_tot_number_K.empty() &&benthos_tot_number_K.at(funcgr)!=0)  benthosnumberoverK = benthos_tot_number.at(funcgr)/benthos_tot_number_K.at(funcgr);

    double benthosbiomassoverK=0;
    if(!benthos_tot_biomass_K.empty() && benthos_tot_biomass_K.at(funcgr)!=0)  benthosbiomassoverK = benthosbiomass/benthos_tot_biomass_K.at(funcgr);

    benthosnumbernodes << setprecision(3) << fixed;
    // pop/ tstep / node / long / lat / number func group id /biomass func group id/ mean weight func group id / benthosbiomassoverK / benthosnumberoverK / benthos_tot_number_K.at(funcgr)
    if(!benthos_tot_number.empty() && benthos_tot_number.at(funcgr)>1e-5) benthosnumbernodes << funcgr << " " << tstep << " " << this->get_idx_node().toIndex() <<
        " " << this->get_x() << " " << this->get_y() << " " << benthos_tot_number.at(funcgr) << " " << benthosbiomass << " "  <<
                       benthos_tot_meanweight.at(funcgr) <<  " " << benthosbiomassoverK  << " " << benthosnumberoverK << " " << benthos_tot_number_K.at(funcgr) << "\n";


}




void Node::setAreaType(int _area_type)
{
    area_type=_area_type;
}


int Node::evaluateAreaType()
{
    return area_type;
}



