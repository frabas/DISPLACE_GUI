#ifndef FISHFARMMAPOBJECT_H
#define FISHFARMMAPOBJECT_H

#include <GeometryPointShapeScaled.h>

class FishfarmGraphics : public qmapcontrol::GeometryPointShapeScaled {
    static QBrush *color;

    float mLat, mLon;
public:
    FishfarmGraphics (float lat, float lon);

    void updated(float lat, float lon);
protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

#endif // FISHFARMMAPOBJECT_H
