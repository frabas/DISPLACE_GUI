#include "nodemapobject.h"

#include <displacemodel.h>
#include <mapobjects/nodegraphics.h>

NodeMapObject::NodeMapObject(MapObjectsController *controller, Role role, NodeData *node)
    : mNode(node),
      mController(controller),
      mGeometry()
{
    switch (role) {

    case GraphNodeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeGraphics(mNode, mController));
        break;

    case GraphNodeWithPopStatsRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(mNode, mController));
        break;

    case GraphNodeWithCumFTimeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumFTimeGraphics(mNode, mController));
        break;

    }

}
