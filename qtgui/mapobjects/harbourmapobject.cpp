#include "harbourmapobject.h"

HarbourMapObject::HarbourMapObject(Harbour *harbour)
    : mHarbour(harbour),
      mGeometry()
{
    mGeometry = std::shared_ptr<qmapcontrol::GeometryPointCircle> (new qmapcontrol::GeometryPointCircle(
                                                                       qmapcontrol::PointWorldCoord(
                                                                           mHarbour->get_x(), mHarbour->get_y()
                                                                           )
                                                                       ));
}
