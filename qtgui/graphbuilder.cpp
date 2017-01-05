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

#include "graphbuilder.h"

#include <cmath>
#include <m_constants.h>

#include <QDebug>

#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/GeodesicLine.hpp>
#include <GeographicLib/Constants.hpp>

#include <algo/geographicgridbuilder.h>
#include <algo/simplegeodesiclinegraphbuilder.h>
#include <algo/simpleplanargraphbuilder.h>

using namespace deprecated;

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
    mLatMin = std::min(latMin, latMax);//  * M_PI / 180.0;
    mLatMax = std::max(latMin, latMax); // * M_PI / 180.0;
    mLonMin = std::min(lonMin, lonMax);// * M_PI / 180.0;
    mLonMax = std::max(lonMin, lonMax);// * M_PI / 180.0;
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
    bool used;
    double weight;
    int id_from, id_to;

    LinkData()
        : removed(false), used(false), weight(0.0),
          id_from(-1), id_to(-1) {
    }
};

bool sortCriteria(const LinkData &l1, const LinkData &l2) {
    return l1.weight < l2.weight;
}

}

QList<GraphBuilder::Node> GraphBuilder::buildGraph()
{
    QList<Node> res;
    CDT d;


    // sanitize
    if (mShapefileExc.get() == 0)
        mRemoveEdgesInExcludeZone = false;

    qDebug() << "Type: " << mType;

    displace::graphbuilders::GeographicGridBuilder *builderInc1, *builderInc2, *builderOut;
    switch (mType) {
    case Hex:
        builderInc1 = new displace::graphbuilders::SimpleGeodesicLineGraphBuilder(
                    displace::graphbuilders::SimpleGeodesicLineGraphBuilder::Hex, mLatMin, mLonMin, mLatMax, mLonMax, mStep1);
        builderInc2 = new displace::graphbuilders::SimpleGeodesicLineGraphBuilder(
                    displace::graphbuilders::SimpleGeodesicLineGraphBuilder::Hex, mLatMin, mLonMin, mLatMax, mLonMax, mStep2);
        builderOut = new displace::graphbuilders::SimpleGeodesicLineGraphBuilder(
                    displace::graphbuilders::SimpleGeodesicLineGraphBuilder::Hex, mLatMin, mLonMin, mLatMax, mLonMax, mStep);
        break;
    case Quad:
        builderInc1 = new displace::graphbuilders::SimpleGeodesicLineGraphBuilder(
                    displace::graphbuilders::SimpleGeodesicLineGraphBuilder::Quad, mLatMin, mLonMin, mLatMax, mLonMax, mStep1);
        builderInc2 = new displace::graphbuilders::SimpleGeodesicLineGraphBuilder(
                    displace::graphbuilders::SimpleGeodesicLineGraphBuilder::Quad, mLatMin, mLonMin, mLatMax, mLonMax, mStep2);
        builderOut = new displace::graphbuilders::SimpleGeodesicLineGraphBuilder(
                    displace::graphbuilders::SimpleGeodesicLineGraphBuilder::Quad, mLatMin, mLonMin, mLatMax, mLonMax, mStep);
        break;
    case HexTrivial:
        builderInc1 = new displace::graphbuilders::SimplePlanarGraphBuilder(
                    displace::graphbuilders::SimplePlanarGraphBuilder::Hex, mLatMin, mLonMin, mLatMax, mLonMax, mStep1);
        builderInc2 = new displace::graphbuilders::SimplePlanarGraphBuilder(
                    displace::graphbuilders::SimplePlanarGraphBuilder::Hex, mLatMin, mLonMin, mLatMax, mLonMax, mStep2);
        builderOut = new displace::graphbuilders::SimplePlanarGraphBuilder(
                    displace::graphbuilders::SimplePlanarGraphBuilder::Hex, mLatMin, mLonMin, mLatMax, mLonMax, mStep);
        break;
    case QuadTrivial:
        builderInc1 = new displace::graphbuilders::SimplePlanarGraphBuilder(
                    displace::graphbuilders::SimplePlanarGraphBuilder::Quad, mLatMin, mLonMin, mLatMax, mLonMax, mStep1);
        builderInc2 = new displace::graphbuilders::SimplePlanarGraphBuilder(
                    displace::graphbuilders::SimplePlanarGraphBuilder::Quad, mLatMin, mLonMin, mLatMax, mLonMax, mStep2);
        builderOut = new displace::graphbuilders::SimplePlanarGraphBuilder(
                    displace::graphbuilders::SimplePlanarGraphBuilder::Quad, mLatMin, mLonMin, mLatMax, mLonMax, mStep);
        break;
    }

    int total = 0;

    if (mShapefileInc1.get()) {
        total += ((mLatMax* M_PI / 180.0 - mLatMin* M_PI / 180.0) / (mStep1 /earthRadius));
    }
    if (mShapefileInc2.get()) {
        total += ((mLatMax* M_PI / 180.0 - mLatMin* M_PI / 180.0) / (mStep2 /earthRadius));
    }
    if (mOutsideEnabled) {
        total += ((mLatMax* M_PI / 180.0 - mLatMin* M_PI / 180.0) / (mStep /earthRadius));
    }

    total += 50; // Node creation, links, removing.

    if (mFeedback) {
        mFeedback->setMax(total);
    }

    std::vector<std::shared_ptr<OGRDataSource> > include, exclude;

    int progress = 0;

    if (mShapefileExc.get())
        exclude.push_back(mShapefileExc);
    if (mShapefileInc1.get()) {
        include.push_back(mShapefileInc1);
        fillWithNodes(builderInc1, res, d, include, exclude, false, progress);
        include.clear();
        exclude.push_back(mShapefileInc1);
    }
    if (mShapefileInc2.get()) {
        include.push_back(mShapefileInc2);
        fillWithNodes(builderInc2, res, d, include, exclude, false, progress);
        include.clear();
        exclude.push_back(mShapefileInc2);
    }
    if (mOutsideEnabled) {
        fillWithNodes(builderOut, res, d, include, exclude, true, progress);
    }

    qDebug() << "Vert: " << d.number_of_vertices() << " Faces: " << d.number_of_faces();
    qDebug() << "Removing nodes based on these parameters: " << mLinkLimits << mMinLinks << mMaxLinks;

    int nv = 0, na =0;
    CDT::Finite_vertices_iterator vrt = d.finite_vertices_begin();
    int p3 = 0;
    int vt = d.number_of_vertices();
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

                if (mRemoveEdgesInExcludeZone) {
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
                if (mLinkLimits > 1e-3 && ld.weight > mLinkLimits)  // remove the link longer than the max lenght, if enabled
                    ld.removed = true;

                links.push_back(ld);
            }
            ++vc;
            ++na;
        } while (vc != done);

        // add links

        std::sort(links.begin(), links.end(), GraphBuilderInternal::sortCriteria);

        int nAdded = 0;
        // first add the
        for (std::vector<GraphBuilderInternal::LinkData>::iterator it = links.begin(); it != links.end(); ++it) {
            if (mMaxLinks > 0 && nAdded >= mMaxLinks)
                break;

            if (!it->removed) {
                ++nAdded;
                res[it->id_from].adiacencies.push_back(it->id_to);
                res[it->id_from].weight.push_back(std::floor(it->weight / 1000 + 0.5));
                it->used = true;
            }
        }
        for (std::vector<GraphBuilderInternal::LinkData>::iterator it = links.begin(); it != links.end(); ++it) {
            if (mMaxLinks > 0 && nAdded >= mMaxLinks)
                break;

            if (it->used)
                continue;

            if (mMinLinks > 0 && nAdded < mMinLinks)        // Keep a min number of links, if enabled
                it->removed = false;
            if (mMaxLinks > 0 && nAdded >= mMaxLinks)        // Remove the links exceeding the max, if enabled
                it->removed = true;

            if (!it->removed) {
                ++nAdded;
                res[it->id_from].adiacencies.push_back(it->id_to);
                res[it->id_from].weight.push_back(std::floor(it->weight / 1000 + 0.5));
            }
        }

#if 0   // DEBUG
        if (nAdded < 4) {
            qDebug() << "Added " << nAdded << " nodes from " << links.size() << "total.";
            for (int j = 0; j < links.size(); ++j) {
                if (links[j].used) {
                    qDebug() << j << links[j].weight << " Ok";
                }
            }
            for (int j = 0; j < links.size(); ++j) {
                if (!links[j].used) {
                    qDebug() << j << links[j].weight << " REMOVED";
                }
            }
        }
#endif

        ++vrt;
        ++nv;

        ++p3;
        if (p3 > vt/50) {
            ++progress;
            if (mFeedback)
                mFeedback->setStep(progress);
            p3 = 0;
        }

    }

    // check that nodes are connected bidirectionally!
    int bidi = 0;
    for (int i = 0; i < res.size(); ++i) {
        for (int j = 0; j < res[i].adiacencies.size(); ++j) {
            int adn = res[i].adiacencies[j];
            bool fnd = false;
            for (int k = 0; k < res[adn].adiacencies.size(); ++k) {
                if (res[adn].adiacencies[k] == i) {
                    fnd = true;
                    break;
                }
            }

            if (!fnd) {
                res[adn].adiacencies.push_back(i);
                res[adn].weight.push_back(res[i].weight[j]);
                ++bidi;
            }
        }
    }

    qDebug() << "NV:" << nv << "na: " << na << " non bidi: " << bidi;
    return res;
}

void GraphBuilder::fillWithNodes (displace::graphbuilders::GeographicGridBuilder *builder, QList<Node> &res, CDT &tri,
                                  std::vector<std::shared_ptr<OGRDataSource> >including, std::vector<std::shared_ptr<OGRDataSource> > excluding, bool outside, int &progress)
{
    int nr = 0, nc = 0;

    CDT::Vertex_handle lastHandle;


    builder->beginCreateGrid();
    while (!builder->atEnd()) {
        if (builder->isAtLineStart())
            lastHandle = CDT::Vertex_handle();

        Node n;
        n.point = builder->getNext();
        n.good = true;

        int zone = (outside ? 0 : 3);       // if outside is not enabled, it is in the remove zone by default.

        OGRPoint point (n.point.x(), n.point.y());

        // Check for shapefile inclusion

        for (std::vector<std::shared_ptr<OGRDataSource> >::iterator it = including.begin(); it != including.end() && zone != 1; ++it) {
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

        for (std::vector<std::shared_ptr<OGRDataSource> >::iterator it = excluding.begin(); it != excluding.end() && zone != 3; ++it) {
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

        ++nc;

        if (builder->isAtLineStart()) {
            ++nr;
            ++progress;

            if (mFeedback)
                mFeedback->setStep(progress);
        }
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

