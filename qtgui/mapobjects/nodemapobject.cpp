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

   int l = mNode->get_marine_landscape();
   QColor c = QColor(((l / 100) % 10) * 25, ((l / 10) % 10) * 25, (l % 10 )* 25);
   mGeometry->setBrush(c);
}
