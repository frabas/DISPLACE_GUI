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

   int l = (mNode->get_marine_landscape() * 0x1000000) / 1000;
   QColor c = QColor(QRgb(l & 0x00ffffff));
   mGeometry->setBrush(c);
}
