// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef WINDMILL_H
#define WINDMILL_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "myutils.h"
#include "Node.h"

class  Windmill
{
    public:
        Windmill(int _idx, string _name, Node *_node, double _size, int _type, int _kWh, int _is_active);

        Windmill();
        virtual ~Windmill();
                                 // Copy constructor
        string get_name() const;
        int get_idx() const;
        int get_type() const;

        int get_is_active() const;

        double get_x() const;
        double get_y() const;

        void set_x(double value);
        void set_y(double value);

        double get_kWh() const;
        double get_kW_production() const;

        void set_kW_production (double _cumul);

        void compute_kWproduction_in_farm();
        void export_windmills_indicators(ofstream& windmillslogs, int tstep);

    protected:
    private:
        string name;
        int idx;
        int type;
        double x, y;
        double size;
        double kWh;
        int is_active=1;

        //output stats
        double kWProduction;

};
#endif							 // WINDMILL_H
