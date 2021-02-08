// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef FIRM_H
#define FIRM_H


#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <myutils.h>
#include <options.h>
#include <dtree/decisiontree.h>
#include <dtree/stateevaluator.h>

#include <mutex>

typedef types::NodeId::type vertex_t;

class Node;
class Metier;
class Vessel;

class  Firm
{
public:

private:
        std::string name;
        int idx_firm;
        int nb_vessels;
        double x, y;
        std::vector <Vessel*> bunch_of_vessels;

        std::mutex mutex;

protected:
        void init();

public:
        Firm(int idx, int a_idx_firm, std::string a_name,  int a_nb_vessels, double lon, double lat, std::vector<Vessel*> &a_bunch_of_vessels);
        Firm();
        ~Firm();

        int get_idx () const;
        std::string get_name () const;
        double get_x () const;
        double get_y () const;
        std::vector<Vessel*> get_bunch_of_vessels();

        void set_x (double x);
        void set_y (double y);
        void set_idx_firm (int a_idx_firm);


        void lock() { mutex.lock(); }
        void unlock() { mutex.unlock(); }

};
#endif							 // FIRM_H
