#include "mapobjectscontroller.h"

#include <displacemodel.h>
#include <mapobjects/harbourmapobject.h>
#include <mapobjects/nodemapobject.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/MapAdapterOSM.h>
#include <QMapControl/Layer.h>
#include <QMapControl/MapAdapterOpenSeaMap.h>
#include <QMapControl/LayerGeometry.h>
#include <QMapControl/GeometryPointCircle.h>
#include <QMapControl/LayerMapAdapter.h>
#include <QMapControl/ImageManager.h>

MapObjectsController::MapObjectsController(qmapcontrol::QMapControl *map)
    : mMap(map)
{
    // create mapadapter, for mainlayer and overlay
    mMainMapAdapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOSM());
    mSeamarkAdapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOpenSeaMap());

    // create a layer with the mapadapter and type MapLayer
    mMainLayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("OpenStreetMap", mMainMapAdapter));
    mSeamarkLayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("Seamark", mSeamarkAdapter));
    mEntityLayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Entities"));
    mGraphLayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Graph"));

    // add Layer to the MapControl
    mMap->addLayer(mMainLayer);
    mMap->addLayer(mSeamarkLayer);
    mMap->addLayer(mGraphLayer);
    mMap->addLayer(mEntityLayer);

    mMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(11.54105,54.49299));
    mMap->setZoom(10);

}

void MapObjectsController::updateMapObjects(DisplaceModel *model)
{
    const QList<Harbour *> &harbours = model->getHarboursList();
    foreach (Harbour *h, harbours) {
        HarbourMapObject *obj = new HarbourMapObject(h);
        mHarbourObjects.append(obj);

        mEntityLayer->addGeometry(obj->getGeometryEntity());
    }

    const QList<Node *> &nodes = model->getNodesList();
    foreach (Node *nd, nodes) {
        NodeMapObject *obj = new NodeMapObject(nd);
        mNodeObjects.append(obj);

        mGraphLayer->addGeometry(obj->getGeometryEntity());
    }

}
