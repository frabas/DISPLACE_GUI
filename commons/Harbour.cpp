// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "Harbour.h"
#include <helpers.h>

Harbour::Harbour(types::NodeId idx,
double xval,
double yval,
int _harbour,
int _code_area,
int _code_landscape,
double _landscape_norm,
double _landscape_alpha,
double _wind,
double _wind_norm,
double _wind_alpha,
double _sst,
double _sst_norm,
double _sst_alpha,
double _salinity,
double _salinity_norm,
double _salinity_alpha,
double _Nitrogen,
double _Nitrogen_norm,
double _Nitrogen_alpha,
double _Phosphorus,
double _Phosphorus_norm,
double _Phosphorus_alpha,
double _Oxygen,
double _Oxygen_norm,
double _Oxygen_alpha,
double _DissolvedCarbon,
double _DissolvedCarbon_norm,
double _DissolvedCarbon_alpha,
double _bathymetry,
double _shippingdensity,
double _siltfraction,
double _icesrectanglecode,
double _benthos_biomass,
double _benthos_number,
double _benthos_meanweight,
double _benthos_biomass_K,
double _benthos_number_K,
int nbpops,
int nbmets,
int nbbenthospops,
int nbszgroups,
string _name,
//multimap<string, double> _mean_fish_price_per_met_per_pop,
multimap<int, double> _mean_fish_price_per_pop_per_cat,
map<int, double> _fuelprice,
vector<types::NodeId> _usual_fgrounds,
vector<double> _freq_usual_fgrounds)
: Node(idx, xval, yval,  _harbour, _code_area,
        _code_landscape, _landscape_norm, _landscape_alpha,
        _wind,  _wind_norm,  _wind_alpha,
        _sst,  _sst_norm,  _sst_alpha,
        _salinity,  _salinity_norm,  _salinity_alpha,
        _Nitrogen,  _Nitrogen_norm,  _Nitrogen_alpha,
        _Phosphorus, _Phosphorus_norm,  _Phosphorus_alpha,
        _Oxygen,  _Oxygen_norm,  _Oxygen_alpha,
        _DissolvedCarbon,  _DissolvedCarbon_norm,  _DissolvedCarbon_alpha,
        _bathymetry, _shippingdensity, _siltfraction, _icesrectanglecode,
       _benthos_biomass, _benthos_number,_benthos_meanweight, _benthos_biomass_K, _benthos_number_K, nbpops, nbmets, nbbenthospops, nbszgroups)
{
	name=_name;
	//mean_fish_price_per_met_per_pop= _mean_fish_price_per_met_per_pop;
	mean_fish_price_per_pop_per_cat=_mean_fish_price_per_pop_per_cat;
	fuelprice= _fuelprice;
    usual_fgrounds=_usual_fgrounds;
    freq_usual_fgrounds=_freq_usual_fgrounds;

}


Harbour::Harbour(string _name)
    : Node(), name(_name),
      fuelprice(), mean_fish_price_per_pop_per_cat()
{
}


Harbour::Harbour()
{
}

Harbour::Harbour(types::NodeId idx, double xval, double yval, int _harbour)
    : Node(idx, xval, yval,  _harbour, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
{

}


Harbour::~Harbour()
{
	//dtor
}


string Harbour::get_name() const
{
	return(this->name);
}


/*
double Harbour::get_prices(string met, int pop)
{
	double a_price;
// search into the multimap 'prices in met/pop'
	vector <double> spe_prices = find_entries_s_d(mean_fish_price_per_met_per_pop, met);
	//print_d(spe_prices);
	if(!spe_prices.empty())
	{
        dout(cout  << "hi there..." << "\n");
		a_price=spe_prices.at(pop);
	}
	else
	{
        dout(cout  << "no price info for this met on this harbour..." << "\n");
		// then, replace by taking another met eg the first met as it comes...
		multimap<string, double>::iterator it = mean_fish_price_per_met_per_pop.begin(); it++; // why it.begin() is void?
        dout(cout  << "instead, get met " << it->first << "\n");
		vector <double> spe_prices = find_entries_s_d(mean_fish_price_per_met_per_pop,
									 it->first);
		a_price=spe_prices.at(pop);
	}

	return(a_price);
}
*/

double Harbour::get_prices_per_cat(int pop, int cat)
{
	double a_price;
	// search into the multimap 'prices in pop/cat'
	vector <double> spe_prices = find_entries_i_d(mean_fish_price_per_pop_per_cat, pop);
	//print_d(spe_prices);
	if(!spe_prices.empty())
	{
       // dout(cout  << "fish price on harbour "<< this->get_name() << " and pop " << pop << "\n");
								 // caution to relative vector index here....
       // dout(cout  << "...for this cat "<< cat << "is:" << "\n");
        a_price=spe_prices.at(cat);
       // dout(cout  << a_price << "\n");

	}
	else
	{
        dout(cout  << "no price info for this pop on this harbour..." << "\n");
		// then, replace by taking another met eg the first met as it comes...
								 //it++; // why it.begin() is void?
		multimap<int, double>::iterator it = mean_fish_price_per_pop_per_cat.begin();
        dout(cout  << "instead, get pop " << it->first << "\n");
		vector <double> spe_prices = find_entries_i_d(mean_fish_price_per_pop_per_cat,
			it->first);
		a_price=spe_prices.at(cat);
	}

	return(a_price);
}


double Harbour::get_fuelprices(int vsize)
{
	// search into the map 'prices per vsize'
	double a_fuelprice= fuelprice.find(vsize)->second;

    return(a_fuelprice);
}


const vector<types::NodeId> &Harbour::get_usual_fgrounds() const
{
    return(usual_fgrounds);
}


const vector<double> &Harbour::get_freq_usual_fgrounds() const
{
    return(freq_usual_fgrounds);
}

void Harbour::set_usual_fgrounds(const vector<types::NodeId> &_usual_fgrounds)
{
    usual_fgrounds=_usual_fgrounds;
}


void Harbour::set_freq_usual_fgrounds(const vector <double> &_freq_usual_fgrounds)
{
    freq_usual_fgrounds=_freq_usual_fgrounds;
}

vector<types::NodeId> Harbour::get_usual_fgrounds_this_met(int met)
{
    auto usual_fgrounds_this_met  = find_entries( usual_fgrounds_per_met, met );

    return(usual_fgrounds_this_met);
}


vector<double> Harbour::get_freq_usual_fgrounds_this_met(int met)
{
     vector<double> freq_usual_fgrounds_this_met = find_entries_i_d( freq_usual_fgrounds_per_met, met );

    return(freq_usual_fgrounds_this_met);
}

void Harbour::set_usual_fgrounds_per_met(multimap<int, types::NodeId> _usual_fgrounds_per_met)
{
    usual_fgrounds_per_met=_usual_fgrounds_per_met;
}


void Harbour::set_freq_usual_fgrounds_per_met(multimap<int, double> _freq_usual_fgrounds_per_met)
{
    freq_usual_fgrounds_per_met=_freq_usual_fgrounds_per_met;
}


const multimap<types::NodeId, int> &Harbour::get_usual_metiers() const
{
    return(usual_metiers);
}


const multimap<int, double> &Harbour::get_freq_usual_metiers() const
{
    return(freq_usual_metiers);
}

void Harbour::set_usual_metiers(multimap<types::NodeId, int> _usual_metiers)
{
 usual_metiers=_usual_metiers;
}


void Harbour::set_freq_usual_metiers(multimap<int, double> _freq_usual_metiers)
{
 freq_usual_metiers=_freq_usual_metiers;
}

void Harbour::set_fishprice(int species, int marketcat)
{
    mean_fish_price_per_pop_per_cat.insert(std::make_pair(species, marketcat));
}

void Harbour::set_mean_fish_price_per_pop_per_cat(multimap<int, double> _mean_fish_price_per_pop_per_cat)
{
 mean_fish_price_per_pop_per_cat = _mean_fish_price_per_pop_per_cat;
}


void Harbour::set_fuelprice(map<int, double> _fuelprice)
{
 fuelprice = _fuelprice;
}
