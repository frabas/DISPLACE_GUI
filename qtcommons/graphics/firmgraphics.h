#ifndef FIRMGRAPHICS_H
#define FIRMGRAPHICS_H

#include <qtcommons_global.h>
#include <GeometryPointShapeScaled.h>

class QTCOMMONSSHARED_EXPORT FirmGraphics : public qmapcontrol::GeometryPointShapeScaled {
    static QBrush *color;

    float mLat, mLon;
public:
    FirmGraphics (float lat, float lon);

    void updated(float lat, float lon);
protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

#endif // FIRMGRAPHICS_H
