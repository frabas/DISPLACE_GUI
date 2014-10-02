#ifndef NODEMAPOBJECT_H
#define NODEMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <modelobjects/nodedata.h>
#include <mapobjects/nodegraphics.h>

namespace qmapcontrol {
    class RectWorldCoord;
}

class NodeMapObject : public MapObject
{
public:
    enum Role {
        GraphNodeRole,
        GraphNodeWithPopStatsRole,
        GraphNodeWithCumFTimeRole,
    };

    NodeMapObject(Role role, NodeData *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

private:
    NodeData *mNode;
    std::shared_ptr<NodeGraphics> mGeometry;

};

#endif // NODEMAPOBJECT_H
