#include "nodemapobject.h"

#include <displacemodel.h>
#include <mapobjects/nodegraphics.h>

NodeMapObject::NodeMapObject(Role role, NodeData *node)
    : mNode(node),
     mGeometry()
{
    switch (role) {

    case GraphNodeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeGraphics(mNode));
        break;

    case GraphNodeWithPopStatsRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(mNode));
        break;

    case GraphNodeWithCumFTimeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumFTimeGraphics(mNode));
        break;

    }

}
