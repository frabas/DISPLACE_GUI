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

#include "graphbuilder_shp.h"

#include <cmath>
#include <m_constants.h>

#include <QDebug>

#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/GeodesicLine.hpp>
#include <GeographicLib/Constants.hpp>

#include <algo/geographicgridbuilder.h>
#include <algo/simplegeodesiclinegraphbuilder.h>
#include <algo/simpleplanargraphbuilder.h>

#include <QDir>

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

#if 0
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
#endif


QList<GraphBuilder::Node> GraphBuilder::buildGraph()
{
#ifdef DEBUG
    const QString OUTDIR = "/tmp/DisplaceBuildGraph";
    QDir toremove(OUTDIR);
    toremove.removeRecursively();
    toremove.mkpath(OUTDIR);
#endif

    OGRSpatialReference sr;
    sr.SetWellKnownGeogCS( "WGS84" );

    // sanitize
    if (mShapefileExc.get() == 0)
        mRemoveEdgesInExcludeZone = false;

    qDebug() << "Type: " << mType;

    auto builderInc1 = createBuilder(mType, mStep1);
    auto builderInc2 = createBuilder(mType, mStep2);
    auto builderOut = createBuilder(mType, mStep);

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

    progress = 0;

    // create the grid.

    // Create an in-memory db
    OGRSFDriverRegistrar *registrar =  OGRSFDriverRegistrar::GetRegistrar();

    auto memdriver = registrar->GetDriverByName("memory");
    auto memdataset = memdriver->CreateDataSource("memory", nullptr );

    auto outdriver = registrar->GetDriverByName("ESRI Shapefile");
    auto outdataset = outdriver->CreateDataSource(OUTDIR.toStdString().c_str(), nullptr );
    auto gridlayer1 = outdataset->CreateLayer("grid1", &sr, wkbPoint, nullptr);

    OGRLayer *outlayer = nullptr, *outlayer2 = nullptr;

    if (mShapefileInc1) {
        auto inclusionLayer = mShapefileInc1->GetLayer(0);
        outlayer = outdataset->CreateLayer("Displace-IncludeGrid1", &sr, wkbPoint, nullptr);
        createGrid(builderInc1, outlayer, gridlayer1, inclusionLayer);
    }

    auto gridlayer2 = memdataset->CreateLayer("grid2", &sr, wkbPoint, nullptr);
    if (mShapefileInc2) {
        auto inclusionLayer2 = mShapefileInc2->GetLayer(0);
        outlayer2 = outdataset->CreateLayer("Displace-IncludeGrid2", &sr, wkbPoint, nullptr);
        createGrid(builderInc2, outlayer2, gridlayer2, inclusionLayer2);
    }

    OGRLayer *resultLayer;

    if (outlayer2) {
        resultLayer = outdataset->CreateLayer("Displace-ResultGrid", &sr, wkbPoint, nullptr);
        outlayer->Union(outlayer2, resultLayer, nullptr, nullptr, nullptr);
    } else {
        resultLayer = outdataset->CopyLayer(outlayer, "Displace-ResultGrid", nullptr);
    }

    // build the list of results
    QList<Node> res;
#if 1
    resultLayer->ResetReading();
    while (auto f = resultLayer->GetNextFeature()) {
        auto pt = f->GetGeometryRef();
        if( pt != NULL && wkbFlatten(pt->getGeometryType()) == wkbPoint )
        {
            OGRPoint *point = (OGRPoint *) pt;
            Node n;
            n.point = QPoint(point->getX(), point->getY());
            n.good = true;
            res << n;
        }
    }

    qDebug() << "Results: " << res.size() << " of " << resultLayer->GetFeatureCount();
#endif

    OGRDataSource::DestroyDataSource(outdataset);
    return res;
}

void GraphBuilder::createGrid(std::shared_ptr<displace::graphbuilders::GeographicGridBuilder> builder,
                              OGRLayer *lyOut,
                              OGRLayer *lyGrid,
                              OGRLayer *lyIncluded
                              )
{
    // First Include Grid
    builder->beginCreateGrid();
    while (!builder->atEnd()) {
        Node n;
        n.point = builder->getNext();
        n.good = true;

        OGRPoint pt(n.point.x(),n.point.y());
        auto f = OGRFeature::CreateFeature(lyGrid->GetLayerDefn());
        f->SetGeometry(&pt);
        if (lyGrid->CreateFeature(f) != OGRERR_NONE) {
            throw std::runtime_error("Cannot create points");
        }
        OGRFeature::DestroyFeature(f);

#if 1
        if (builder->isAtLineStart()) {
            ++progress;

            if (mFeedback)
                mFeedback->setStep(progress);
        }
#endif
    }

    if (lyGrid->Clip(lyIncluded, lyOut, nullptr, nullptr, nullptr) != OGRERR_NONE) {
        throw std::runtime_error("Error clipping");
    }
}



#if 0
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
#endif


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

std::shared_ptr<displace::graphbuilders::GeographicGridBuilder> GraphBuilder::createBuilder(GraphBuilder::Type type, double step)
{
    switch (type) {
    case Hex:
        return std::make_shared<displace::graphbuilders::SimpleGeodesicLineGraphBuilder>(
                    displace::graphbuilders::SimpleGeodesicLineGraphBuilder::Hex, mLatMin, mLonMin, mLatMax, mLonMax, step);
    case Quad:
        return std::make_shared<displace::graphbuilders::SimpleGeodesicLineGraphBuilder>(
                    displace::graphbuilders::SimpleGeodesicLineGraphBuilder::Quad, mLatMin, mLonMin, mLatMax, mLonMax, step);
    case HexTrivial:
        return std::make_shared<displace::graphbuilders::SimplePlanarGraphBuilder>(
                    displace::graphbuilders::SimplePlanarGraphBuilder::Hex, mLatMin, mLonMin, mLatMax, mLonMax, step);
        break;
    case QuadTrivial:
        return std::make_shared<displace::graphbuilders::SimplePlanarGraphBuilder>(
                    displace::graphbuilders::SimplePlanarGraphBuilder::Quad, mLatMin, mLonMin, mLatMax, mLonMax, step);
    }

    throw std::runtime_error("Unhandled case");
}

