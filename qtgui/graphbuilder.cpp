#include "graphbuilder.h"

#include <cmath>
#include <m_constants.h>

#include <QDebug>

const double GraphBuilder::earthRadius = 6371000;   // ...


GraphBuilder::GraphBuilder()
{
}

void GraphBuilder::setLimits(double lonMin, double lonMax, double latMin, double latMax)
{
    mLatMin = std::min(latMin, latMax) * M_PI / 180.0;
    mLatMax = std::max(latMin, latMax) * M_PI / 180.0;
    mLonMin = std::min(lonMin, lonMax) * M_PI / 180.0;
    mLonMax = std::max(lonMin, lonMax) * M_PI / 180.0;
}

QList<QPointF> GraphBuilder::buildGraph()
{
    QList<QPointF> res;

    double lat = mLatMin;
    double latinc = std::sqrt(3) / 2.0 * mStep;

    QPointF p1(mLonMin, mLatMin), p2;

    int nr = 0;
    while (lat <= mLatMax) {
        if ((nr % 2) == 1) {
            pointSumWithBearing(QPointF(mLonMin, lat), mStep/2, M_PI_2, p1);
        } else {
            p1.setX(mLonMin);
            p1.setY(lat);
        }

        int nc = 0;
        while (p1.x() <= mLonMax) {
            res.append(QPointF(p1.x() * 180.0 / M_PI, p1.y() * 180.0 / M_PI));
            pointSumWithBearing(p1, mStep, M_PI_2, p2);
            p1 = p2;

            ++nc;
        }

        pointSumWithBearing(p1, latinc, 0, p2);
        lat = p2.y();
        ++nr;
    }

    return res;
}

void GraphBuilder::pointSumWithBearing(const QPointF &p1, double dist, double bearing, QPointF &p2)
{
    // φ Latitude λ Longitude d distance R earth radius [6371km], brng bearing (rad, north, clockwise)
    // var φ2 = Math.asin( Math.sin(φ1)*Math.cos(d/R) +
    //      Math.cos(φ1)*Math.sin(d/R)*Math.cos(brng) );
    // var λ2 = λ1 + Math.atan2(Math.sin(brng)*Math.sin(d/R)*Math.cos(φ1),
    //     Math.cos(d/R)-Math.sin(φ1)*Math.sin(φ2));

    p2.setY(std::asin (
                std::sin(p1.y()) * std::cos(dist / earthRadius) +
                std::cos(p1.y()) * std::sin(dist / earthRadius) * std::cos(bearing)));
    p2.setX(p1.x() +
            std::atan2( std::sin(bearing) * std::sin(dist/earthRadius) * std::cos(p1.y()),
                        std::cos(dist/earthRadius) - std::sin(p1.y()) * std::sin(p2.y()))
            );
}
