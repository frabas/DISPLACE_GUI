#ifndef HARBOURGRAPHICS_H
#define HARBOURGRAPHICS_H

#include <GeometryPointImageScaled.h>
#include <memory>

QT_BEGIN_NAMESPACE
class QPixmap;
QT_END_NAMESPACE

class HarbourGraphics : public qmapcontrol::GeometryPointImageScaled
{
    static std::shared_ptr<QPixmap> symbol;
    float mLat, mLon;
public:
    HarbourGraphics(float lat, float lon);
};

#endif // HARBOURGRAPHICS_H
