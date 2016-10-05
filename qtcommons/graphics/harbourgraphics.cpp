#include "harbourgraphics.h"

#include <QPixmap>

std::shared_ptr<QPixmap> HarbourGraphics::symbol;

HarbourGraphics::HarbourGraphics(float lat, float lon)
    : qmapcontrol::GeometryPointImageScaled(qmapcontrol::PointWorldCoord(lon, lat),QPixmap(), 10, 9, 17)
{
    if (symbol == nullptr)
        symbol = std::make_shared<QPixmap>(":/icons/harbour.png");

    setImage(symbol);
}
