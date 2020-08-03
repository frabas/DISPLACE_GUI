// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "mapobjectscontroller.h"

#include <idtypes.h>
#include <displacemodel.h>
#include <mapobjects/harbourmapobject.h>
#include <mapobjects/nodemapobject.h>
#include <mapobjects/vesselmapobject.h>
#include <mapobjects/firmmapobject.h>
#include <mapobjects/shipmapobject.h>
#include <mapobjects/fishfarmobject.h>
#include <mapobjects/windmillobject.h>
#include <mapobjects/edgemapobject.h>
#include <mapobjects/edgelayer.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/MapAdapterOSM.h>
#include <QMapControl/Layer.h>
#include <QMapControl/MapAdapterOpenSeaMap.h>
#include <QMapControl/LayerGeometry.h>
#include <QMapControl/GeometryPointCircle.h>
#include <QMapControl/LayerMapAdapter.h>
#include <QMapControl/ImageManager.h>
#include <QMapControl/GeometryWidget.h>
#include <QMapControl/LayerESRIShapefile.h>

#include <QTextEdit>

#include <appsettings.h>

/* If defined, CONFIG_ALLOW_MULTIPLE_DETAILS_WINDOWS allows opening more than one Details window.
 */
#undef CONFIG_ALLOW_MULTIPLE_DETAILS_WINDOWS

MapObjectsController::MapObjectsController(qmapcontrol::QMapControl *map)
    : mMap(map),
      mModelVisibility(MainWindow::MAX_MODELS, false),
      mLayers(MainWindow::MAX_MODELS, LayerListImpl(LayerMax)),
      mEnvLayers(MainWindow::MAX_MODELS, LayerListImpl(EnvLayerMax)),
      mOutputLayers(MainWindow::MAX_MODELS, LayerListImpl(OutLayerMax)),
      mTariffsLayers(MainWindow::MAX_MODELS, LayerListImpl(TariffLayerMax)),
      mShapefiles(MainWindow::MAX_MODELS, QList<std::shared_ptr<GDALDataset> >()),
      mShapefileLayers(MainWindow::MAX_MODELS, LayerVarListImpl<qmapcontrol::LayerESRIShapefile>()),
      mEditorMode(NoEditorMode),
      mClosing(false)
{
    // create mapadapter, for mainlayer and overlay
    mMainMapAdapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOSM());
    mSeamarkAdapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOpenSeaMap());

    // create a layer with the mapadapter and type MapLayer
    mMainLayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("OpenStreetMap", mMainMapAdapter));
    mSeamarkLayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("Seamark", mSeamarkAdapter));
    mWidgetLayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Details"));
    mEditorLayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Editor"));
    mEditorLayer->setVisible(true);

    mMap->addLayer(mMainLayer);
    mMap->addLayer(mSeamarkLayer);
    mMap->addLayer(mWidgetLayer);
    mMap->addLayer(mEditorLayer);

    displace::AppSettings appsettings;
    auto center = appsettings.getMapCenterPoint();
    mMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(center.x(), center.y()));
    mMap->setZoom(appsettings.getMapZoom());

    connect (mMap, SIGNAL(geometryClicked(const Geometry*)), this, SLOT(geometryClicked(const Geometry*)));
}

void MapObjectsController::setModel(int model_n, std::shared_ptr<DisplaceModel> model)
{
    mModels[model_n] = model;
}

DisplaceModel &MapObjectsController::getModel(int model_n)
{
    return *mModels[model_n];
}

void MapObjectsController::removeModel(int model_n)
{
    mModels[model_n] = std::shared_ptr<DisplaceModel>();
    // maybe we should reload everything...?
}

void MapObjectsController::createMapObjectsFromModel(int model_n, DisplaceModel *model)
{
    addStandardLayer(model_n, LayerMain, mMainLayer, true);
    addStandardLayer(model_n, LayerSeamarks, mSeamarkLayer, true);

    DisplaceModel::ModelType type = model->modelType();

    mEntityLayer[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Entities")).arg(model_n).toStdString()));
    mGraphLayer[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Graph")).arg(model_n).toStdString()));
    mEdgesLayer[model_n] = std::shared_ptr<EdgeLayer>(new EdgeLayer(this, QString(tr("#%1#Graph Edges")).arg(model_n)));

    addStandardLayer(model_n, LayerEntities, mEntityLayer[model_n], true);
    addStandardLayer(model_n, LayerEdges, mEdgesLayer[model_n]->layer(), false);
    addStandardLayer(model_n, LayerGraph, mGraphLayer[model_n], false);

    mStatsLayerPop[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Abundance (PopValues TotalN)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerPopStats, mStatsLayerPop[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerBiomass[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Biomass (PopValues TotalW)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerBiomass, mStatsLayerBiomass[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerImpact[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Impact ((PopValues Impact)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerPopImpact, mStatsLayerImpact[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerCumcatchesPerPop[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Catches Per Pop (PopValues CumCatches)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerPopCumcatches, mStatsLayerCumcatchesPerPop[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerBenthosBiomass[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Benthos Biomass (FuncGroups bNTot)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerBenthosBiomass, mStatsLayerBenthosBiomass[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerBenthosNumber[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Benthos Number (FuncGroups bB)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerBenthosNumber, mStatsLayerBenthosNumber[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerBenthosMeanweight[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Benthos Mean Weight (FuncGroups bMeanW)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerBenthosMeanweight, mStatsLayerBenthosMeanweight[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerCumftime[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Fishing Effort (NodesStat CumFTime)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerCumFTime, mStatsLayerCumftime[model_n],type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerCumsweptarea[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Swept Area (NodesStat CumSwA)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerCumSweptArea, mStatsLayerCumsweptarea[model_n],type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerCumsubsurfacesweptarea[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Subsurface Swept Area (NodesStat CumSubSurfSwA)")).arg(model_n).toStdString()));
    addOutputLayer(model_n,OutLayerCumSubsurfaceSweptArea, mStatsLayerCumsubsurfacesweptarea[model_n],type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerCumcatches[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Catches (NodesStat CumCatches)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerCumCatches, mStatsLayerCumcatches[model_n],type != DisplaceModel::LiveModelType ? false : true);

    mStatsLayerCumcatchesWithThreshold[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Catches With 70\% Threshold (NodesStat CumCathesThrshld)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerCumCatchesWithThreshold, mStatsLayerCumcatchesWithThreshold[model_n],type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerCumdiscards[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Discards (NodesStat CumDiscards)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerCumdiscards, mStatsLayerCumdiscards[model_n],type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerCumdiscardsratio[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Discards Ratio (NodesStat CumDiscardsRatio)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerCumdiscardsRatio, mStatsLayerCumdiscardsratio[model_n],type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerNbchoked[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Nb choked stocks (NodesStat NbChokedStks)")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerNbChoked, mStatsLayerNbchoked[model_n],type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerWind[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Wind")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerWind, mStatsLayerWind[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerSST[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#SST")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerSST, mStatsLayerSST[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerSalinity[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Salinity")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerSalinity, mStatsLayerSalinity[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerNitrogen[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Nitrogen")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerNitrogen, mStatsLayerNitrogen[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerPhosphorus[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Phosphorus")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerPhosphorus, mStatsLayerPhosphorus[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerOxygen[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Oxygen")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerOxygen, mStatsLayerOxygen[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerDissolvedCarbon[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#DissolvedCarbon")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerDissolvedCarbon, mStatsLayerDissolvedCarbon[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerBathymetry[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Bathymetry")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerBathymetry, mStatsLayerBathymetry[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerShippingdensity[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Shippingdensity")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerShippingdensity, mStatsLayerShippingdensity[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerSiltfraction[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Siltfraction")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerSiltfraction, mStatsLayerSiltfraction[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerIcesrectanglecode[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Icesrectanglecode")).arg(model_n).toStdString()));
    addEnvLayer(model_n, EnvLayerIcesrectanglecode, mStatsLayerIcesrectanglecode[model_n], type != DisplaceModel::LiveModelType ? false : false);
    
    mStatsLayerTariffAll[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Tariff all (NodesTariffStat TariffAll)")).arg(model_n).toStdString()));
    addTariffLayer(model_n, TariffLayerTariffAll, mStatsLayerTariffAll[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerTariffPop[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Tariff pop (NodesTariffStat TariffPop)")).arg(model_n).toStdString()));
    addTariffLayer(model_n, TariffLayerTariffPop, mStatsLayerTariffPop[model_n], type != DisplaceModel::LiveModelType ? false : false);

    mStatsLayerTariffBenthos[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Tariff benthos (NodesTariffStat TariffBenthos)")).arg(model_n).toStdString()));
    addTariffLayer(model_n, TariffLayerTariffBenthos, mStatsLayerTariffBenthos[model_n], type != DisplaceModel::LiveModelType ? false : false);


    const QList<std::shared_ptr<HarbourData> > &harbours = model->getHarboursList();
    foreach (std::shared_ptr<HarbourData> h, harbours) {
        addHarbour(model_n, h, true);
    }

    const QList<std::shared_ptr<NodeData> > &nodes = model->getNodesList();
    foreach (std::shared_ptr<NodeData> nd, nodes) {
        if (nd->get_harbour())
            continue;
        addNode(model_n, nd, true);
    }

    const QList<std::shared_ptr<VesselData> > &vessels = model->getVesselList();
    foreach (std::shared_ptr<VesselData> vsl, vessels) {
        VesselMapObject *obj = new VesselMapObject(this,vsl.get());
        mVesselObjects[model_n].add(vsl->mVessel->get_idx(),obj, 0);

        mEntityLayer[model_n]->addGeometry(obj->getGeometryEntity());
    }

    const QList<std::shared_ptr<ShipData> > &ships = model->getShipList();
    foreach (std::shared_ptr<ShipData> sh, ships) {
        ShipMapObject *obj = new ShipMapObject(this,sh.get());
        mShipObjects[model_n].add(sh->mShip->get_idx(),obj, 0);

        mEntityLayer[model_n]->addGeometry(obj->getGeometryEntity());
    }

    const QList<std::shared_ptr<FishfarmData> > &fishfarms = model->getFishfarmList();
    foreach (std::shared_ptr<FishfarmData> ff, fishfarms) {
        FishfarmMapObject *obj = new FishfarmMapObject(this,ff.get());
        mFishfarmObjects[model_n].add(ff->mFishfarm->get_name(),obj, 0);

        mEntityLayer[model_n]->addGeometry(obj->getGeometryEntity());
    }

    const QList<std::shared_ptr<WindmillData> > &windmills = model->getWindmillList();
    foreach (std::shared_ptr<WindmillData> ff, windmills) {
        WindmillMapObject *obj = new WindmillMapObject(this,ff.get());
        mWindmillObjects[model_n].add(ff->mWindmill->get_idx(),obj, 0);

        mEntityLayer[model_n]->addGeometry(obj->getGeometryEntity());
    }

}

void MapObjectsController::updateMapObjectsFromModel(int model_n, DisplaceModel *model)
{
    const QList<std::shared_ptr<VesselData> > &vessels = model->getVesselList();
    foreach (std::shared_ptr<VesselData> vsl, vessels) {
        updateVesselPosition(model_n, vsl->mVessel->get_idx());
    }

}

void MapObjectsController::updateVesselPosition(int model, int idx)
{
    mVesselObjects[model].get(idx, 0)->vesselUpdated();
}

void MapObjectsController::updateShipPosition(int model, int idx)
{
    mShipObjects[model].get(idx, 0)->shipUpdated();
}


void MapObjectsController::updateFishfarmPosition(int model, int idx)
{
    mFishfarmObjects[model].get(idx, 0)->fishfarmUpdated();
}

void MapObjectsController::updateWindmillPosition(int model, int idx)
{
    mWindmillObjects[model].get(idx, 0)->windmillUpdated();
}

void MapObjectsController::updateNodes(int model)
{
    auto it = mNodeObjects[model].begin();
    while (!mNodeObjects[model].atEnd(it)) {
        NodeMapObject *obj = mNodeObjects[model].get(it, NodeMapObject::GraphNodeRole);
        obj->updateProperties();
        obj->getGeometryEntity()->requestRedraw();

        ++it;
    }
}

void MapObjectsController::setModelVisibility(int model, MapObjectsController::Visibility visibility)
{
    bool visible = (visibility == Visible);
    mModelVisibility[model] = visible;

    mLayers[model].updateVisibility(visible);
    mEnvLayers[model].updateVisibility(visible);
    mOutputLayers[model].updateVisibility(visible);
    mTariffsLayers[model].updateVisibility(visible);
}

void MapObjectsController::setLayerVisibility(int model, ObjectTreeModel::Category type, int layer, bool visibility)
{
    switch (type) {
    case ObjectTreeModel::Layers:
        mLayers[model].setVisible(layer, visibility);
        if (isModelActive(model))
            mLayers[model].layer(layer)->setVisible(visibility);
        break;
    case ObjectTreeModel::EnvLayers:
        mEnvLayers[model].setVisible(layer, visibility);
        if (isModelActive(model))
            mEnvLayers[model].layer(layer)->setVisible(visibility);
        break;
    case ObjectTreeModel::OutputLayers:
        mOutputLayers[model].setVisible(layer, visibility);
        if (isModelActive(model))
            mOutputLayers[model].layer(layer)->setVisible(visibility);
        break;
    case ObjectTreeModel::TariffsLayers:
        mTariffsLayers[model].setVisible(layer, visibility);
        if (isModelActive(model))
            mTariffsLayers[model].layer(layer)->setVisible(visibility);
        break;
    case ObjectTreeModel::ShapefileLayers:
        mShapefileLayers[model].setVisible(layer, visibility);
        if (isModelActive(model))
            mShapefileLayers[model].layers[layer]->setVisible(visibility);
        break;
    default:
        throw std::runtime_error("Unhandled type in setLayerVisibility");
    }
}

bool MapObjectsController::isLayerVisible(int model, ObjectTreeModel::Category type, int layer)
{
    if (!mModelVisibility[model])
        return false;

    switch (type) {
    case ObjectTreeModel::Layers:
        return mLayers[model].isVisible(layer);
    case ObjectTreeModel::EnvLayers:
        return mEnvLayers[model].isVisible(layer);
    case ObjectTreeModel::OutputLayers:
        return mOutputLayers[model].isVisible(layer);
    case ObjectTreeModel::TariffsLayers:
        return mTariffsLayers[model].isVisible(layer);
    case ObjectTreeModel::ShapefileLayers:
        return mShapefileLayers[model].isVisible(layer);
    default:
        break;
    }

    return false;
}

bool MapObjectsController::isModelActive(int model) const
{
    return mModelVisibility[model];
}

void MapObjectsController::setPalette(int model, PaletteRole n, const Palette &palette)
{
    Q_UNUSED(model);
    PaletteManager::instance()->setPalette(n, palette);
}

void MapObjectsController::forceRedraw()
{
    mMap->requestRedraw();
}

void MapObjectsController::showDetailsWidget(const PointWorldCoord &point, QWidget *widget)
{
#ifndef CONFIG_ALLOW_MULTIPLE_DETAILS_WINDOWS
    removeAllWidgets();
//    foreach (const auto &g, mWidgetLayer->getGeometryWidgets()) {
//        mWidgetLayer->removeGeometry(g);
//    }
#endif

    std::shared_ptr<qmapcontrol::GeometryWidget>  mDetailsWidgetContainer = std::shared_ptr<qmapcontrol::GeometryWidget>(new qmapcontrol::GeometryWidget(point, widget));
    mDetailsWidgetContainer->setAlignmentType(GeometryPoint::AlignmentType::BottomLeft);
    mDetailsWidgetContainer->setVisible(true);

    widget->setUserData(0, new WidgetUserData(mDetailsWidgetContainer));
    widget->setAttribute(Qt::WA_DeleteOnClose);
    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetClosed(QObject*)));

    mWidgetLayer->addGeometry(mDetailsWidgetContainer);
}

bool MapObjectsController::importShapefile(int model_idx, QString path, QString layername)
{
    /// @bug The following code will result in a memory leak and/or double free bug.
    std::shared_ptr<GDALDataset> src((GDALDataset*)OGROpen(path.toStdString().c_str(), 0, nullptr));
    if (!src.get()) {
        return false;
    }

    std::shared_ptr<ESRIShapefile> file (new ESRIShapefile(src.get(), layername.toStdString()));

    file->setPenPolygon(QPen(Qt::red));
    QColor col (Qt::yellow);
    col.setAlpha(64);
    file->setBrushPolygon(QBrush(col));

    QFileInfo info(path);
    QString label = QString("Shapefile %1").arg(info.fileName());

    if (!layername.isEmpty())
        label += QString (" (%1)").arg(layername);

    std::shared_ptr<qmapcontrol::LayerESRIShapefile> newlayer(new qmapcontrol::LayerESRIShapefile(label.toStdString()));
    newlayer->addESRIShapefile(file);
    //addShapefileLayer(model_idx, layername, src, newlayer); // WRONG
    addShapefileLayer(model_idx, path, src, newlayer);

    return true;
}

QStringList MapObjectsController::getShapefilesList(int model_idx) const
{
    QStringList list;
    int n = mShapefileLayers[model_idx].getCount();
    for (int i = 0; i < n; ++i) {
        list << mShapefileLayers[model_idx].getName(i);
    }
    return list;
}

std::shared_ptr<ESRIShapefile> MapObjectsController::getShapefile(int model_idx, int idx)
{
    return mShapefileLayers[model_idx].layers[idx]->getShapefile(0);
}

std::shared_ptr<GDALDataset> MapObjectsController::cloneShapefileDatasource(int model_idx, const QString &name)
{
    for (int i = 0; i < mShapefileLayers[model_idx].getCount(); ++i) {
        if (mShapefileLayers[model_idx].getName(i) == name) {
            const QString &fullpath = mShapefileLayers[model_idx].fullpath.at(i);
            return std::shared_ptr<GDALDataset>((GDALDataset*)OGROpen(fullpath.toStdString().c_str(), 0, nullptr));
        }
    }

    return std::shared_ptr<GDALDataset>();
}

void MapObjectsController::setEditorMode(MapObjectsController::EditorModes mode)
{
    mEditorMode = mode;
}

void MapObjectsController::clearNodeSelection(int model)
{
    QSet<NodeMapObject *> tmp = mNodeSelection[model];
    for (QSet<NodeMapObject *>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
        NodeMapObject *nmo = *it;
        nmo->setSelection(false);
        nodeSelectionHasChanged(nmo);
        nmo->getGeometryEntity()->requestRedraw();
    }
}

void MapObjectsController::selectNodes(int model, QList<types::NodeId> nodes)
{
    foreach (auto node, nodes) {
        NodeMapObject *nmo = mNodeObjects[model].get(node, NodeMapObject::GraphNodeRole);
        nmo->setSelection(true);
        nodeSelectionHasChanged(nmo);
        nmo->getGeometryEntity()->requestRedraw();
    }
}

void MapObjectsController::delSelected(int model)
{
    if (mEditorMode == NoEditorMode)
        return;

    switch (mEditorMode) {
    case EdgeEditorMode:
        delSelectedEdges(model);
        break;
    case NodeEditorMode:
        delSelectedNodes(model);
        break;
    default:
        break;
    }
}

void MapObjectsController::clearAllNodes(int model_n)
{
    mGraphLayer[model_n]->clearGeometries();
    mStatsLayerBiomass[model_n]->clearGeometries();
    mStatsLayerCumftime[model_n]->clearGeometries();
    mStatsLayerCumsweptarea[model_n]->clearGeometries();
    mStatsLayerCumcatches[model_n]->clearGeometries();
    mStatsLayerCumcatchesWithThreshold[model_n]->clearGeometries();
    mStatsLayerCumdiscards[model_n]->clearGeometries();
    mStatsLayerCumdiscardsratio[model_n]->clearGeometries();
    mStatsLayerNbchoked[model_n]->clearGeometries();
    mStatsLayerImpact[model_n]->clearGeometries();
    mStatsLayerCumcatchesPerPop[model_n]->clearGeometries();
    mStatsLayerBenthosBiomass[model_n]->clearGeometries();
    mStatsLayerBenthosNumber[model_n]->clearGeometries();
    mStatsLayerBenthosMeanweight[model_n]->clearGeometries();
    mStatsLayerPop[model_n]->clearGeometries();
    mStatsLayerTariffAll[model_n]->clearGeometries();
    mStatsLayerTariffPop[model_n]->clearGeometries();
    mStatsLayerTariffBenthos[model_n]->clearGeometries();
    mStatsLayerSalinity[model_n]->clearGeometries();
    mStatsLayerSST[model_n]->clearGeometries();
    mStatsLayerWind[model_n]->clearGeometries();
    mStatsLayerNitrogen[model_n]->clearGeometries();
    mStatsLayerPhosphorus[model_n]->clearGeometries();
    mStatsLayerOxygen[model_n]->clearGeometries();
    mStatsLayerDissolvedCarbon[model_n]->clearGeometries();
    mStatsLayerBathymetry[model_n]->clearGeometries();
    mStatsLayerShippingdensity[model_n]->clearGeometries();
    mStatsLayerSiltfraction[model_n]->clearGeometries();
    mStatsLayerIcesrectanglecode[model_n]->clearGeometries(); 
    mEdgesLayer[model_n]->clear();
    mEntityLayer[model_n]->clearGeometries();

    mHarbourObjects[model_n].clear();
    mNodeObjects[model_n].clear();
    mEdgeObjects[model_n].clear();
}

void MapObjectsController::addStandardLayer(int model, LayerIds id, std::shared_ptr<Layer> layer, bool visibility)
{
    if (layer != mMainLayer && layer != mSeamarkLayer)
        mMap->addLayer(layer);
    mLayers[model].setLayer(id, layer, visibility);
}

void MapObjectsController::addOutputLayer(int model, OutLayerIds id, std::shared_ptr<Layer> layer, bool visibility)
{
    mMap->addLayer(layer);
    mOutputLayers[model].setLayer(id,layer, visibility);
}

void MapObjectsController::addEnvLayer(int model, int id, std::shared_ptr<Layer> layer, bool visibility)
{
    mMap->addLayer(layer);
    mEnvLayers[model].setLayer(id,layer, visibility);
}

void MapObjectsController::addTariffLayer(int model, int id, std::shared_ptr<Layer> layer, bool visibility)
{
    mMap->addLayer(layer);
    mTariffsLayers[model].setLayer(id,layer, visibility);
}


void MapObjectsController::addShapefileLayer(int model, QString name, std::shared_ptr<GDALDataset> datasource, std::shared_ptr<qmapcontrol::LayerESRIShapefile> layer, bool show)
{
    mMap->addLayer(layer);
    mShapefileLayers[model].add(layer, name, show);
    mShapefiles[model].append(datasource);
}

void MapObjectsController::addNode(int model_n, std::shared_ptr<NodeData> nd, bool disable_redraw)
{
    if (nd->isDeleted())
        return;

    auto idxnode = nd->get_idx_node();

    NodeMapObject *obj = new NodeMapObject(this, model_n, NodeMapObject::GraphNodeRole, nd);
    mNodeObjects[model_n].add(idxnode, obj, obj->getRole());

    mGraphLayer[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    /* add here other roles */
    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithPopStatsRole, nd);
    mNodeObjects[model_n].add(idxnode, obj, obj->getRole());
    mStatsLayerPop[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumFTimeRole, nd);
    mNodeObjects[model_n].add(idxnode, obj, obj->getRole());
    mStatsLayerCumftime[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumSweptAreaRole, nd);
    mNodeObjects[model_n].add(idxnode, obj, obj->getRole());
    mStatsLayerCumsweptarea[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumSubsurfaceSweptAreaRole, nd);
    mNodeObjects[model_n].add(idxnode, obj, obj->getRole());
    mStatsLayerCumsubsurfacesweptarea[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumCatchesRole, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerCumcatches[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumCatchesWithThresholdRole, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerCumcatchesWithThreshold[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumDiscardsRole, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerCumdiscards[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumDiscardsRatioRole, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerCumdiscardsratio[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithNbChokedRole, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerNbchoked[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithPopImpact, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerImpact[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithPopCumcatches, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerCumcatchesPerPop[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithBenthosBiomass, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerBenthosBiomass[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithBenthosNumber, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerBenthosNumber[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithBenthosMeanweight, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerBenthosMeanweight[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithBiomass, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerBiomass[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithTariffs0, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerTariffAll[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithTariffs1, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerTariffPop[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithTariffs2, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerTariffBenthos[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithSalinity, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerSalinity[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithSST, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerSST[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithWind, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerWind[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithNitrogen, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerNitrogen[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithPhosphorus, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerPhosphorus[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithOxygen, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerOxygen[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithDissolvedCarbon, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerDissolvedCarbon[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithBathymetry, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerBathymetry[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithShippingdensity, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerShippingdensity[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithSiltfraction, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerSiltfraction[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n, NodeMapObject::GraphNodeWithIcesrectanglecode, nd);
    mNodeObjects[model_n].add(nd->get_idx_node(), obj, obj->getRole());
    mStatsLayerIcesrectanglecode[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);
    
    for (int i = 0; i < nd->getAdiacencyCount(); ++i) {
        addEdge(model_n,nd->getAdiacencyByIdx(i), disable_redraw);
    }
}

void MapObjectsController::addEdge (int model_n, std::shared_ptr<NodeData::Edge> _edge, bool disable_redraw)
{
    EdgeMapObject *edge = new EdgeMapObject(this, _edge);

    connect (edge, SIGNAL(edgeSelectionHasChanged(EdgeMapObject*)), this, SLOT(edgeSelectionHasChanged(EdgeMapObject*)));

    mEdgeObjects[model_n].add(-1, edge, 0);
    mEdgesLayer[model_n]->addEdge(edge, disable_redraw);
}

void MapObjectsController::addHarbour(int model_n, std::shared_ptr<HarbourData> h, bool disable_redraw)
{
    HarbourMapObject *obj = new HarbourMapObject(this, mModels[model_n].get(), h.get());
    mHarbourObjects[model_n].add(h->mHarbour->get_idx_node(), obj, 0);

    mEntityLayer[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    auto nd = mModels[model_n]->getNodesList()[h->mHarbour->get_idx_node().toIndex()];
    for (int i = 0; i < nd->getAdiacencyCount(); ++i) {
        addEdge(model_n,nd->getAdiacencyByIdx(i), disable_redraw);
    }
}

void MapObjectsController::clearEditorLayer()
{
    mEditorLayer->clearGeometries();
}

void MapObjectsController::addEditorLayerGeometry(std::shared_ptr<Geometry> geometry)
{
    mEditorLayer->addGeometry(geometry);
}

MapsDataProvider &MapObjectsController::getMapDataProvider(int model)
{
    return mModels[model]->getMapDataProvider();
}

void MapObjectsController::delSelectedEdges(int model)
{
    // TODO: Remove also the edge data from mEdgeObjects
    foreach (EdgeMapObject *edge, mEdgeSelection[model]) {
        std::shared_ptr<NodeData> nd = edge->node();
        std::shared_ptr<NodeData> tg = edge->target();

//        int nodeid1 = nd->get_idx_node();

        // TODO: automatically remove other side edges
//        int nodeid2 = nd->getAdiacencyByIdx(eid1);

//        NodeData *nd2 = mModels[model]->getNodesList()[nodeid2];
//        nd2->removeAdiacencyByTarget(nodeid1);
//        nd->removeAdiacencyByIdx(eid1);
        if (nd.get() != nullptr && tg != nullptr) {
            nd->removeAdiacencyByTarget(tg);

            mEdgesLayer[model]->removeEdge(edge);
        }
    }

    mEdgeSelection[model].clear();
    emit edgeSelectionChanged(0);
}

void MapObjectsController::delSelectedNodes(int model)
{
    QList<NodeMapObject *> deleteList;

    foreach (NodeMapObject *node, mNodeSelection[model]) {
        std::shared_ptr<NodeData> nd = node->node();
            // remove all adiacencies
        for (int i = 0; i < nd->getAdiacencyCount(); ++i) {
            auto edge = nd->getAdiacencyByIdx(i);

            std::shared_ptr<NodeData> tg = edge->target.lock();
            if (tg.get()) {
                tg->removeAdiacencyByTarget(nd);
            }
        }
        // remove the node
        auto objs = mNodeObjects[model].remove(nd->get_idx_node());

        deleteList.append(objs);

        mModels[model]->removeNode(nd);
    }

    foreach (NodeMapObject *obj, deleteList) {
        if (obj) {
            std::shared_ptr<qmapcontrol::Geometry> geom = obj->getGeometryEntity();
            qmapcontrol::LayerGeometry *layer = geom->layer();
            if (layer) {
                layer->removeGeometry(geom);
            }
            delete obj;
        }
    }

    mNodeSelection[model].clear();
    emit nodeSelectionChanged(0);
}

void MapObjectsController::geometryClicked(const Geometry *geometry)
{
    WidgetAncillaryData *objPtr = reinterpret_cast<WidgetAncillaryData *>(geometry->ancillaryData());

    if (objPtr && objPtr->object()) {
        switch(mEditorMode) {
        case NoEditorMode:
            objPtr->object()->showProperties();
            break;
        case EdgeEditorMode:
            do {
                EdgeMapObject *emo = dynamic_cast<EdgeMapObject*>(objPtr->object());
                if (emo != nullptr) {
                    objPtr->object()->toggleSelection();
                    edgeSelectionHasChanged(emo);
                    emo->getGeometryEntity()->requestRedraw();
                }
            } while (false);
            break;
        case NodeEditorMode:
            do {
                NodeMapObject *nmo = dynamic_cast<NodeMapObject*>(objPtr->object());
                if (nmo != nullptr) {
                    objPtr->object()->toggleSelection();
                    nodeSelectionHasChanged(nmo);
                    nmo->getGeometryEntity()->requestRedraw();
                }
            } while (false);
            break;
        default:
            break;
        }
    }
}

void MapObjectsController::widgetClosed(QObject *widget)
{
    if (mClosing)
        return;
    WidgetUserData *obj = reinterpret_cast<WidgetUserData*>(widget->userData(0));
    if (obj) {
        mWidgetLayer->removeGeometry(obj->widget(), mClosing);
    }
}

void MapObjectsController::signalAppIsClosing()
{
    mClosing = true;
}

void MapObjectsController::removeAllWidgets()
{
    foreach (const auto &wid, mWidgetLayer->getGeometryWidgets()) {
        wid->getWidget()->close();
        mWidgetLayer->removeGeometry(wid, true);
    }
}

void MapObjectsController::edgeSelectionHasChanged(EdgeMapObject *object)
{
    int modelIndex = object->node()->getModel()->index();

    if (object->selected())
        mEdgeSelection[modelIndex].insert(object);
    else
        mEdgeSelection[modelIndex].remove(object);

    emit edgeSelectionChanged(mEdgeSelection[modelIndex].size());
}

void MapObjectsController::nodeSelectionHasChanged(NodeMapObject *node)
{
    int modelIndex = node->node()->getModel()->index();

    if (node->selected())
        mNodeSelection[modelIndex].insert(node);
    else
        mNodeSelection[modelIndex].remove(node);

    emit nodeSelectionChanged(mNodeSelection[modelIndex].size());
}

void MapObjectsController::redraw()
{
    mMap->requestRedraw();
}
