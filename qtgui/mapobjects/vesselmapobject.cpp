#include "vesselmapobject.h"

VesselMapObject::VesselMapObject(Vessel *vessel)
    : mVessel(vessel)
{

    mGeometry = std::shared_ptr<qmapcontrol::GeometryPointShapeScaled> (new qmapcontrol::GeometryPointShapeScaled(
                                                                       qmapcontrol::PointWorldCoord(
                                                                           mVessel->get_x(), mVessel->get_y()
                                                                           ),
                                                                            QSizeF(10.0, 10.0), 14
                                                                       ));
}
