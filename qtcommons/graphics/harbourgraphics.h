#ifndef HARBOURGRAPHICS_H
#define HARBOURGRAPHICS_H

#include <qtcommons_global.h>
#include <GeometryPointImageScaled.h>
#include <memory>

QT_BEGIN_NAMESPACE
class QPixmap;
QT_END_NAMESPACE

class QTCOMMONSSHARED_EXPORT HarbourGraphics : public qmapcontrol::GeometryPointImageScaled
{
    static std::shared_ptr<QPixmap> symbol;
    float mLat, mLon;
public:
    HarbourGraphics(float lat, float lon);
};

#endif // HARBOURGRAPHICS_H
