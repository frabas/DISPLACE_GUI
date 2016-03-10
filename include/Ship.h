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

#ifndef SHIP_H
#define SHIP_H
#include "myutils.h"
#include "Node.h"				 // to get the inline functions...
#include<vector>
#include <string>
#include <iostream>
#include <list>


#include <pthread.h>

class Ship
{
	private:
		string name;
        string flag;
        string type;
		int idx_ship;
		int count;
		vector<double> lats;
		vector<double> longs;
		double origin_x, origin_y, end_point_x, end_point_y;
		double x, y, course;
        double imo, yearbuild, typecode, loa, KW, breadth, grosstonnage, nbunits;
        double fueluse, NOxEmission_gperKWh, SOxEmission_percentpertotalfuelmass;
        double GHGEmission, PMEmission;
        double vmax, vcruise;

        double cumul_fueluse;

        pthread_mutex_t mutex;

    public:
        Ship(int idx, string name,  double imo, double yearbuild, string flag,
             string type, double typecode, double loa, double KW, double breadth,
             double grosstonnage, double nbunits,
             double fueluse, double NOxEmission_gperKWh, double SOxEmission_percentpertotalfuelmass,
             double GHGEmission, double PMEmission,
             double vmax, double vcruise,
             vector<double> lats, vector<double> longs);
		Ship();
		~Ship();

        void lock() { pthread_mutex_lock (&mutex); }
        void unlock() { pthread_mutex_unlock (&mutex); }


		// getters
		string get_name () const;
		int get_idx () const;
		int get_count () const;
		vector<double> get_lats () const;
		vector<double> get_longs () const;
								 // in the continuous space
		double get_origin_x() const;
								 // in the continuous space
		double get_origin_y() const;
								 // in the continuous space
		double get_end_point_x() const;
								 // in the continuous space
		double get_end_point_y() const;
		double get_x() const;	 // in the continuous space
		double get_y() const;	 // in the continuous space
        string get_type () const;
        string get_flag () const;
        double get_imo() const;
        double get_typecode() const;
        double get_grosstonnage() const;
        double get_loa() const;
        double get_KW() const;
        double get_breadth() const;
        double get_nbunits() const;
        double get_fueluse() const;
        double get_NOxEmission_gperKWh()const;
        double get_SOxEmission_percentpertotalfuelmass()const;
        double get_GHGEmission()const;
        double get_PMEmission()const;
        double get_yearbuild() const;
        double get_vmax() const;
		double get_vcruise() const;
		double get_course() const;

        double get_cumul_fueluse () const;

        // setters
        void set_idx_ship (int val);
        void set_count (int val);
        void set_vmax (double val);
		void set_vcruise (double val);
		void set_course (double val);
		void set_lats (vector<double> _lats);
		void set_longs (vector<double> _longs);
		void set_xy (double _x, double _y);
		void set_xy (vector <double> _xy);
		void set_end_point_xy (double _x, double _y);
		void set_lane (vector<double> _longs, vector<double> _lats);

        void set_cumul_fueluse (double _cumul);

        void move ();

};
#endif							 // SHIP_H
