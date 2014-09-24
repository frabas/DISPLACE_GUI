#include "nodemapobject.h"

NodeMapObject::NodeMapObject(Node *node)
    : mNode(node),
     mGeometry()
{
   mGeometry = std::shared_ptr<qmapcontrol::GeometryPointCircle> (new qmapcontrol::GeometryPointCircle(
                                                                      qmapcontrol::PointWorldCoord(
                                                                          mNode->get_x(), mNode->get_y()
                                                                          )
                                                                      ));
}
