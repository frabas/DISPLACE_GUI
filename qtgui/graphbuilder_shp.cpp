// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2018 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "inputfileparser.h"

#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/GeodesicLine.hpp>
#include <GeographicLib/Constants.hpp>

#include <algo/geographicgridbuilder.h>
#include <algo/simplegeodesiclinegraphbuilder.h>
#include <algo/simpleplanargraphbuilder.h>

#include <QDir>
#include <QDebug>

#include <cmath>
#include <m_constants.h>

const double GraphBuilder::earthRadius = 6378137;   // ...


typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned, K>    Vb;

typedef CGAL::Constrained_triangulation_face_base_2<K>           Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>              TDS;
typedef CGAL::Exact_predicates_tag                               Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> CDT;

typedef CDT::Point                                             Point;


GraphBuilder::GraphBuilder()
    : mType(Hex),
      mStep(0), mStep1(0), mStep2(0),
      mLatMin(0), mLatMax(0),
      mLonMin(0), mLonMax(0),
      mShapefileInc1(), mShapefileInc2(), mShapefileExc(),
      mFeedback(0)
{
    mSpatialReference.SetWellKnownGeogCS( "WGS84" );
}

void GraphBuilder::setLimits(double lonMin, double lonMax, double latMin, double latMax)
{
    mLatMin = std::min(latMin, latMax);//  * M_PI / 180.0;
    mLatMax = std::max(latMin, latMax); // * M_PI / 180.0;
    mLonMin = std::min(lonMin, lonMax);// * M_PI / 180.0;
    mLonMax = std::max(lonMin, lonMax);// * M_PI / 180.0;
}

void GraphBuilder::setIncludingShapefile1(std::shared_ptr<GDALDataset> src)
{
    mShapefileInc1 = src;
}

void GraphBuilder::setIncludingShapefile2(std::shared_ptr<GDALDataset> src)
{
    mShapefileInc2 = src;
}

void GraphBuilder::setExcludingShapefile(std::shared_ptr<GDALDataset> src)
{
    mShapefileExc = src;
}

void GraphBuilder::createMainGrid(GDALDriver *memdriver, GDALDataset *outdataset, OGRLayer *&resultLayer)
{
    auto memdataset = memdriver->Create("memory", 0,0,0, GDT_Unknown, nullptr);

    auto builderInc1 = createBuilder(mType, mStep1);
    auto builderInc2 = createBuilder(mType, mStep2);
    auto builderOut = createBuilder(mType, mStep);

    OGRLayer *gridlayer1 = nullptr;
    OGRLayer *gridlayer2 = nullptr;
    OGRLayer *outlayer1 = nullptr, *outlayer2 = nullptr;
    OGRLayer *gridlayerOut = nullptr;

    OGRLayer *exclusionLayer1 = nullptr;
    OGRLayer *exclusionLayer2 = nullptr;
    OGRLayer *outLayerOut = nullptr;

    if (mShapefileInc1) {
        gridlayer1 = createGridLayer(outdataset, "grid1");
        auto inclusionLayer = mShapefileInc1->GetLayer(0);
        OGRLayer *exclusionLayer = nullptr;
        if (mShapefileInc2 && mStep2 < mStep1)
            exclusionLayer = mShapefileInc2->GetLayer(0);
        outlayer1 = createGridLayer(outdataset, "Displace-IncludeGrid1");
        createGrid(memdataset, builderInc1, outlayer1, gridlayer1, inclusionLayer, exclusionLayer, nullptr);
    }

    if (mShapefileInc2) {
        gridlayer2 = createGridLayer(outdataset, "grid2");
        auto inclusionLayer = mShapefileInc2->GetLayer(0);
        OGRLayer *exclusionLayer = nullptr;
        if (mShapefileInc1 && mStep1 < mStep2)
            exclusionLayer = mShapefileInc1->GetLayer(0);
        outlayer2 = createGridLayer(outdataset, "Displace-IncludeGrid2");
        createGrid(memdataset, builderInc2, outlayer2, gridlayer2, inclusionLayer, exclusionLayer, nullptr);
    }


    if (outsideEnabled()) {
        gridlayerOut = createGridLayer(outdataset, "gridOut");
        outLayerOut = createGridLayer(outdataset, "Displace-OutGrid");

        if (mShapefileInc1)
            exclusionLayer1 = mShapefileInc1->GetLayer(0);
        if (mShapefileInc2)
            exclusionLayer2 = mShapefileInc2->GetLayer(0);
        createGrid(memdataset, builderOut, outLayerOut, gridlayerOut, nullptr, exclusionLayer1, exclusionLayer2);

        if (mShapefileExc) {
            resultLayer = outdataset->CopyLayer(outLayerOut, "exc1", nullptr);
        } else {
            resultLayer = outdataset->CopyLayer(outLayerOut, "Displace-ResultGrid", nullptr);
        }
    } else {
        if (mShapefileExc) {
            resultLayer = createGridLayer(outdataset, "exc1");
        } else {
            resultLayer = createGridLayer(outdataset, "Displace-ResultGrid");
        }
    }

    if (outlayer1) {
        copyLayerContent(outlayer1, resultLayer);
    }
    if (outlayer2) {
        copyLayerContent(outlayer2, resultLayer);
    };

    if (mShapefileExc) {
        auto tempLayer = resultLayer;
        resultLayer = createGridLayer(outdataset, "Displace-ResultGrid");
        exclusionLayer1 = mShapefileExc->GetLayer(0);
        diff(tempLayer, exclusionLayer1, resultLayer, memdataset);
    }
}

void GraphBuilder::loadMainGrid(GDALDriver *memdriver, GDALDataset *memdataset, OGRLayer *&resultLayer)
{
    InputFileParser parser;
    QList<GraphBuilder::Node> nodes;
    QString error;
    if (parser.parseGraph(QString(), mLoadPath, nodes, &error)) {
        qDebug()  << nodes.size() << "Nodes loaded.";
    } else {
        throw std::runtime_error(QObject::tr("Error loading coords file: %1").arg(error).toStdString());
    }

    resultLayer = createGridLayer(memdataset, "Displace-ResultGrid");

    auto fId = getPointFieldIndex(resultLayer);
    auto fieldConstrain = resultLayer->FindFieldIndex("Constrain", true);
    if (fieldConstrain == -1) {
        OGRFieldDefn fldConstrain("Constrain", OFTInteger);
        resultLayer->CreateField(&fldConstrain);
        fieldConstrain = resultLayer->FindFieldIndex("Constrain", true);
    }

    for (auto &node : nodes) {
        OGRPoint pt(node.point.x(),node.point.y());
        auto f = OGRFeature::CreateFeature(resultLayer->GetLayerDefn());
        auto id = mId++;
        f->SetField(fId, id);
        f->SetField(fieldConstrain, (int)-1);

        f->SetGeometry(&pt);
        if (resultLayer->CreateFeature(f) != OGRERR_NONE) {
            throw std::runtime_error("Cannot create points");
        }
        OGRFeature::DestroyFeature(f);
    }
}

QList<GraphBuilder::Node> GraphBuilder::buildGraph()
{
    const QString OUTDIR = QString("%1/DisplaceBuildGraph").arg(QDir::tempPath());
    QDir toremove(OUTDIR);
    toremove.removeRecursively();
    toremove.mkpath(OUTDIR);

    // sanitize
    if (mShapefileExc.get() == 0)
        mRemoveEdgesInExcludeZone = false;

    int total = 0;

    if (mCreateMode) {
        if (mShapefileInc1.get()) {
            total += ((mLatMax * M_PI / 180.0 - mLatMin * M_PI / 180.0) / (mStep1 / earthRadius));
        }
        if (mShapefileInc2.get()) {
            total += ((mLatMax * M_PI / 180.0 - mLatMin * M_PI / 180.0) / (mStep2 / earthRadius));
        }
        if (mOutsideEnabled) {
            total += ((mLatMax * M_PI / 180.0 - mLatMin * M_PI / 180.0) / (mStep / earthRadius));
        }
    }

    total += 50; // Node creation, links, removing.

    if (mFeedback) {
        mFeedback->setMax(total);
    }

    progress = 0;

    // create the grid.

    // Create an in-memory db
    OGRSFDriverRegistrar *registrar = OGRSFDriverRegistrar::GetRegistrar();

    auto memdriver = registrar->GetDriverByName("memory"); // was memory

    auto outdriver = registrar->GetDriverByName("ESRI Shapefile");

    auto outdataset = outdriver->Create(OUTDIR.toStdString().c_str(), 0, 0, 0, GDT_Unknown, nullptr);

    ///
    OGRLayer *resultLayer = nullptr;

    try {
        if (mCreateMode) {
            createMainGrid(memdriver, outdataset, resultLayer);
        } else {
            loadMainGrid(memdriver, outdataset, resultLayer);
        }

        // Triangulate
        auto nIdField = getPointFieldIndex(resultLayer);

        long prevFid = -1;
        CDT::Vertex_handle prevHandle;
        auto fieldConstrain = resultLayer->FindFieldIndex("Constrain", true);
        assert(fieldConstrain != -1);

        CDT tri;
        OGRFeature *feature;
        resultLayer->ResetReading();

        while ((feature = resultLayer->GetNextFeature()) != nullptr) {
            const auto geometry(feature->GetGeometryRef());
            assert(geometry != nullptr);
            assert(wkbFlatten(geometry->getGeometryType()) == wkbPoint);

            const auto point(static_cast<OGRPoint *>(geometry));
            assert(point != nullptr);

            auto id = feature->GetFieldAsInteger(nIdField);
            auto constrFid = feature->GetFieldAsInteger(fieldConstrain);

            CDT::Point pt(point->getX(), point->getY());
            auto handle = tri.insert(pt);

            if (constrFid != -1 && constrFid == prevFid) {
                tri.insert_constraint(prevHandle, handle);
            }
            prevHandle = handle;
            prevFid = id;

            handle->info() = id;
            ++id;

            resultLayer->SetFeature(feature);
        }

        OGRLayer *layerEdges = createEdgesLayer(outdataset, "Displace-InEdges");
        auto fldFrom = getFromFieldIndex(layerEdges);
        assert(fldFrom != -1);
        auto fldTo = getToFieldIndex(layerEdges);
        assert(fldTo != -1);
        auto fldWeight = getWeightFieldIndex(layerEdges);
        assert(fldWeight != -1);

        const GeographicLib::Geodesic &geod = GeographicLib::Geodesic::WGS84();

        qDebug() << "Triangulation: " << tri.number_of_vertices() << " Vertices ";
        CDT::Finite_vertices_iterator vrt = tri.finite_vertices_begin();
        while (vrt != tri.finite_vertices_end()) {
            CDT::Vertex_circulator vc = tri.incident_vertices((CDT::Vertex_handle) vrt);
            CDT::Vertex_circulator done(vc);

            const auto &pt = vrt->point();
            do {
                if (vc != tri.infinite_vertex()) {
                    const auto &pt2 = vc->point();

                    double d;
                    geod.Inverse(pt.y(), pt.x(), pt2.y(), pt2.x(), d);

                    if (mLinkLimits < 1e-3 || d < mLinkLimits) {
                        OGRLineString line;
                        line.addPoint(pt.x(), pt.y());
                        line.addPoint(pt2.x(), pt2.y());

                        OGRFeature *f = OGRFeature::CreateFeature(layerEdges->GetLayerDefn());
                        f->SetGeometry(&line);
                        f->SetField(fldFrom, static_cast<int>(vrt->info()));
                        f->SetField(fldTo, static_cast<int>(vc->info()));

                        f->SetField(fldWeight, std::floor(d / 1000 + 0.5));

                        layerEdges->CreateFeature(f);
                        OGRFeature::DestroyFeature(f);
                    }
                }
                ++vc;
            } while (vc != done);
            ++vrt;
        }

        // here remove the other Exclusion Grid

        int deletedField = -1;
        if (mShapefileExc) {
            auto tempLayer2 = layerEdges;

            layerEdges = createEdgesLayer(outdataset, "Displace-ResultEdges");
            OGRFieldDefn delFld("Deleted", OFTInteger);
            layerEdges->CreateField(&delFld);
            deletedField = layerEdges->FindFieldIndex("Deleted", TRUE);

            fldFrom = getFromFieldIndex(layerEdges);
            assert(fldFrom != -1);
            fldTo = getToFieldIndex(layerEdges);
            assert(fldTo != -1);
            fldWeight = getWeightFieldIndex(layerEdges);
            assert(fldWeight != -1);

//        diffEdges(tempLayer2, exclusionLayer1, layerEdges, memdataset);
            copyLayerContent(tempLayer2, layerEdges);

            auto exclusionLayer1 = mShapefileExc->GetLayer(0);
            exclusionLayer1->ResetReading();
            qDebug() << "Before removing features: " << layerEdges->GetFeatureCount();

            OGRFeature *feature;

            int n = 0;
            while ((feature = exclusionLayer1->GetNextFeature()) != nullptr) {
                OGRGeometry *geometry = feature->GetGeometryRef();
                layerEdges->ResetReading();
                layerEdges->SetSpatialFilter(geometry);
                OGRFeature *edgeF;
                while ((edgeF = layerEdges->GetNextFeature()) != nullptr) {
                    layerEdges->DeleteFeature(edgeF->GetFID());
                    //edgeF->SetField(deletedField, 1);
                    ++n;
                    //layerEdges->SetFeature(edgeF);
                }
            }

            qDebug() << "Removed: " << n;
        }

        // build the list of results
        QList<Node> res;
        resultLayer->ResetReading();
        while (auto f = resultLayer->GetNextFeature()) {
            auto pt = f->GetGeometryRef();
            if (pt != NULL && wkbFlatten(pt->getGeometryType()) == wkbPoint) {
                OGRPoint *point = (OGRPoint *) pt;
                Node n;
                n.point = QPointF(point->getX(), point->getY());
                n.good = true;

                int id = f->GetFieldAsInteger(nIdField);
                assert(id != -1);

                while (res.size() <= id)
                    res.push_back(Node());
                res[id] = n;
            }
        }

        // Fill the adiacencies.

        int nEdges = 0;
        layerEdges->SetSpatialFilter(nullptr);
        layerEdges->ResetReading();

        qDebug() << "Actual features: " << layerEdges->GetFeatureCount() << deletedField;
        while (auto f = layerEdges->GetNextFeature()) {
            /*
            if (deletedField != -1) {
                auto isDeleted = f->GetFieldAsInteger(deletedField);
                if (isDeleted != 0)
                    continue;   // Skip deleted fields.
            }*/

            auto from = static_cast<unsigned>(f->GetFieldAsInteger(fldFrom));
            auto to = static_cast<unsigned>(f->GetFieldAsInteger(fldTo));
            auto weight = f->GetFieldAsDouble(fldWeight);

            auto &adj = res[from].adiacencies;
            adj.push_back(to);
            res[from].weight.push_back(weight);
            ++nEdges;
        }

        qDebug() << "Results: " << res.size() << " of " << resultLayer->GetFeatureCount() << nEdges << " edges ";

        for (int i = 0; i < 10; ++i) {
            qDebug() << i << res[i].point << res[i].adiacencies << res[i].weight;
        }

        GDALClose(outdataset);
        return res;
    } catch (std::exception &ex) {
        GDALClose(outdataset);

        qCritical() << "Error: " << ex.what();

        return QList<GraphBuilder::Node>();
    }
}

void GraphBuilder::createGrid(GDALDataset *tempDatasource,
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

    auto fId = getPointFieldIndex(gridout);
    auto fieldConstrain = gridout->FindFieldIndex("Constrain", true);
    if (fieldConstrain == -1) {
        OGRFieldDefn fldConstrain("Constrain", OFTInteger);
        gridout->CreateField(&fldConstrain);
        fieldConstrain = gridout->FindFieldIndex("Constrain", true);
    }

    // First Include Grid
    long oldFid = -1;

    builder->beginCreateGrid();
    while (!builder->atEnd()) {
        Node n;
        n.point = builder->getNext();
        n.good = true;

        OGRPoint pt(n.point.x(),n.point.y());
        auto f = OGRFeature::CreateFeature(gridout->GetLayerDefn());
        auto id = mId++;
        f->SetField(fId, id);
        f->SetField(fieldConstrain, (int)oldFid);

        f->SetGeometry(&pt);
        if (gridout->CreateFeature(f) != OGRERR_NONE) {
            throw std::runtime_error("Cannot create points");
        }
        oldFid = id;
        OGRFeature::DestroyFeature(f);

        if (builder->isAtLineStart()) {
            oldFid = -1;
            ++progress;

            if (mFeedback)
                mFeedback->setStep(progress);
        }
    }

    if (lyIncluded) {
        if (lyExclusion1) {
            if (lyExclusion2) {
                auto tempOut1 = createGridLayer(tempDatasource, "temp1");
                clip (lyGrid, lyIncluded, tempOut1, tempDatasource);
                auto tempOut2 = createGridLayer(tempDatasource, "temp2");
                diff (tempOut1, lyExclusion1, tempOut2, tempDatasource);
                diff (tempOut2, lyExclusion2, lyOut, tempDatasource);
                deleteLayer(tempDatasource, tempOut1);
                deleteLayer(tempDatasource, tempOut2);
            } else {
                auto tempOut1 = createGridLayer(tempDatasource, "temp1");
                clip (lyGrid, lyIncluded, tempOut1, tempDatasource);
                diff (tempOut1, lyExclusion1, lyOut, tempDatasource);
                deleteLayer(tempDatasource, tempOut1);
            }
        } else  {
            clip (lyGrid, lyIncluded, lyOut, tempDatasource);
        }
    } else {
        if (lyExclusion1) {
            if (lyExclusion2) {
                auto tempOut1 = createGridLayer(tempDatasource, "temp1");
                diff (lyGrid, lyExclusion1, tempOut1, tempDatasource);
                diff (tempOut1, lyExclusion2, lyOut, tempDatasource);
                deleteLayer(tempDatasource, tempOut1);
            } else {
                diff (lyGrid, lyExclusion1, lyOut, tempDatasource);
            }
        } else {
            // Nothing to do, just return (lyGrid => lyOut)
        }
    }
}

void GraphBuilder::clip(OGRLayer *in, OGRLayer *feature, OGRLayer *out, GDALDataset *tempds)
{
    startNewPartProgress("Clipping");
    auto temp = createGridLayer(tempds, "tempclip");
    if (in->Clip(feature, temp, nullptr, waitfunc, (void *)this) != OGRERR_NONE) {
        throw std::runtime_error("Error clipping");
    }
    copyLayerContent(temp, out);
    deleteLayer(tempds, temp);
}

void GraphBuilder::diff(OGRLayer *in1, OGRLayer *in2, OGRLayer *out, GDALDataset *tempds)
{
    /*
#if defined (DEBUG)
    auto in1_copy = tempds->CopyLayer(in1, "copy");
    in1 = in1_copy;
#endif */

    startNewPartProgress("Clipping");
    auto temp = createGridLayer(tempds, "temp2");
    if (in1->Clip(in2, temp, nullptr, waitfunc, (void *)this) != OGRERR_NONE) {
        throw std::runtime_error("Error clipping");
    }
    startNewPartProgress("Removing Points");
    auto temp3 = createGridLayer(tempds, "temp3");
    if (in1->SymDifference(temp, temp3, nullptr, waitfunc, (void *)this)) {
        throw std::runtime_error("Error diffing");
    }
    copyLayerContent(temp3, out);
    deleteLayer(tempds, temp);
    deleteLayer(tempds, temp3);
}

void GraphBuilder::diffEdges(OGRLayer *in1, OGRLayer *in2, OGRLayer *out, GDALDataset *tempds)
{
    startNewPartProgress("Clipping");
    auto temp = createEdgesLayer(tempds, "temp2");
    if (in1->Clip(in2, temp, nullptr, waitfunc, (void *)this) != OGRERR_NONE) {
        throw std::runtime_error("Error clipping");
    }
    startNewPartProgress("Removing Points");
    auto temp3 = createEdgesLayer(tempds, "temp3");
    if (in1->SymDifference(temp, temp3, nullptr, waitfunc, (void *)this)) {
        throw std::runtime_error("Error diffing");
    }
    copyLayerContent(temp3, out);
    deleteLayer(tempds, temp);
    deleteLayer(tempds, temp3);
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

OGRLayer *GraphBuilder::createGridLayer(GDALDataset *datasource, const char *const name)
{
    auto resultLayer = datasource->CreateLayer(name, &mSpatialReference, wkbPoint, nullptr);
    OGRFieldDefn fldConstrain("Constrain", OFTInteger);
    resultLayer->CreateField(&fldConstrain);
    getPointFieldIndex(resultLayer);
    return resultLayer;
}

OGRLayer *GraphBuilder::createEdgesLayer(GDALDataset *datasource, const char *const name)
{
    OGRLayer *layer = datasource->CreateLayer(name, &mSpatialReference, wkbLineString, nullptr);
    OGRFieldDefn fromField( "FromFid", OFTInteger );
    OGRFieldDefn toField ("ToFid", OFTInteger );
    OGRFieldDefn weightField ("Weight", OFTReal );
    if( layer->CreateField( &fromField ) != OGRERR_NONE
            || layer->CreateField( &toField) != OGRERR_NONE
            || layer->CreateField( &weightField ) != OGRERR_NONE
            ) {
        throw std::runtime_error( "Creating Name field failed." );
    }

    return layer;
}

void GraphBuilder::deleteLayer(GDALDataset *src, OGRLayer *layer)
{
    for (int i = 0; i < src->GetLayerCount(); ++i) {
        if (src->GetLayer(i) == layer) {
            src->DeleteLayer(i);
            return;
        }
    }

    qDebug() << "Layer not found: " << layer->GetName();
}

int GraphBuilder::getFromFieldIndex(OGRLayer *layer)
{
    return layer->FindFieldIndex("FromFid", true);
}

int GraphBuilder::getToFieldIndex(OGRLayer *layer)
{
    return layer->FindFieldIndex("ToFid", true);
}

int GraphBuilder::getWeightFieldIndex(OGRLayer *layer)
{
    return layer->FindFieldIndex("Weight", true);
}

int GraphBuilder::getPointFieldIndex(OGRLayer *layer)
{
    const auto FieldName = "pointId";
    auto nIdField = layer->FindFieldIndex(FieldName, true);
    if (nIdField == -1) {
        OGRFieldDefn idField (FieldName, OFTInteger );
        if( layer->CreateField( &idField ) != OGRERR_NONE ) {
            throw std::runtime_error( "Creating Id field failed." );
        }
        nIdField = layer->FindFieldIndex(FieldName, true);
        assert(nIdField != -1);
    }
    return nIdField;
}

int GraphBuilder::waitfunc(double progress, const char *msg, void *thiz)
{
    Q_UNUSED(thiz);
    Q_UNUSED(msg);
    reinterpret_cast<GraphBuilder*>(thiz)->makePartProgress(progress);

    return TRUE;
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

void GraphBuilder::actionCreate()
{
    mCreateMode = true;
}

void GraphBuilder::actionLoad(QString path)
{
    mCreateMode = false;
    mLoadPath = path;
}
