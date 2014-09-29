#ifndef MAPOBJECTSCONTROLLER_H
#define MAPOBJECTSCONTROLLER_H

#include <QList>

#include <memory>
#include <mainwindow.h>

namespace qmapcontrol {
class QMapControl;
class MapAdapter;
class LayerMapAdapter;
class LayerGeometry;
}

class DisplaceModel;
class HarbourMapObject;
class NodeMapObject;
class VesselMapObject;

class MapObjectsController
{
public:
    MapObjectsController(qmapcontrol::QMapControl *map);

    void updateMapObjects(int model_n, DisplaceModel *model);

    void updateVesselPosition (int model, int idx);

private:
    qmapcontrol::QMapControl *mMap;
    QList<HarbourMapObject *> mHarbourObjects[MAX_MODELS];
    QList<NodeMapObject *> mNodeObjects[MAX_MODELS];
    QList<VesselMapObject *> mVesselObjects[MAX_MODELS];

    std::shared_ptr<qmapcontrol::MapAdapter> mMainMapAdapter;
    std::shared_ptr<qmapcontrol::MapAdapter> mSeamarkAdapter;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mMainLayer;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mSeamarkLayer;
    std::shared_ptr<qmapcontrol::LayerGeometry> mEntityLayer;
    std::shared_ptr<qmapcontrol::LayerGeometry> mGraphLayer;
};

#endif // MAPOBJECTSCONTROLLER_H
