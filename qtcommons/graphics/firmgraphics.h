#ifndef FIRMGRAPHICS_H
#define FIRMGRAPHICS_H

#include <GeometryPointShapeScaled.h>

class FirmGraphics : public qmapcontrol::GeometryPointShapeScaled {
    static QBrush *color;

    float mLat, mLon;
public:
    FirmGraphics (float lat, float lon);

    void updated(float lat, float lon);
protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

#endif // FIRMGRAPHICS_H
