#include "graphbuilder.h"

#include <cmath>
#include <m_constants.h>

#include <QDebug>

#ifdef HAVE_GEOGRAPHICLIB
#include <GeographicLib/Geodesic.hpp>
#endif

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <utility>

const double GraphBuilder::earthRadius = 6378137;   // ...


GraphBuilder::GraphBuilder()
    : mType(Hex),
      mStep(0), mStep1(0), mStep2(0),
      mLatMin(0), mLatMax(0),
      mLonMin(0), mLonMax(0),
      mShapefileInc1(), mShapefileInc2(), mShapefileExc(),
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

void GraphBuilder::setIncludingShapefile1(std::shared_ptr<OGRDataSource> src)
{
    mShapefileInc1 = src;
}

void GraphBuilder::setIncludingShapefile2(std::shared_ptr<OGRDataSource> src)
{
    mShapefileInc2 = src;
}

void GraphBuilder::setExcludingShapefile(std::shared_ptr<OGRDataSource> src)
{
    mShapefileExc = src;
}

QList<GraphBuilder::Node> GraphBuilder::buildGraph()
{
    QList<Node> res;

    double lat = mLatMin;
    double stepy;
    double stepx;
    double fal;

    typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
    typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned, K>    Vb;
    typedef CGAL::Triangulation_data_structure_2<Vb>                    Tds;
    typedef CGAL::Delaunay_triangulation_2<K, Tds>                      Delaunay;
    typedef Delaunay::Point                                             Point;

    Delaunay d;

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

    if (mFeedback) {
        mFeedback->setMax((mLatMax - mLatMin) / (stepy /earthRadius));
    }

    bool removePoint;
    double flon = mLonMin;
    double xs = 0.0, ys = 0.0;
    int nr = 0, nc = 0;
    int xi, yi;

    int s = std::floor(mStep / 100.0);
    int s1 = std::floor(mStep1 / 100.0);
    int s2 = std::floor(mStep2 / 100.0);

    std::vector<std::pair<Point,unsigned> > points;

    ys = 0.0;
    while (lat <= mLatMax) {
        xs = 0.0;
        nc = 0;
        flon = p1.x();
        while (p1.x() <= mLonMax) {
            Node n;
            n.point = QPointF(p1.x() * 180.0 / M_PI, p1.y() * 180.0 / M_PI);
            n.good = true;

            int zone = 0;

            OGRPoint point (n.point.x(), n.point.y());

            // Check for shapefile inclusion
            if (mShapefileInc1.get()) {
                for (int lr = 0; lr < mShapefileInc1->GetLayerCount(); ++lr) {
                    OGRLayer *layer = mShapefileInc1->GetLayer(lr);
                    layer->ResetReading();
                    layer->SetSpatialFilter(&point); //getting only the feature intercepting the point

                    if (layer->GetNextFeature() != 0) { // found, point is included, skip this check.
                        zone = 1;
                        break;
                    }
                }
            }

            if (mShapefileInc2.get()) {
                for (int lr = 0; lr < mShapefileInc2->GetLayerCount(); ++lr) {
                    OGRLayer *layer = mShapefileInc2->GetLayer(lr);
                    layer->ResetReading();
                    layer->SetSpatialFilter(&point); //getting only the feature intercepting the point

                    if (layer->GetNextFeature() != 0) { // found, point is included, skip this check.
                        zone = 2;
                        break;
                    }
                }
            }

            // Check for point exclusion
            if (mShapefileExc.get()) {
                for (int lr = 0; lr < mShapefileExc->GetLayerCount(); ++lr) {
                    OGRLayer *layer = mShapefileExc->GetLayer(lr);
                    layer->ResetReading();
                    layer->SetSpatialFilter(&point); //getting only the feature intercepting the point

                    if (layer->GetNextFeature() != 0) {     // found: exclude this point
                        zone = 3;
                        break;
                    }
                }
            }


            xi = static_cast<int>(xs / 100.0);
            yi = static_cast<int>(ys / 100.0);

            switch (zone) {
            case 0: // outside zone
                removePoint = ((xi % s) != 0) || ((yi % s) != 0);
                break;
            case 1: // Include Zone 1
                removePoint = ((xi % s1) != 0) || ((yi % s1) != 0);
                break;
            case 2: // Include Zone 2
                removePoint = ((xi % s2) != 0) || ((yi % s2) != 0);
                break;
            case 3: // Exclude Zone
                removePoint = true;
                break;
            }

            if (removePoint) {
                n.good = false;
            } else {
                Point pt (n.point.x(), n.point.y());
                points.push_back(std::make_pair(pt, res.size()));
                res.append(n);
            }

            pointSumWithBearing(p1, stepx, M_PI_2, p2);
            p1 = p2;

            xs += stepx;
            ++nc;
        }

        if ((nr % 2) == 1) {
            pointSumWithBearing(QPointF(flon, lat), mStep, -fal * M_PI / 180, p2);
        } else {
            pointSumWithBearing(QPointF(flon, lat), mStep, fal * M_PI / 180, p2);
        }

        lat = p2.y();
        ++nr;
        p1=p2;

        ys += stepx;

        if (mFeedback)
            mFeedback->setStep(nr);
    }

    qDebug() << "Inserted: " << points.size() << res.size();

    d.insert(points.begin(), points.end());

    qDebug() << "Vert: " << d.number_of_vertices() << " Faces: " << d.number_of_faces();

    int nv = 0, na =0;
    Delaunay::Finite_vertices_iterator vrt = d.finite_vertices_begin();
    while (vrt != d.finite_vertices_end()) {
        Delaunay::Vertex_circulator vc = d.incident_vertices((Delaunay::Vertex_handle)vrt);
        Delaunay::Vertex_circulator done(vc);

        do {
            if (vc != d.infinite_vertex()) {
                pushAd(res, vrt->info(), vc->info());
            }
            ++vc;
            ++na;
        } while (vc != done);

        ++vrt;
        ++nv;
    }


    qDebug() << "NV:" << nv << "na: " << na;
    return res;
}

void GraphBuilder::pointSumWithBearing(const QPointF &p1, double dist, double bearing, QPointF &p2)
{

#ifdef HAVE_GEOGRAPHICLIB
#if GEOGRAPHICLIB_VERSION_MINOR > 25
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

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

            if (mType == Quad) {
                if (i+1 < pidx.size())
                    pushAd(nodes, idx[i], pidx[i+1]);
                if (i+1 < nidx.size())
                    pushAd(nodes, idx[i], nidx[i+1]);
            }

        } else {    // odd
            if (i < pidx.size())
                pushAd(nodes, idx[i], pidx[i]);
            if (i+1 < pidx.size())
                pushAd(nodes, idx[i], pidx[i+1]);

            if (i < nidx.size())
                pushAd(nodes, idx[i], nidx[i]);
            if (i+1 < nidx.size())
                pushAd(nodes, idx[i], nidx[i+1]);

            if (mType == Quad) {
                if (i > 0 && i-1 < pidx.size())
                    pushAd(nodes, idx[i], pidx[i-1]);
                if (i > 0 && i-1 < nidx.size())
                    pushAd(nodes, idx[i], nidx[i-1]);
            }
        }
    }
}

void GraphBuilder::pushAd(QList<GraphBuilder::Node> &nodes, int source, int target)
{
    nodes[source].adiacencies.push_back(target);

#ifdef HAVE_GEOGRAPHICLIB
    double d;

#if GEOGRAPHICLIB_VERSION_MINOR > 25
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

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

    nodes[source].weight.push_back(std::floor(d / 1000 + 0.5));
}
