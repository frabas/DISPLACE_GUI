#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Ship.h"
#include "myutils.h"
#include "myRutils.h"
#define PING_RATE 1 // tstep=> 1 hour
#define NAUTIC 1.852 // 1 knots=1.852 km
#define PI 3.14159265


//this is how we can able/disable cout in C++:
//#define VERBOSE

#ifdef VERBOSE
#define dout cout
#else
#define dout 0 && cout
#endif


//------------------------------------------------------------//
//------------------------------------------------------------//
// creator methods...
//------------------------------------------------------------//
//------------------------------------------------------------//

Ship::Ship()
{
    idx_ship = 0;
}

Ship::~Ship()
{

}

Ship::Ship(string a_name, double a_vmax, double a_vcruise, vector<double> a_longs, vector<double> a_lats)
{
    count=0;
    idx_ship = 0;
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

string Ship::get_name () const
{
    return(name);
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


//------------------------------------------------------------//
//------------------------------------------------------------//
// setters...
//------------------------------------------------------------//
//------------------------------------------------------------//


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


//------------------------------------------------------------//
//------------------------------------------------------------//
// methods
//------------------------------------------------------------//
//------------------------------------------------------------//

void Ship::move()
{

// movement for a ship
    set_course( bearing(get_x(), get_y(), get_end_point_x(), get_end_point_y()) );
    vector<double> a_move = destB (get_x(), get_y(), get_course(), get_vcruise()* PING_RATE * NAUTIC);

    dout << "should the ship move to (" << a_move[0] << "," << a_move[1] << ") ?" << endl;
    int is_pt = is_pt_lying_on_segment(get_x(), a_move[0], get_end_point_x(),
                                       get_y(), a_move[1], get_end_point_y());
    // test if the next move (on the same segment) is acceptable
    if(!is_pt)
    {

        dout << "yes..." << endl;
        set_xy(a_move); // YES, keep this move
    }
    else
    {
        dout << "no..." << endl;
        vector <double> this_longs = get_longs();
        dout << "end point x is "  << endl;
        dout <<  get_end_point_x() << endl;
        dout << "dist to end of the lane is "  << endl;
        dout << fabs(this_longs.at(this_longs.size()-1) - get_end_point_x()) << endl;
        if( fabs(this_longs[this_longs.size()-1] - get_end_point_x())<1) // the end of the lane reached...
        {
            dout << "end of the lane..." << endl;
            vector <double> the_longs = get_longs();
            vector <double> the_lats = get_lats();
            reverse(the_longs.begin(), the_longs.end());
            reverse(the_lats.begin(), the_lats.end());
            this->set_lane(the_longs, the_lats); // ...go backward
        }
        else
        {
            dout << "compute a move on the next segment then..." << endl;
            // compute a move (on the next segment)
            double dist_to_end_pt         = dist (get_x(), get_y(), get_end_point_x(), get_end_point_y());
            double dist_km_to_be_traveled = get_vcruise()* PING_RATE * NAUTIC;
            double dist_on_next_segment   = dist_km_to_be_traveled-dist_to_end_pt;
            dout << "dist_on_next_segment..." << dist_on_next_segment << endl;
            set_count(get_count () + 1);
            int idx=(1+this->get_count());
            double end_point_x =get_longs().at(idx);
            double end_point_y =get_lats().at(idx);
            dout << "end_point_x is..." << end_point_x << endl;
            dout << "end_point_y is..." << end_point_y << endl;
            set_course( bearing(get_end_point_x(), get_end_point_y(), end_point_x, end_point_y) );
            vector<double> a_move = destB (this->get_end_point_x(), this->get_end_point_y(), this->get_course(), dist_on_next_segment);
            dout << "move to (" << a_move[0] << "," << a_move[1] << ") " << endl;

            // update
            this->set_end_point_xy(end_point_x, end_point_y);
            this->set_xy(a_move);
        }
    }

}




