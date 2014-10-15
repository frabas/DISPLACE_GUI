#ifndef NODEMAPOBJECT_H
#define NODEMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <modelobjects/nodedata.h>
#include <mapobjects/nodegraphics.h>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;

class NodeMapObject : public MapObject
{
    MapObjectsController *mController;
public:
    /* Note to developers: When adding "Roles", add a proper creation case into Constructor.
     * Also check MapObjectsController::createMapObjectsFromModel for proper layer creation.
     * And MapObjectsController should define a proper Output Layer type (OutLayerId)
     *
     * */
    enum Role {
        GraphNodeRole,
        GraphNodeWithPopStatsRole,
        GraphNodeWithBiomass,
        GraphNodeWithCumFTimeRole,
        GraphNodeWithPopImpact,
    };

    NodeMapObject(MapObjectsController *controller, int indx, Role role, NodeData *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    virtual bool clicked();
private:
    NodeData *mNode;
    Role mRole;
    std::shared_ptr<NodeGraphics> mGeometry;

};

#endif // NODEMAPOBJECT_H
