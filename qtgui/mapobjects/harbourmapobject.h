#ifndef HARBOURMAPOBJECT_H
#define HARBOURMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <QMapControl/GeometryPointImageScaled.h>
#include <Harbour.h>
#include <QPixmap>

class HarbourMapObject : public MapObject
{
public:
    HarbourMapObject(Harbour *harbour);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

private:
    Harbour *mHarbour;
    std::shared_ptr<qmapcontrol::GeometryPointImageScaled> mGeometry;

    static QPixmap *symbol;
};

#endif // HARBOURMAPOBJECT_H
