// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef COMMONS_MYRUTILS_H
#define COMMONS_MYRUTILS_H

#include <string>
#include <m_constants.h>
#include <vector>
#include <algorithm>

#include <cmath>

using namespace std;

// source: adapted from Rmath
// e,g, http://svn.r-project.org/R/trunk/src/nmath/standalone/sunif.c
// TO DO: use a standalone compiled version of Rmath would be better...
void revsort(double *a, int *ib, int n);
// in random.c
void ProbSampleReplace(int n, double *p, int *perm, int nans, int *ans);
double unif_rand(void);

template <typename I>
std::vector<I> do_sample( int n, int nval, const std::vector<I> &val, const std::vector<double> &proba)
{
    using Rec = std::tuple<I, double>;
    class RecGreater {
    public:
        bool operator () (const Rec&v1, const Rec&v2) const {
            return std::get<1>(v1) > std::get<1>(v2);
        }
    };

    if (val.size() == 0 || proba.size() == 0 || nval == 0)
        return vector<I>();

    if (nval != (int)val.size() || nval != (int)proba.size())
        throw std::invalid_argument("do_sample requires nval == val.size() == proba.size()");

    double total = 0.0;

    for (auto pr : proba) {
        total += pr;
    }

    vector<Rec> prb;
    for (int i = 0; i < (int)val.size(); ++i) {
        prb.push_back(std::make_pair(val[i], proba[i]/ total));
    }

    vector<I> res;
    res.reserve(n);

    int nans = n;

    double rU;
    int nm1 = nval - 1;

    /* sort the probabilities into descending order */
    std::sort(prb.begin(), prb.end(), RecGreater());

    /* compute cumulative probabilities */
    for (int i = 1 ; i < nval; i++) {
        std::get<1>(prb[i]) += std::get<1>(prb[i - 1]);
    }

    /* compute the sample */
    for (int i = 0; i < nans; i++)
    {
        rU = unif_rand();
        int j;
        for (j = 0; j < nm1; j++)
        {
            if (rU <= std::get<1>(prb[j]))
                break;
        }
        res.push_back(std::get<0>(prb[j]));
    }
    return res;
}

//svn.r-project.org/R/trunk/src/nmath/
void set_seed(unsigned int i1, unsigned int i2);
void get_seed(unsigned int *i1, unsigned int *i2);
double exp_rand(void);
double fmax2(double x, double y);
double fmin2(double x, double y);
double norm_rand(void);
double rgamma(double a, double scale);
double rnorm(double mu, double sigma);
double rlnorm(double meanlog, double sdlog);

// distance between two points in long lat
inline double dist (double x1, double y1, double x2, double y2)
{
    double p = 180/M_PI;
    double Rearth = 6371.0;
    double res = Rearth * acos(sin(y1/p)*sin(y2/p) + (cos(y1/p) * cos(y2/p)*cos(x1/p - x2/p)));
    return(res);
}


// bearing between two points in long lat
inline double bearing (double x1, double y1, double x2, double y2)
{
    double p = 180/M_PI;
    double res = atan2(   sin((x2/p)-(x1/p))*cos(y2/p), cos(y1/p)*sin(y2/p)-sin(y1/p)*cos(y2/p)*cos((x2/p)-(x1/p)) );
    return(res*p);
}


// bearing between two points in long lat
inline vector<double> destB (double x1, double y1, double angleAB, double distkm)
{
    vector<double> res(2);
    double p = 180/M_PI;
    double Rearth = 6371.0;
    // LAT
    res[1] =  asin((sin(y1/p)*cos(distkm/Rearth)) + (cos(y1/p)*sin(distkm/Rearth)*cos(angleAB/p))) ;
    // LONG
    res[0] =  (x1/p + atan2(sin(angleAB/p)*sin(distkm/Rearth)*cos(y1/p), cos(distkm/Rearth)-sin(y1/p)*sin(res[1])) ) ;
    res[1] =   res[1] *p;
    res[0] =   res[0] *p;
    return(res);
}


inline vector <double> compute_line_equation (double x1, double x2, double y1, double y2)
{
    vector <double> res(2);
    double dx = x2 - x1;
    double dy = y2 - y1;
    res[0] = dy / dx;			 //slope
    res[1] = y1 - res[0] * x1;	 // intercept
    return(res);
}


// is a point lying inside a segment
inline int is_pt_lying_on_segment (double x1, double x2, double x3, double y1, double y2, double y3)
{
    vector <double> equ = compute_line_equation(x1,x2,y1,y2);
    double left, right, top, bottom;

    if(x1 < x2)
    {
        left = x1;
        right = x2;
    }
    else
    {
        left = x2;
        right = x1;
    }
    if(y1 < y2)
    {
        top = y1;
        bottom = y2;
    }
    else
    {
        top = y1;
        bottom = y2;
    }

    //test
    if( equ[0] * x3 + equ[1] > (y3 - 0.01) &&
            equ[0] * x3 + equ[1] < (y3 + 0.01)) {
        if (x3 > left && x3 < right &&
            y3 > top && y3 < bottom) {
            return (1);
        } else {
            return (0);
        }
    } else {
        return (0);
    }

}

#endif // COMMONS_MYRUTILS_H
