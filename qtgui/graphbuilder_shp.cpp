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
    OGRLayer *gridlayer1 = nullptr;
    OGRLayer *gridlayer2 = nullptr;
    OGRLayer *gridlayerOut = nullptr;

    OGRLayer *outlayer1 = nullptr, *outlayer2 = nullptr;

    if (mShapefileInc1) {
        gridlayer1 = outdataset->CreateLayer("grid1", &sr, wkbPoint, nullptr);
        auto inclusionLayer = mShapefileInc1->GetLayer(0);
        OGRLayer *exclusionLayer = nullptr;
        if (mShapefileInc2 && mStep1 < mStep2)
            exclusionLayer = mShapefileInc2->GetLayer(0);
        outlayer1 = outdataset->CreateLayer("Displace-IncludeGrid1", &sr, wkbPoint, nullptr);
        createGrid(memdataset, builderInc1, outlayer1, gridlayer1, inclusionLayer, exclusionLayer, nullptr);
    }

    if (mShapefileInc2) {
        gridlayer2 = memdataset->CreateLayer("grid2", &sr, wkbPoint, nullptr);
        auto inclusionLayer = mShapefileInc2->GetLayer(0);
        OGRLayer *exclusionLayer = nullptr;
        if (mShapefileInc1 && mStep2 < mStep1)
            exclusionLayer = mShapefileInc1->GetLayer(0);
        outlayer2 = outdataset->CreateLayer("Displace-IncludeGrid2", &sr, wkbPoint, nullptr);
        createGrid(memdataset, builderInc2, outlayer2, gridlayer2, inclusionLayer, exclusionLayer, nullptr);
    }

    gridlayerOut = memdataset->CreateLayer("gridOut", &sr, wkbPoint, nullptr);
    OGRLayer *exclusionLayer1 = nullptr, *exclusionLayer2 = nullptr;
    if (mShapefileInc1)
        exclusionLayer1 = mShapefileInc1->GetLayer(0);
    if (mShapefileInc2)
        exclusionLayer2 = mShapefileInc2->GetLayer(0);
    OGRLayer *outLayerOut = outdataset->CreateLayer("Displace-OutGrid", &sr, wkbPoint, nullptr);
    createGrid(memdataset, builderOut, outLayerOut, gridlayerOut, nullptr, exclusionLayer1, exclusionLayer2);

    OGRLayer *resultLayer;
    if (mShapefileExc) {
        resultLayer = memdataset->CopyLayer(outLayerOut, "temp", nullptr);
    } else {
        resultLayer = outdataset->CopyLayer(outLayerOut, "Displace-ResultGrid", nullptr);
    }

    if (outlayer1) {
        copyLayerContent(outlayer1, resultLayer);
    }
    if (outlayer2) {
        copyLayerContent(outlayer2, resultLayer);
    };


    // here remove the other Exclusion Grid

    if (mShapefileExc) {
        auto tempLayer = resultLayer;
        resultLayer = outdataset->CreateLayer("Displace-ResultGrid", &sr);
        exclusionLayer1 = mShapefileExc->GetLayer(0);
        diff(tempLayer, exclusionLayer1, resultLayer, memdataset);
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

void GraphBuilder::createGrid(OGRDataSource *tempDatasource,
                              std::shared_ptr<displace::graphbuilders::GeographicGridBuilder> builder,
                              OGRLayer *lyOut,
                              OGRLayer *lyGrid,
                              OGRLayer *lyIncluded,
                              OGRLayer *lyExclusion1,
                              OGRLayer *lyExclusion2
                              )
{
    OGRLayer *gridout = lyGrid;
    if (lyIncluded == nullptr && lyExclusion1 == nullptr && lyExclusion2 == nullptr)
        gridout = lyOut;

    // First Include Grid
    builder->beginCreateGrid();
    while (!builder->atEnd()) {
        Node n;
        n.point = builder->getNext();
        n.good = true;

        OGRPoint pt(n.point.x(),n.point.y());
        auto f = OGRFeature::CreateFeature(gridout->GetLayerDefn());
        f->SetGeometry(&pt);
        if (gridout->CreateFeature(f) != OGRERR_NONE) {
            throw std::runtime_error("Cannot create points");
        }
        OGRFeature::DestroyFeature(f);

        if (builder->isAtLineStart()) {
            ++progress;

            if (mFeedback)
                mFeedback->setStep(progress);
        }
    }

    if (lyIncluded) {
        if (lyExclusion1) {
            if (lyExclusion2) {
                auto tempOut1 = tempDatasource->CreateLayer("temp1");
                clip (lyGrid, lyIncluded, tempOut1);
                auto tempOut2 = tempDatasource->CreateLayer("temp1");
                diff (tempOut1, lyExclusion1, tempOut2, tempDatasource);
                diff (tempOut2, lyExclusion2, lyOut, tempDatasource);
            } else {
                auto tempOut1 = tempDatasource->CreateLayer("temp1");
                clip (lyGrid, lyIncluded, tempOut1);
                diff (tempOut1, lyExclusion1, lyOut, tempDatasource);
            }
        } else  {
            clip (lyGrid, lyIncluded, lyOut);
        }
    } else {
        if (lyExclusion1) {
            if (lyExclusion2) {
                auto tempOut1 = tempDatasource->CreateLayer("temp1");
                diff (lyGrid, lyExclusion1, tempOut1, tempDatasource);
                diff (tempOut1, lyExclusion2, lyOut, tempDatasource);
            } else {
                diff (lyGrid, lyExclusion1, lyOut, tempDatasource);
            }
        } else {
            // Nothing to do, just return (lyGrid => lyOut)
        }
    }
}

void GraphBuilder::clip (OGRLayer *in, OGRLayer *feature, OGRLayer *out)
{
    startNewPartProgress("Clipping");
    if (in->Clip(feature, out, nullptr, waitfunc, (void *)this) != OGRERR_NONE) {
        throw std::runtime_error("Error clipping");
    }
}

void GraphBuilder::diff (OGRLayer *in1, OGRLayer *in2, OGRLayer *out, OGRDataSource *tempds)
{
#if defined (DEBUG)
    auto in1_copy = tempds->CopyLayer(in1, "copy");
    in1 = in1_copy;
#endif

    startNewPartProgress("Clipping");
    auto temp = tempds->CreateLayer("temp", nullptr, wkbPoint, nullptr);
    if (in1->Clip(in2, temp, nullptr, waitfunc, (void *)this) != OGRERR_NONE) {
        throw std::runtime_error("Error clipping");
    }
    startNewPartProgress("Removing Points");
    if (in1->SymDifference(temp, out, nullptr, waitfunc, (void *)this)) {
        throw std::runtime_error("Error diffing");
    }
}

void GraphBuilder::copyLayerContent(OGRLayer *src, OGRLayer *dst)
{
    OGRFeature *f ;
    while ((f = src->GetNextFeature()) != nullptr) {
        dst->CreateFeature(f);
    }
}

void GraphBuilder::makePartProgress(double x)
{
    if (mFeedback) {
        int pro = static_cast<int>(std::trunc(x*100 + 0.5));

        auto now = Timer::now();
        std::chrono::duration<double> diff = now - mFeedbackStartTime;
        auto detc = diff.count() / x;
        long ela = static_cast<long>(std::trunc(diff.count()+0.5));
        long etc = static_cast<long> (std::trunc(detc + 0.5)) - ela;

//        qDebug() << "Progress: " << x << diff.count() << detc << etc;

        if (pro != mFeedbackPreviousETC) {
            QString msg = QString("%1 (%2%) - ELA: %3 - ETC: %4").arg(mFeedbackProgressMsg)
                    .arg(pro)
                    .arg(ela)
                    .arg(etc);
            mFeedback->setPartMessage(msg);
            mFeedbackPreviousETC = etc;
        }
    }
}

void GraphBuilder::startNewPartProgress(QString msg)
{
    mFeedbackStartTime = Timer::now();
    mFeedbackProgressMsg = msg;
    mFeedbackPreviousETC = -1;
}

int GraphBuilder::waitfunc(double progress, const char *msg, void *thiz)
{
    Q_UNUSED(thiz);
    Q_UNUSED(msg);
    reinterpret_cast<GraphBuilder*>(thiz)->makePartProgress(progress);

    return TRUE;
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

