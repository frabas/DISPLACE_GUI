#ifndef SHIPLANESGRAPHICS_H
#define SHIPLANESGRAPHICS_H

#include <GeometryLineString.h>
#include <memory>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class ShipLanesGraphics : public qmapcontrol::GeometryLineString
{
public:
    ShipLanesGraphics();
};

#endif // SHIPLANESGRAPHICS_H
