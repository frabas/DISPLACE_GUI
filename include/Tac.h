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



#ifndef TAC_H
#define TAC_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "myutils.h"



class Tac
{

public:
    Tac(double init_tac, double tac_percent_simulated, map<string,double> relative_stability_key);
    Tac();
    virtual ~Tac();
    Tac(const Tac& other);    // Copy constructor
    double get_percent_for_simulated_vessels() const;
    map<string,double>  get_relative_stability_key() const;
    vector<double> get_ts_tac() const;
    double get_current_tac() const;
    void add_tac_y_plus_1(double a_tac);


protected:
private:
    double percent_for_simulated_vessels;
    map<string,double>  relative_stability_key; // nrow is nb of countries
    vector<double> ts_tac; // time series per year
    double current_tac;

   // serialization in xml with boost
    /*
     friend class boost::serialization::access;
                template<class Archive>
                void serialize(Archive & ar, const unsigned int version)
                {
                  }

    */
};

#endif // TAC_H

