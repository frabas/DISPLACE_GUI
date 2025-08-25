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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Ship.h"
#define PING_RATE 1				 // tstep=> 1 hour
#define NAUTIC 1.852			 // 1 knots=1.852 km
#define PI 3.14159265

#include <helpers.h>
//------------------------------------------------------------//
//------------------------------------------------------------//
// creator methods...
//------------------------------------------------------------//
//------------------------------------------------------------//

Ship::Ship()
{
    idx_ship = 0;

    // output stat variables
    nb_transported_units=0;
    fuel_use_litre=0.0;
    NOxEmission=0.0;
    SOxEmission=0;
    GHGEmission=0;
    PMEEmission=0;


}


Ship::~Ship()
{

}


Ship::Ship(int idx, string a_name, int is_active, double a_imo, double a_yearbuild, string a_flag,
           string a_type, double a_typecode, double a_loa, double a_KW, double a_breadth,
           double a_grosstonnage, double a_nbunits,
           double a_fueluse, double a_NOxEmission_gperKWh,
           double a_SOxEmission_percentpertotalfuelmass,
           double a_GHGEmission_gperKWh, double a_PMEEmission_gperKWh,
           double a_vmax, double a_vcruise,
           vector<double> a_longs, vector<double> a_lats)
{
    count=0;
    idx_ship = idx;
    is_active=is_active; // active by default
	origin_x =a_longs[0];
	origin_y =a_lats[0];
	longs=a_longs;
	lats=a_lats;
	end_point_x=a_longs[1];
	end_point_y=a_lats[1];
	x=a_longs[0];
	y=a_lats[0];
	vmax=a_vmax;
	vcruise=a_vcruise;
	course=0;
	name = a_name;
    imo = a_imo;
    yearbuild=a_yearbuild;
    flag=a_flag;
    type=a_type;
    typecode=a_typecode;
    loa=a_loa;
    KW=a_KW;
    breadth=a_breadth;
    grosstonnage=a_grosstonnage;
    nbunits=a_nbunits;
    fueluse=a_fueluse;

    NOxEmission_gperKWh=a_NOxEmission_gperKWh;  // 9 for Tier III 1st jan 2016
    SOxEmission_percentpertotalfuelmass=a_SOxEmission_percentpertotalfuelmass;// limit is 0.10% y 1 January 2015
    GHGEmission_gperKWh=a_GHGEmission_gperKWh;
    PMEEmission_gperKWh=a_PMEEmission_gperKWh;

    // output stat variables
    nb_transported_units=0;
    fuel_use_litre=0.0;
    NOxEmission=0.0;
    SOxEmission=0;
    GHGEmission=0;
    PMEEmission=0;

}


//------------------------------------------------------------//
//------------------------------------------------------------//
// getters...
//------------------------------------------------------------//
//------------------------------------------------------------//

int Ship::get_idx () const
{
	return(idx_ship);
}


int Ship::get_count () const
{
	return(count);
}

int Ship::get_is_active () const
{
    return(is_active);
}



string Ship::get_name () const
{
	return(name);
}

string Ship::get_type () const
{
    return(type);
}

string Ship::get_flag () const
{
    return(flag);
}

double Ship::get_imo () const
{
    return(imo);
}

double Ship::get_typecode () const
{
    return(typecode);
}

double Ship::get_grosstonnage () const
{
    return(grosstonnage);
}

double Ship::get_loa () const
{
    return(loa);
}

double Ship::get_KW () const
{
    return(KW);
}

double Ship::get_breadth () const
{
    return(breadth);
}

double Ship::get_nbunits () const
{
    return(nbunits);
}

double Ship::get_fueluse () const
{
    return(fueluse);
}

double Ship::get_NOxEmission_gperKWh () const
{
    return(NOxEmission_gperKWh);
}

double Ship::get_SOxEmission_percentpertotalfuelmass () const
{
    return(SOxEmission_percentpertotalfuelmass);
}

double Ship::get_GHGEmission_gperKWh () const
{
    return(GHGEmission_gperKWh);
}

double Ship::get_PMEEmission_gperKWh () const
{
    return(PMEEmission_gperKWh);
}

double Ship::get_yearbuild () const
{
    return(yearbuild);
}


double Ship::get_vmax () const
{
	return(vmax);
}


double Ship::get_vcruise () const
{
	return(vcruise);
}


vector<double> Ship::get_longs () const
{
	return(longs);
}


vector<double> Ship::get_lats () const
{
	return(lats);
}


double Ship::get_x () const
{
	return(x);
}


double Ship::get_y () const
{
	return(y);
}


double Ship::get_origin_x () const
{
	return(origin_x);
}


double Ship::get_origin_y () const
{
	return(origin_y);
}


double Ship::get_end_point_x () const
{
	return(end_point_x);
}


double Ship::get_end_point_y () const
{
	return(end_point_y);
}


double Ship::get_course () const
{
	return(course);
}


double Ship::get_nb_transported_units () const
{
    return(nb_transported_units);
}

double Ship::get_fuel_use_litre () const
{
    return(fuel_use_litre);
}

double Ship::get_NOxEmission () const
{
    return(NOxEmission);
}

double Ship::get_SOxEmission () const
{
    return(SOxEmission);
}

double Ship::get_GHGEmission () const
{
    return(GHGEmission);
}

double Ship::get_PMEEmission () const
{
    return(PMEEmission);
}

//------------------------------------------------------------//
//------------------------------------------------------------//
// setters...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Ship::set_idx_ship (int _idx)
{
    idx_ship= _idx;
}


void Ship::set_lats (vector <double> _lats)
{
	lats= _lats;
}


void Ship::set_longs (vector <double> _longs)
{
	longs= _longs;
}


void Ship::set_count (int _count)
{
	count= _count;
}


void Ship::set_vmax (double _vmax)
{
	vmax= _vmax;
}


void Ship::set_vcruise (double _vcruise)
{
	vcruise= _vcruise;
}


void Ship::set_course (double _course)
{
	course= _course;
}


void Ship::set_xy (double _x, double _y)
{
	x= _x;
	y= _y;
}


void Ship::set_xy (vector <double> _xy)
{
	x= _xy[0];
	y= _xy[1];
}


void Ship::set_end_point_xy (double _x, double _y)
{
	end_point_x= _x;
	end_point_y= _y;
}


void Ship::set_lane (vector<double> _longs, vector<double> _lats)
{
	this->set_lats(_lats);
	this->set_longs(_longs);
	this->set_xy(_longs[0], _lats[0]);
	this->set_end_point_xy(_longs[1], _lats[1]);
	this->set_count(0);
}


void Ship::set_fuel_use_litre(double _cumul)
{
    fuel_use_litre=_cumul;
}

void Ship::set_nb_transported_units(double _cumul)
{
    nb_transported_units=_cumul;
}

void Ship::set_NOxEmission(double _cumul)
{
    NOxEmission=_cumul;
}

void Ship::set_SOxEmission(double _cumul)
{
    SOxEmission=_cumul;
}

void Ship::set_GHGEmission(double _cumul)
{
    GHGEmission=_cumul;
}

void Ship::set_PMEEmission(double _cumul)
{
    PMEEmission=_cumul;
}


//------------------------------------------------------------//
//------------------------------------------------------------//
// methods
//------------------------------------------------------------//
//------------------------------------------------------------//

void Ship::move()
{

    // update tracking
    this->set_fuel_use_litre(get_fuel_use_litre() + get_fueluse()); // assuming no shut down of the engine so far
    //cout <<  "accumulated fuel use for this ship "<< this->get_name() << " is " << this->get_fuel_use_litre() << "\n";

	// movement for a ship
	set_course( bearing(get_x(), get_y(), get_end_point_x(), get_end_point_y()) );
	vector<double> a_move = destB (get_x(), get_y(), get_course(), get_vcruise()* PING_RATE * NAUTIC);

    dout(cout  << "should the ship move to (" << a_move[0] << "," << a_move[1] << ") ?" << "\n");
	int is_pt = is_pt_lying_on_segment(get_x(), a_move[0], get_end_point_x(),
		get_y(), a_move[1], get_end_point_y());
	// test if the next move (on the same segment) is acceptable
	if(!is_pt)
	{

        dout(cout  << "yes..." << "\n");
		set_xy(a_move);			 // YES, keep this move
	}
	else
	{
        dout(cout  << "no..." << "\n");
		vector <double> this_longs = get_longs();
        dout(cout  << "end point x is "  << "\n");
        dout(cout  <<  get_end_point_x() << "\n");
        dout(cout  << "dist to end of the lane is "  << "\n");
        dout(cout  << fabs(this_longs.at(this_longs.size()-1) - get_end_point_x()) << "\n");
								 // the end of the lane reached...
		if( fabs(this_longs[this_longs.size()-1] - get_end_point_x())<1)
		{
            dout(cout  << "end of the lane..." << "\n");
			vector <double> the_longs = get_longs();
			vector <double> the_lats = get_lats();
			reverse(the_longs.begin(), the_longs.end());
			reverse(the_lats.begin(), the_lats.end());
								 // ...go backward
			this->set_lane(the_longs, the_lats);
		}
		else
		{
            dout(cout  << "compute a move on the next segment then..." << "\n");
			// compute a move (on the next segment)
			double dist_to_end_pt         = dist (get_x(), get_y(), get_end_point_x(), get_end_point_y());
			double dist_km_to_be_traveled = get_vcruise()* PING_RATE * NAUTIC;
			double dist_on_next_segment   = dist_km_to_be_traveled-dist_to_end_pt;
            dout(cout  << "dist_on_next_segment..." << dist_on_next_segment << "\n");
			set_count(get_count () + 1);
			int idx=(1+this->get_count());
			double end_point_x =get_longs().at(idx);
			double end_point_y =get_lats().at(idx);
            dout(cout  << "end_point_x is..." << end_point_x << "\n");
            dout(cout  << "end_point_y is..." << end_point_y << "\n");
			set_course( bearing(get_end_point_x(), get_end_point_y(), end_point_x, end_point_y) );
			vector<double> a_move = destB (this->get_end_point_x(), this->get_end_point_y(), this->get_course(), dist_on_next_segment);
            dout(cout  << "move to (" << a_move[0] << "," << a_move[1] << ") " << "\n");

			// update
			this->set_end_point_xy(end_point_x, end_point_y);
			this->set_xy(a_move);
		}
	}

}




void Ship::compute_emissions_in_ship()
{
// TODO
    this->set_nb_transported_units(this->get_nb_transported_units()); // TODO delivery and clearing at ports

    // for other indicators, apply just a cumul
    this->set_fuel_use_litre(this->get_fuel_use_litre() + this->get_fueluse());
    this->set_NOxEmission(this->get_NOxEmission() + (this->get_NOxEmission_gperKWh()*this->get_KW()));
    this->set_SOxEmission(this->get_SOxEmission() + (this->get_SOxEmission_percentpertotalfuelmass()*this->get_fueluse()));
    this->set_GHGEmission(this->get_GHGEmission() + (this->get_GHGEmission_gperKWh()*this->get_KW()));
    this->set_PMEEmission(this->get_PMEEmission() + (this->get_PMEEmission_gperKWh()*this->get_KW()));
}

void Ship::export_ships_indicators(ofstream& shiplogs, int tstep)
{

    dout(cout  << "export ships indicators...." << "\n");
    // note that this file will also be used by the ui for displaying the statistics in stat windows


    shiplogs << setprecision(3) << fixed;

    // see parseShipsStats():
    // tstep(0) / node(1) / long(2) / lat(3) /shiptype(4) / shipid(5) / nb_units / fuel_use_h /
    // NOx_emission_gperkW / SOx_emission_percentpertotalfuelmass / GHG_emission_gperkW / PME_emission_gperkW /
    // fuel_use_litre / NOx_emission / SOx_emission / GHG_emissions / PME_emission

    shiplogs <<  tstep << " " << 0 << " "<<  this->get_x() << " "<< this->get_y() << " "<<
        this->get_typecode() << " " << this->get_idx() << " "<< this->get_nbunits() << " " << this->get_fueluse() << " " <<
        this->get_NOxEmission_gperKWh() << " " << this->get_SOxEmission_percentpertotalfuelmass() << " " <<
        this->get_GHGEmission_gperKWh() << " " << this->get_PMEEmission_gperKWh() << " " <<
        this->get_fuel_use_litre() << " " <<
        this->get_NOxEmission() << " " << this->get_SOxEmission() << " " << this->get_GHGEmission() << " " <<  this->get_PMEEmission() << "\n";


}



