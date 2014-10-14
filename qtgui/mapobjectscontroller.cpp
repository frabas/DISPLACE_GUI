#include "mapobjectscontroller.h"

#include <displacemodel.h>
#include <mapobjects/harbourmapobject.h>
#include <mapobjects/nodemapobject.h>
#include <mapobjects/vesselmapobject.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/MapAdapterOSM.h>
#include <QMapControl/Layer.h>
#include <QMapControl/MapAdapterOpenSeaMap.h>
#include <QMapControl/LayerGeometry.h>
#include <QMapControl/GeometryPointCircle.h>
#include <QMapControl/LayerMapAdapter.h>
#include <QMapControl/ImageManager.h>

MapObjectsController::MapObjectsController(qmapcontrol::QMapControl *map)
    : mMap(map),
      mPaletteManager(),
      mModelVisibility(MAX_MODELS, false),
      mLayers(MAX_MODELS, LayerListImpl(LayerMax)),
      mOutputLayers(MAX_MODELS, LayerListImpl(OutLayerMax))
{
    // create mapadapter, for mainlayer and overlay
    mMainMapAdapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOSM());
    mSeamarkAdapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOpenSeaMap());

    // create a layer with the mapadapter and type MapLayer
    mMainLayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("OpenStreetMap", mMainMapAdapter));
    mSeamarkLayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("Seamark", mSeamarkAdapter));

    mMap->addLayer(mMainLayer);
    mMap->addLayer(mSeamarkLayer);

    mMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(11.54105,54.49299));
    mMap->setZoom(10);
}

void MapObjectsController::createMapObjectsFromModel(int model_n, DisplaceModel *model)
{
    mPaletteManager[model_n] = std::shared_ptr<PaletteManager>(new PaletteManager());
    QFile pf(":/palettes/iso1996_2.p2c");
    Palette p;
    p.loadFromFile(&pf);

    for (int i = 0; i < (int)LastRole; ++i)
        mPaletteManager[model_n]->setPalette((PaletteRole)i, p);

    addStandardLayer(model_n, LayerMain, mMainLayer);
    addStandardLayer(model_n, LayerSeamarks, mSeamarkLayer);

    std::shared_ptr<qmapcontrol::LayerGeometry> mEntityLayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Entities"));
    std::shared_ptr<qmapcontrol::LayerGeometry> mGraphLayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Graph"));

    addStandardLayer(model_n, LayerEntities, mEntityLayer);
    addStandardLayer(model_n, LayerGraph, mGraphLayer);

    std::shared_ptr<qmapcontrol::LayerGeometry> popstatslayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Pop Stats"));
    addOutputLayer(model_n, OutLayerPopStats, popstatslayer);

    std::shared_ptr<qmapcontrol::LayerGeometry> impactlayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Impact"));
    addOutputLayer(model_n, OutLayerPopImpact, impactlayer);

    std::shared_ptr<qmapcontrol::LayerGeometry> cumftimelayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Cum F Time"));
    addOutputLayer(model_n, OutLayerCumFTime, cumftimelayer);

    const QList<Harbour *> &harbours = model->getHarboursList();
    foreach (Harbour *h, harbours) {
        HarbourMapObject *obj = new HarbourMapObject(h);
        mHarbourObjects[model_n].append(obj);

        mEntityLayer->addGeometry(obj->getGeometryEntity());
    }

    const QList<NodeData *> &nodes = model->getNodesList();
    foreach (NodeData *nd, nodes) {
        NodeMapObject *obj = new NodeMapObject(this, model_n, NodeMapObject::GraphNodeRole, nd);
        mNodeObjects[model_n].append(obj);

        mGraphLayer->addGeometry(obj->getGeometryEntity());

        /* add here other roles */
        obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithPopStatsRole, nd);
        mNodeObjects[model_n].append(obj);
        popstatslayer->addGeometry(obj->getGeometryEntity());

        obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumFTimeRole, nd);
        mNodeObjects[model_n].append(obj);
        cumftimelayer->addGeometry(obj->getGeometryEntity());

        obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithPopImpact, nd);
        mNodeObjects[model_n].append(obj);
        impactlayer->addGeometry(obj->getGeometryEntity());
    }

    const QList<VesselData *> &vessels = model->getVesselList();
    foreach (VesselData *vsl, vessels) {
        VesselMapObject *obj = new VesselMapObject(vsl);
        mVesselObjects[model_n].append(obj);

        mEntityLayer->addGeometry(obj->getGeometryEntity());
    }
}

void MapObjectsController::updateMapObjectsFromModel(int model_n, DisplaceModel *model)
{
    const QList<VesselData *> &vessels = model->getVesselList();
    foreach (VesselData *vsl, vessels) {
        updateVesselPosition(model_n, vsl->mVessel->get_idx());
    }

}

void MapObjectsController::updateVesselPosition(int model, int idx)
{
    mVesselObjects[model].at(idx)->vesselUpdated();
}

void MapObjectsController::updateNodes(int model)
{
    foreach (NodeMapObject *obj, mNodeObjects[model]) {
        obj->getGeometryEntity()->requestRedraw();
    }
}

void MapObjectsController::setModelVisibility(int model, MapObjectsController::Visibility visibility)
{
    bool visible = (visibility == Visible);
    mModelVisibility[model] = visible;

    foreach (HarbourMapObject *h, mHarbourObjects[model]) {
        h->getGeometryEntity()->setVisible(visible);
    }
    foreach (NodeMapObject *n, mNodeObjects[model]) {
        n->getGeometryEntity()->setVisible(visible);
    }
    foreach (VesselMapObject *v, mVesselObjects[model]) {
        v->getGeometryEntity()->setVisible(visible);
    }
}

void MapObjectsController::setLayerVisibility(int model, MapObjectsController::LayerIds layer, bool visibility)
{
    mLayers[model].setVisible(layer, visibility);
    if (isModelActive(model))
        mLayers[model].layers[layer]->setVisible(visibility);
}

bool MapObjectsController::isLayerVisible(int model, MapObjectsController::LayerIds layer)
{
    return mModelVisibility[model] && mLayers[model].isVisible(layer);
}

void MapObjectsController::setOutLayerVisibility(int model, MapObjectsController::OutLayerIds layer, bool visibility)
{
    mOutputLayers[model].setVisible(layer, visibility);
    if (isModelActive(model))
        mOutputLayers[model].layers[layer]->setVisible(visibility);
}

bool MapObjectsController::isOutLayerVisible(int model, OutLayerIds layer)
{
    return mModelVisibility[model] && mOutputLayers[model].isVisible(layer);
}

bool MapObjectsController::isModelActive(int model) const
{
    return mModelVisibility[model];
}

void MapObjectsController::setPalette(int model, PaletteRole n, const Palette &palette)
{
    mPaletteManager[model]->setPalette(n, palette);
}

void MapObjectsController::forceRedraw()
{
    mMap->requestRedraw();
}

void MapObjectsController::addStandardLayer(int model, LayerIds id, std::shared_ptr<Layer> layer)
{
    if (layer != mMainLayer && layer != mSeamarkLayer)
        mMap->addLayer(layer);
    mLayers[model].layers[id] = layer;
}

void MapObjectsController::addOutputLayer(int model, OutLayerIds id, std::shared_ptr<Layer> layer)
{
    mMap->addLayer(layer);
    mOutputLayers[model].layers[id] = layer;
}
