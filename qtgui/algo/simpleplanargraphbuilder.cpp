#include "simpleplanargraphbuilder.h"

#include <GeographicLib/Geodesic.hpp>

#include <QDebug>

using namespace displace::graphbuilders;

SimplePlanarGraphBuilder::SimplePlanarGraphBuilder(Type type, double lat_min_deg, double lon_min_deg, double lat_max_deg, double lon_max_deg, double step_x)
    : mLonMin(lon_min_deg * M_PI / 180.0), mLatMin(lat_min_deg * M_PI / 180.0),
      mLonMax(lon_max_deg * M_PI / 180.0), mLatMax(lat_max_deg * M_PI / 180.0),
      stepx(step_x)
{
    switch (type) {
    case Quad:
        fal = 0;
        break;
    case Hex:
        fal = 30;
        break;
    }
}

SimplePlanarGraphBuilder::~SimplePlanarGraphBuilder()
{

}

bool SimplePlanarGraphBuilder::beginCreateGrid()
{
    p1 = QPointF(mLonMin, mLatMin);
    clat = mLatMin;
    flon = mLonMin;
    linestart = true;
    nr = 0;

    qDebug() << "FAL: " << fal;

    return true;
}

QPointF SimplePlanarGraphBuilder::getNext()
{
    QPointF pt = QPointF(p1.x() * 180.0 / M_PI, p1.y() * 180.0 / M_PI);

    linestart = false;
    pointSumWithBearing(p1, stepx, M_PI_2, p2);
    p1 = p2;

    if (p1.x() > mLonMax) {
        ++nr;

        if ((nr % 2) == 1) {
            pointSumWithBearing(QPointF(flon, clat), stepx, -fal * M_PI / 180, p2);
        } else {
            pointSumWithBearing(QPointF(flon, clat), stepx, fal * M_PI / 180, p2);
        }

        clat = p2.y();
        flon = p2.x();
        p1=p2;
        linestart = true;
    }

    return pt;
}

bool SimplePlanarGraphBuilder::atEnd()
{
    return (clat > mLatMax);
}

bool SimplePlanarGraphBuilder::isAtLineStart()
{
    return linestart;
}

bool SimplePlanarGraphBuilder::endCreateGrid()
{
    return true;
}

void SimplePlanarGraphBuilder::pointSumWithBearing(const QPointF &p1, double dist, double bearing, QPointF &p2)
{
#if GEOGRAPHICLIB_VERSION_MINOR > 25
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

    double x,y;
    geod.Direct(p1.y()/ M_PI * 180.0, p1.x()/ M_PI * 180.0, bearing / M_PI * 180.0, dist, y, x);

    p2.setX(x* M_PI / 180.0);
    p2.setY(y* M_PI / 180.0);
}

