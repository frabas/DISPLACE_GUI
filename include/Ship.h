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
		int idx_ship;
		int count;
		vector<double> lats;
		vector<double> longs;
		double origin_x, origin_y, end_point_x, end_point_y;
		double x, y, course;
		double vmax, vcruise;

        pthread_mutex_t mutex;

    public:
		Ship(string name,  double vmax, double vcruise, vector<double> lats, vector<double> longs);
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
		double get_vmax() const;
		double get_vcruise() const;
		double get_course() const;

		// setters
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

		void move ();

};
#endif							 // SHIP_H
