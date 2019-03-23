// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "simpleplanargraphbuilder.h"

#include <cmath>
#include <m_constants.h>


#include <GeographicLib/Geodesic.hpp>

#include <QDebug>

using namespace displace::graphbuilders;

SimplePlanarGraphBuilder::SimplePlanarGraphBuilder(Type type, double lat_min_deg, double lon_min_deg, double lat_max_deg, double lon_max_deg, double step)
    : mType(type),
      mLonMin(lon_min_deg * M_PI / 180.0), mLatMin(lat_min_deg * M_PI / 180.0),
      mLonMax(lon_max_deg * M_PI / 180.0), mLatMax(lat_max_deg * M_PI / 180.0)
{
    switch (mType) {
    case Quad:
        stepx = step;
        stepy = step;
        incr = 0;
        break;
    case Hex:
        stepx = step/2;
        stepy = std::sqrt(3) * step / 2;
        incr = 1;
        break;
    }
}

SimplePlanarGraphBuilder::~SimplePlanarGraphBuilder()
{

}

bool SimplePlanarGraphBuilder::beginCreateGrid()
{
    clat = mLatMin;
    linestart = true;
    nr = nc = 0;

    double l = mLonMin;
    p1 = p2 = QPointF(mLonMin, mLatMin);
    while (l < mLonMax) {
        mLongitudes.push_back(p2.x());

//        qDebug() << p2.x();
        pointSumWithBearing(p1, stepx, M_PI_2, p2);
        p1 = p2;

        l = p2.x();
    }

    p1 = QPointF(mLonMin, mLatMin);

//    qDebug() << "Nodes: " << mLongitudes.size();

    return true;
}

QPointF SimplePlanarGraphBuilder::getNext()
{
    QPointF pt = QPointF(p1.x() * 180.0 / M_PI, p1.y() * 180.0 / M_PI);
//    qDebug() << nr << nc << pt << p1;

    linestart = false;

    ++nc;
    if (mType == Hex) {
        ++nc;
    }

    if (nc >= mLongitudes.size()) {
        ++nr;
        nc = 0;
        if (mType == Hex) {
            if ((nr % 2) == 1) {
                ++nc;
            }
        }

        pointSumWithBearing(QPointF(mLongitudes[0], clat), stepy, 0, p2);

        clat = p2.y();
        linestart = true;
    }

    p1.setX(mLongitudes[nc]);
    p1.setY(clat);

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

