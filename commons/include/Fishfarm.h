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

#ifndef FISHFARM_H
#define FISHFARM_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "myutils.h"
#include "Node.h"

class Fishfarm
{
    public:
        Fishfarm(int name,
            const vector<Node* > &nodes,
            const multimap <int, double> fishfarm_size
             );

        Fishfarm();
        virtual ~Fishfarm();
                                 // Copy constructor
        int get_name() const;
        const vector<Node *> &get_list_nodes() const;
        void set_list_nodes(vector<Node* > _list_nodes);
        double get_x() const;
        double get_y() const;
        void set_x(double value);
        void set_y(double value);

    protected:
    private:
        int name;
        double x, y;
                                 // area distribution
        vector<Node* > list_nodes;
        multimap <int, double> fishfarm_size;

};
#endif							 // FISHFARM_H
