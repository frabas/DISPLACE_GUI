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

namespace GraphBuilderInternal {

struct LinkData {
    bool removed;
    double weight;
    int id_from, id_to;
};

bool sortCriteria(const LinkData &l1, const LinkData &l2) {
    return l1.weight < l2.weight;
}

}

QList<GraphBuilder::Node> GraphBuilder::buildGraph()
{
    QList<Node> res;
    CDT d;

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

    if (mFeedback) {
        mFeedback->setMax((mLatMax - mLatMin) / (stepy /earthRadius));
    }

    std::vector<std::shared_ptr<OGRDataSource> > include, exclude;

    if (mShapefileExc.get())
        exclude.push_back(mShapefileExc);
    if (mShapefileInc1.get()) {
        include.push_back(mShapefileInc1);
        fillWithNodes(res, d, mStep1, fal, include, exclude, false);
        include.clear();
        exclude.push_back(mShapefileInc1);
    }
    if (mShapefileInc2.get()) {
        include.push_back(mShapefileInc2);
        fillWithNodes(res, d, mStep2, fal, include, exclude, false);
        include.clear();
        exclude.push_back(mShapefileInc2);
    }
    if (mOutsideEnabled) {
        fillWithNodes(res, d, mStep, 30, include, exclude, true);
    }

    qDebug() << "Vert: " << d.number_of_vertices() << " Faces: " << d.number_of_faces();
    qDebug() << "Removing nodes based on these parameters: " << mLinkLimits << mMinLinks << mMaxLinks;

    int nv = 0, na =0;
    CDT::Finite_vertices_iterator vrt = d.finite_vertices_begin();
    while (vrt != d.finite_vertices_end()) {
        CDT::Vertex_circulator vc = d.incident_vertices((CDT::Vertex_handle)vrt);
        CDT::Vertex_circulator done(vc);

        std::vector<GraphBuilderInternal::LinkData> links;

        do {
            if (vc != d.infinite_vertex()) {
                QPointF p1 = res[vrt->info()].point;
                QPointF p2 = res[vc->info()].point;

                GraphBuilderInternal::LinkData ld;
                ld.id_from = vrt->info();
                ld.id_to = vc->info();
                ld.removed = false;

                if (mOutsideEnabled && mRemoveEdgesInExcludeZone) {
                    // look for edges intersecting the exclusion Zone
                    OGRLineString *line = (OGRLineString *)OGRGeometryFactory::createGeometry(wkbLineString);
                    line->addPoint(p1.x(), p1.y());
                    line->addPoint(p2.x(), p2.y());

                    for (int lr = 0; lr < mShapefileExc->GetLayerCount() && !ld.removed; ++lr) {
                        OGRLayer *layer = mShapefileExc->GetLayer(lr);
                        layer->ResetReading();
                        layer->SetSpatialFilter(line); //getting only the feature intercepting the point

                        if (layer->GetNextFeature() != 0) { // found, point is included, skip this check.
                            ld.removed = true;
                            break;
                        }
                    }

                    OGRGeometryFactory::destroyGeometry(line);
                }

                double d;

#if GEOGRAPHICLIB_VERSION_MINOR > 25
                const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
                const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

                geod.Inverse(p1.y(), p1.x(), p2.y(), p2.x(), d);
                ld.weight = d;

                links.push_back(ld);
            }
            ++vc;
            ++na;
        } while (vc != done);

        // add links

        std::sort(links.begin(), links.end(), GraphBuilderInternal::sortCriteria);

        int i = 0;
        for (std::vector<GraphBuilderInternal::LinkData>::iterator it = links.begin(); it != links.end(); ++it) {
            if (mLinkLimits > 1e-3 && it->weight > mLinkLimits)  // remove the link longer than the max lenght, if enabled
                it->removed = true;

            if (mMinLinks > 0 && i < mMinLinks)        // Keep a min number of links, if enabled
                it->removed = false;
            if (mMaxLinks > 0 && i >= mMaxLinks)        // Remove the links exceeding the max, if enabled
                it->removed = true;

            if (!it->removed) {
                ++i;
                res[it->id_from].adiacencies.push_back(it->id_to);
                res[it->id_from].weight.push_back(std::floor(it->weight / 1000 + 0.5));
            }
        }

        if (i < 4) {
            qDebug() << "Removed " << i << " from " << links.size();
            for (int j = 0; j < links.size(); ++j) {
                qDebug() << j << links[j].weight;
            }
        }

        ++vrt;
        ++nv;
    }

    // TODO: check that nodes are connected bidirectionally!

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

void GraphBuilder::fillWithNodes (QList<Node> &res, CDT &tri,
                                  double stepx, double fal, std::vector<std::shared_ptr<OGRDataSource> >including, std::vector<std::shared_ptr<OGRDataSource> > excluding, bool outside)
{
    double flon = mLonMin;
    int nr = 0, nc = 0;
    double lat = mLatMin;

    QPointF p1(mLonMin, mLatMin), p2;

    CDT::Vertex_handle lastHandle;

    while (lat <= mLatMax) {
        nc = 0;
        flon = p1.x();
        lastHandle = CDT::Vertex_handle();
        while (p1.x() <= mLonMax) {
            Node n;
            n.point = QPointF(p1.x() * 180.0 / M_PI, p1.y() * 180.0 / M_PI);
            n.good = true;

            int zone = (outside ? 0 : 3);       // if outside is not enabled, it is in the remove zone by default.

            OGRPoint point (n.point.x(), n.point.y());

            // Check for shapefile inclusion

            for (std::vector<std::shared_ptr<OGRDataSource> >::iterator it = including.begin(); it != including.end(); ++it) {
                for (int lr = 0; lr < (*it)->GetLayerCount(); ++lr) {
                    OGRLayer *layer = (*it)->GetLayer(lr);
                    layer->ResetReading();
                    layer->SetSpatialFilter(&point); //getting only the feature intercepting the point

                    if (layer->GetNextFeature() != 0) { // found, point is included, skip this check.
                        zone = 1;
                        break;
                    }
                }
            }

            for (std::vector<std::shared_ptr<OGRDataSource> >::iterator it = excluding.begin(); it != excluding.end(); ++it) {
                for (int lr = 0; lr < (*it)->GetLayerCount(); ++lr) {
                    OGRLayer *layer = (*it)->GetLayer(lr);
                    layer->ResetReading();
                    layer->SetSpatialFilter(&point); //getting only the feature intercepting the point

                    if (layer->GetNextFeature() != 0) {     // found: exclude this point
                        zone = 3;
                        break;
                    }
                }
            }

            if (zone == 3) {
                n.good = false;
                lastHandle = CDT::Vertex_handle();
            } else {
                CDT::Point pt(n.point.x(), n.point.y());

                CDT::Face_handle hint;
                CDT::Vertex_handle v_hint = tri.insert(pt, hint);
                if (v_hint!=CDT::Vertex_handle()){
                    v_hint->info()=res.size();
                    hint=v_hint->face();

                    if (lastHandle != CDT::Vertex_handle())
                        tri.insert_constraint(lastHandle, v_hint);
                    lastHandle = v_hint;
                    res.push_back(n);
                }
            }

            pointSumWithBearing(p1, stepx, M_PI_2, p2);
            p1 = p2;

            ++nc;
        }

        if ((nr % 2) == 1) {
            pointSumWithBearing(QPointF(flon, lat), stepx, -fal * M_PI / 180, p2);
        } else {
            pointSumWithBearing(QPointF(flon, lat), stepx, fal * M_PI / 180, p2);
        }

        lat = p2.y();
        ++nr;
        p1=p2;

        if (mFeedback)
            mFeedback->setStep(nr);
    }

}

void GraphBuilder::pushAd(QList<GraphBuilder::Node> &nodes, int source, int target)
{

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

    if (mLinkLimits < 1e-3 || d < mLinkLimits) {
        nodes[source].adiacencies.push_back(target);
        nodes[source].weight.push_back(std::floor(d / 1000 + 0.5));
    }
}

bool GraphBuilder::outsideEnabled() const
{
    return mOutsideEnabled;
}

void GraphBuilder::setOutsideEnabled(bool outsideEnabled)
{
    mOutsideEnabled = outsideEnabled;
}

void GraphBuilder::setLinkLimits(double limit_km)
{
    mLinkLimits = limit_km * 1000;
}

