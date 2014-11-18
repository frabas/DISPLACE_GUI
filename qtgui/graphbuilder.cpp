#include "graphbuilder.h"

#include <cmath>
#include <m_constants.h>

#include <QDebug>

#ifdef HAVE_GEOGRAPHICLIB
#include <GeographicLib/Geodesic.hpp>
#endif

const double GraphBuilder::earthRadius = 6378137;   // ...


GraphBuilder::GraphBuilder()
    : mType(Hex),
      mStep(0),
      mLatMin(0), mLatMax(0),
      mLonMin(0), mLonMax(0),
      mShapefile(),
      mFeedback(0)
{
}

void GraphBuilder::setLimits(double lonMin, double lonMax, double latMin, double latMax)
{
    mLatMin = std::min(latMin, latMax) * M_PI / 180.0;
    mLatMax = std::max(latMin, latMax) * M_PI / 180.0;
    mLonMin = std::min(lonMin, lonMax) * M_PI / 180.0;
    mLonMax = std::max(lonMin, lonMax) * M_PI / 180.0;
}

void GraphBuilder::setShapefile(std::shared_ptr<OGRDataSource> src)
{
    mShapefile = src;
}

QList<GraphBuilder::Node> GraphBuilder::buildGraph()
{
    QList<Node> res;

    double lat = mLatMin;
    double stepy;
    double stepx;
    double fal;

    switch (mType) {
    case Hex:
        stepy = std::sqrt(3) / 2.0 * mStep;
        stepx = mStep;
        fal = 30;
        break;
    case Quad:
        stepy = stepx = mStep;
        fal = 0;
        break;
    }

    QPointF p1(mLonMin, mLatMin), p2;

    QList<int> idx0, idx1, idx2;

    if (mFeedback) {
        mFeedback->setMax((mLatMax - mLatMin) / (stepy /earthRadius));
    }

    double flon = mLonMin;
    int nr = 0, nc = 0;
    while (lat <= mLatMax) {
        nc = 0;
        flon = p1.x();
        while (p1.x() <= mLonMax) {
            Node n;
            n.point = QPointF(p1.x() * 180.0 / M_PI, p1.y() * 180.0 / M_PI);
            n.good = true;

            if (mShapefile.get()) {
                OGRPoint point (n.point.x(), n.point.y());
                for (int lr = 0; lr < mShapefile->GetLayerCount(); ++lr) {
                    OGRLayer *layer = mShapefile->GetLayer(lr);
                    layer->SetSpatialFilter(&point); //getting only the feature intercepting the point

                    layer->ResetReading();
                    if (layer->GetNextFeature() != 0) {
                        n.good = false;
                        break;
                    }
                }
            }

            res.append(n);
            idx0.push_back(res.size()-1);

            pointSumWithBearing(p1, stepx, M_PI_2, p2);
            p1 = p2;

            ++nc;
        }

        createAdiacencies(res, idx2, idx1, idx0, nr-1);

        if ((nr % 2) == 1) {
            pointSumWithBearing(QPointF(flon, lat), mStep, -fal * M_PI / 180, p2);
        } else {
            pointSumWithBearing(QPointF(flon, lat), mStep, fal * M_PI / 180, p2);
        }

        lat = p2.y();
        ++nr;
        p1=p2;

        idx2 = idx1;
        idx1 = idx0;
        idx0.clear();

        if (mFeedback)
            mFeedback->setStep(nr);
    }

    createAdiacencies(res, idx2, idx1, idx0, nr-1);

    return res;
}

void GraphBuilder::pointSumWithBearing(const QPointF &p1, double dist, double bearing, QPointF &p2)
{

#ifdef HAVE_GEOGRAPHICLIB
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;

    double x,y;
    geod.Direct(p1.y()/ M_PI * 180.0, p1.x()/ M_PI * 180.0, bearing / M_PI * 180.0, dist, y, x);

    p2.setX(x* M_PI / 180.0);
    p2.setY(y* M_PI / 180.0);

#else
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
#endif
}

void GraphBuilder::createAdiacencies(QList<GraphBuilder::Node> &nodes, const QList<int> &pidx, const QList<int> &idx, const QList<int> &nidx, int row_index)
{
    for (int i = 0; i < idx.size(); ++i) {
        // current node is nodes[i]

        if (i > 0)
            pushAd(nodes, idx[i], idx[i-1]);     // left node
        if (i < idx.size()-1)
            pushAd(nodes, idx[i], idx[i+1]);     // right node

        if ((row_index % 2) == 0) {     // even
            if (i > 0 && i-1 < pidx.size())
                pushAd(nodes, idx[i], pidx[i-1]);
            if (i < pidx.size())
                pushAd(nodes, idx[i], pidx[i]);

            if (i > 0 && i-1 < nidx.size())
                pushAd(nodes, idx[i], nidx[i-1]);
            if (i < nidx.size())
                pushAd(nodes, idx[i], nidx[i]);
        } else {    // odd
            if (i < pidx.size())
                pushAd(nodes, idx[i], pidx[i]);
            if (i+1 < pidx.size())
                pushAd(nodes, idx[i], pidx[i+1]);

            if (i < nidx.size())
                pushAd(nodes, idx[i], nidx[i]);
            if (i+1 < nidx.size())
                pushAd(nodes, idx[i], nidx[i+1]);
        }
    }
}

void GraphBuilder::pushAd(QList<GraphBuilder::Node> &nodes, int source, int target)
{
    nodes[source].adiacencies.push_back(target);

#ifdef HAVE_GEOGRAPHICLIB
    double d;

    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
    geod.Inverse(nodes[source].point.y(), nodes[source].point.x(), nodes[target].point.y(), nodes[target].point.x(), d);

#else
    double ph1 = nodes[source].point.x() * M_PI / 180;
    double la1 = nodes[source].point.y() * M_PI / 180;
    double ph2 = nodes[target].point.x() * M_PI / 180;
    double la2 = nodes[target].point.y() * M_PI / 180;
    double dp = ph2 - ph1;
    double dl = la2 - la1;

    double a = std::sin(dp/2) * std::sin(dp/2) +
            std::cos(ph1) * std::cos(ph2) *
            std::sin(dl/2) * std::sin(dl/2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));

    double d = earthRadius * c;
#endif

    nodes[source].weight.push_back(d / 1000);
}
