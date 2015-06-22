#ifndef SIMPLEGEODESICLINEGRAPHBUILDER_H
#define SIMPLEGEODESICLINEGRAPHBUILDER_H

#include <algo/geographicgridbuilder.h>

#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/GeodesicLine.hpp>

namespace  displace {
namespace graphbuilders {

/** \brief Implements a Simple Geodesic Grid
 *
 * */
class SimpleGeodesicLineGraphBuilder : public GeographicGridBuilder
{
public:
    SimpleGeodesicLineGraphBuilder(double latMin_deg, double lonMin_deg, double latMax_deg, double lonMax_deg, double step);

    // GeographicGridBuilder interface
public:
    bool beginCreateGrid();
    QPointF getNext();
    bool atEnd();
    bool endCreateGrid();

private:
    GeographicLib::Geodesic mGeodesic;

    // Parameters
    double mLatMin, mLonMin, mLatMax, mLonMax;
    double mStep;

    // outer loop
    double s12_y, azi1_y, azi2_y, a12_y, da_y;
    GeographicLib::GeodesicLine line_y;
    int num_y;

    // inner loop
    double s12, azi1, azi2, a12, da;
    GeographicLib::GeodesicLine line;
    int num;

    int j,i;
};

}
}

#endif // SIMPLEGEODESICLINEGRAPHBUILDER_H
