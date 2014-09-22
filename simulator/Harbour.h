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
			int nbszgroups,
			string _name,
		//multimap<string, double> mean_fish_price_per_met_per_pop,
			multimap<int, double> mean_fish_price_per_pop_per_cat,
			map<string, double> fuelprice);
		Harbour(string name);
		Harbour();
		~Harbour();
		virtual string get_name() const;
		//virtual double get_prices(string met, int pop) ;
		virtual double get_prices_per_cat(int pop, int cat) ;
		virtual double get_fuelprices(string vsize) ;

	private:
		string name;
		//int idx_node;
		map<string, double>  fuelprice;
		//multimap<string, double> mean_fish_price_per_met_per_pop;
		multimap<int, double> mean_fish_price_per_pop_per_cat;

		// serialization in xml with boost
		/*
		   friend class boost::serialization::access;
					  template<class Archive>
					  void serialize(Archive & ar, const unsigned int version)
					  {
					  // method 1 : invoke base class serialization
					  boost::serialization::base_object<Node>(*this);
					  // method 2 : explicitly register base/derived relationship
					  //boost::serialization::void_cast_register<Harbour, Node>(
					  // static_cast<Node *>(NULL),
					  //  static_cast<Harbour *>(NULL)
					 //);

					   ar & boost::serialization::make_nvp("fuelprice", fuelprice);
					   ar & boost::serialization::make_nvp("mean_fish_price_per_met_per_pop", mean_fish_price_per_met_per_pop);
					  }
		*/

};
#endif							 // HARBOUR_H
