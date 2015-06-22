#include "simplegeodesiclinegraphbuilder.h"

#include <GeographicLib/GeodesicLine.hpp>

#include <QPointF>

#include <cmath>

using namespace displace::graphbuilders;

SimpleGeodesicLineGraphBuilder::SimpleGeodesicLineGraphBuilder(double latMin_deg, double lonMin_deg, double latMax_deg, double lonMax_deg, double step)
    : mGeodesic(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f()),
      mLatMin(latMin_deg), mLonMin(lonMin_deg),
      mLatMax(latMax_deg), mLonMax(lonMax_deg),
      mStep(step)
{

}

bool SimpleGeodesicLineGraphBuilder::beginCreateGrid()
{
    a12_y = mGeodesic.Inverse(mLatMin, mLonMin, mLatMax, mLonMin, s12_y, azi1_y, azi2_y);
    num_y = int(std::ceil(s12_y / mStep)); // The number of intervals
    da_y = a12_y / num_y;

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

        a12 = mGeodesic.Inverse(ylat, mLonMin, ylat, mLonMax, s12, azi1, azi2);
        line = GeographicLib::GeodesicLine(mGeodesic, ylat, mLonMin, azi1);
        int num = int(std::ceil(s12 / mStep)); // The number of intervals
        da = a12 / num;
    }

    double plat, plon;
    line.ArcPosition(i * da, plat, plon);

    // HERE

    ++i;
    if (i > num) {
        i = 0;
        ++j;
    }

}

bool SimpleGeodesicLineGraphBuilder::atEnd()
{
    return (j >= num_y);
}

bool SimpleGeodesicLineGraphBuilder::endCreateGrid()
{
    return true;
}
