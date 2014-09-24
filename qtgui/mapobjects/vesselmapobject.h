#ifndef VESSELMAPOBJECT_H
#define VESSELMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <QMapControl/GeometryPointShapeScaled.h>
#include <Vessel.h>

class VesselMapObject : public MapObject
{
public:
    VesselMapObject(Vessel *vessel);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

private:
    Vessel *mVessel;
    std::shared_ptr<qmapcontrol::GeometryPointShapeScaled> mGeometry;

};

#endif // VESSELMAPOBJECT_H
