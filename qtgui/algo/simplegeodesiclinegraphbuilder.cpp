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

#include "simplegeodesiclinegraphbuilder.h"

#include <GeographicLib/GeodesicLine.hpp>

#include <QPointF>
#include <QDebug>

#include <cmath>

using namespace displace::graphbuilders;

SimpleGeodesicLineGraphBuilder::SimpleGeodesicLineGraphBuilder(Type type, double latMin_deg, double lonMin_deg, double latMax_deg, double lonMax_deg, double step)
    : mGeodesic(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f()),
      mLatMin(latMin_deg), mLonMin(lonMin_deg),
      mLatMax(latMax_deg), mLonMax(lonMax_deg),
      mStepX(step / 2.0)
{
    switch (type) {
    case Hex:
        mStepY = step * std::sqrt(3.0) / 2.0;
        mInterlace = true;
        break;
    case Quad:
        mStepY = step;
        mInterlace = false;
        break;
    }
}

bool SimpleGeodesicLineGraphBuilder::beginCreateGrid()
{
    a12_y = mGeodesic.Inverse(mLatMin, mLonMin, mLatMax, mLonMin, s12_y, azi1_y, azi2_y);
    line_y = GeographicLib::GeodesicLine(mGeodesic, mLatMin, mLonMin, azi1_y);
    num_y = int(std::ceil(s12_y / mStepY)); // The number of intervals
    da_y = a12_y / num_y;

    ds_y = s12_y / num_y;

    i = 0;
    j = 0;

    return true;
}

QPointF SimpleGeodesicLineGraphBuilder::getNext()
{
    if (i == 0) {
        // First inner loop
        double ylat,ylon;
        line_y.ArcPosition(j*da_y, ylat, ylon);
       // line_y.Position(j*ds_y, ylat, ylon);

        a12 = mGeodesic.Inverse(ylat, mLonMin, ylat, mLonMax, s12, azi1, azi2);
        line = GeographicLib::GeodesicLine(mGeodesic, ylat, mLonMin, azi1);
        num = int(std::ceil(s12 / mStepX)); // The number of intervals
        da = a12 / num;

        // Use intervals of equal length
        // ds = s12 / num;
    }

    double plat, plon;
    line.ArcPosition((i + (mInterlace ? j % 2 : 0))* da, plat, plon);

    //line.Position((i + (mInterlace ? j % 2 : 0))*  ds, plat, plon);
    // http://geographiclib.sourceforge.net/html/classGeographicLib_1_1GeodesicLine.html

    i += 2;
    if (i > num) {
        ++j;
        i = 0;
    }

    return QPointF(plon, plat);
}

bool SimpleGeodesicLineGraphBuilder::atEnd()
{
    return (j >= num_y);
}

bool SimpleGeodesicLineGraphBuilder::endCreateGrid()
{
    return true;
}

bool SimpleGeodesicLineGraphBuilder::isAtLineStart()
{
    return (i == 0);
}
