#ifndef HARBOURMAPOBJECT_H
#define HARBOURMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <QMapControl/GeometryPointCircle.h>
#include <Harbour.h>

class HarbourMapObject : public MapObject
{
public:
    HarbourMapObject(Harbour *harbour);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

private:
    Harbour *mHarbour;
    std::shared_ptr<qmapcontrol::GeometryPointCircle> mGeometry;
};

#endif // HARBOURMAPOBJECT_H
