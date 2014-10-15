#ifndef VESSELMAPOBJECT_H
#define VESSELMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <modelobjects/vesseldata.h>

#include <QMapControl/GeometryPointShapeScaled.h>

#include <QBrush>

namespace qmapcontrol {
    class RectWorldCoord;
}

class VesselMapObject : public MapObject
{

    class VesselGraphics : public qmapcontrol::GeometryPointShapeScaled {
        static QBrush *color;
        static QBrush *statHarbour, *statSteaming, *statFishing;

        VesselData *mVessel;
    public:
        VesselGraphics (VesselData *vessel);

        void updated();
    protected:
        virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
    };

public:
    VesselMapObject(VesselData *vessel);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    void vesselUpdated();

private:
    VesselData *mVessel;
    std::shared_ptr<VesselGraphics> mGeometry;

};

#endif // VESSELMAPOBJECT_H
