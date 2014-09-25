#ifndef MAPOBJECTSCONTROLLER_H
#define MAPOBJECTSCONTROLLER_H

#include <QList>

#include <memory>

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

    void updateMapObjects(DisplaceModel *model);

    void updateVesselPosition (int idx);

private:
    qmapcontrol::QMapControl *mMap;
    QList<HarbourMapObject *> mHarbourObjects;
    QList<NodeMapObject *> mNodeObjects;
    QList<VesselMapObject *> mVesselObjects;

    std::shared_ptr<qmapcontrol::MapAdapter> mMainMapAdapter;
    std::shared_ptr<qmapcontrol::MapAdapter> mSeamarkAdapter;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mMainLayer;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mSeamarkLayer;
    std::shared_ptr<qmapcontrol::LayerGeometry> mEntityLayer;
    std::shared_ptr<qmapcontrol::LayerGeometry> mGraphLayer;
};

#endif // MAPOBJECTSCONTROLLER_H
