#ifndef VESSELMAPOBJECT_H
#define VESSELMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <QMapControl/GeometryPointShapeScaled.h>
#include <Vessel.h>

#include <QBrush>

namespace qmapcontrol {
    class RectWorldCoord;
}

class VesselMapObject : public MapObject
{

    class VesselGraphics : public qmapcontrol::GeometryPointShapeScaled {
        static QBrush *color;
        static QBrush *altColor;

        Vessel *mVessel;
    public:
        VesselGraphics (Vessel *vessel);

        void draw(QPainter& painter, const qmapcontrol::RectWorldCoord& backbuffer_rect_coord, const int& controller_zoom);
    };

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
