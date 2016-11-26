#ifndef WINDMILLMAPOBJECT_H
#define WINDMILLMAPOBJECT_H

#include <GeometryPointShapeScaled.h>

class WindmillGraphics : public qmapcontrol::GeometryPointShapeScaled {
    static QBrush *color;

    float mLat, mLon;
public:
    WindmillGraphics (float lat, float lon);

    void updated(float lat, float lon);
protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

#endif // WINDMILLMAPOBJECT_H
