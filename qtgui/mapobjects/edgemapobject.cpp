#include "edgemapobject.h"

#include <mapobjectscontroller.h>
#include <displacemodel.h>

EdgeMapObject::EdgeMapObject(MapObjectsController *controller, int indx, NodeData *node)
    : QObject(),
      mController(controller),
      mEdgeIndex(indx),
      mNode(node)
{
    std::vector<qmapcontrol::PointWorldCoord> line;
    line.push_back(qmapcontrol::PointWorldCoord(mNode->get_x(), mNode->get_y()));

    NodeData *t = mNode->getModel()->getNodesList()[mNode->getAdiacencyByIdx(mEdgeIndex)];
    line.push_back(qmapcontrol::PointWorldCoord(t->get_x(), t->get_y()));

    mGeometry = std::shared_ptr<qmapcontrol::GeometryLineString>(new qmapcontrol::GeometryLineString(line));

    mGeometry->setPen(QPen(Qt::black));
}
