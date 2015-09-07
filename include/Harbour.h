// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef HARBOUR_H
#define HARBOUR_H

#include <string>
#include <iostream>
#include <vector>

#include "Node.h"

// to help boost to cope with polymorphism:
//#include <boost/serialization/export.hpp>

using namespace std;

class Harbour : public Node
{
	public:
        Harbour(int idx,
            double xval,
            double yval,
            int _harbour,
            int _code_area,
            int _code_landscape,
            int nbpops,
            int nbbenthospops,
            int nbszgroups,
            string _name,
        //multimap<string, double> mean_fish_price_per_met_per_pop,
            multimap<int, double> mean_fish_price_per_pop_per_cat,
            map<int, double> fuelprice,
            vector<int> usual_fgrounds,
            vector<double> freq_usual_fgrounds);
        Harbour(string name);
		Harbour();
        Harbour(int idx,double xval,double yval,int _harbour);

		~Harbour();
		virtual string get_name() const;
		//virtual double get_prices(string met, int pop) ;
		virtual double get_prices_per_cat(int pop, int cat) ;
        virtual double get_fuelprices(int vsize) ;
        virtual const vector<int> &get_usual_fgrounds () const;
        virtual const vector<double> &get_freq_usual_fgrounds () const;
        virtual void set_usual_fgrounds (vector <int> usual_fgrounds);
        virtual void set_freq_usual_fgrounds (vector <double> freq_usual_fgrounds);
        virtual const multimap<int, int> &get_usual_metiers() const;
        virtual const multimap<int,double> &get_freq_usual_metiers () const;
        virtual void set_usual_metiers (multimap<int, int> usual_metiers);
        virtual void set_freq_usual_metiers (multimap <int,double> freq_usual_metiers);


	private:
		string name;
		//int idx_node;
        map<int, double>  fuelprice;
		//multimap<string, double> mean_fish_price_per_met_per_pop;
		multimap<int, double> mean_fish_price_per_pop_per_cat;
        vector <int> usual_fgrounds;
        vector <double> freq_usual_fgrounds;
        multimap<int, int> usual_metiers;
        multimap<int, double> freq_usual_metiers;

};
#endif							 // HARBOUR_H
