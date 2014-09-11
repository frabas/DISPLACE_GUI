
#ifndef SHIP_H
#define SHIP_H
#include "myutils.h"
#include<vector>
#include <string>
#include <iostream>
#include <list>


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

public:
    Ship(string name,  double vmax, double vcruise, vector<double> lats, vector<double> longs);
    Ship();
    ~Ship();


    // getters
    string get_name () const;
    int get_idx () const;
    int get_count () const;
    vector<double> get_lats () const;
    vector<double> get_longs () const;
    double get_origin_x() const; // in the continuous space
    double get_origin_y() const; // in the continuous space
    double get_end_point_x() const; // in the continuous space
    double get_end_point_y() const; // in the continuous space
    double get_x() const; // in the continuous space
    double get_y() const; // in the continuous space
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

#endif // SHIP_H
