#ifndef SHIPLANESGRAPHICS_H
#define SHIPLANESGRAPHICS_H

#include <qtcommons_global.h>
#include <GeometryLineString.h>
#include <memory>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class QTCOMMONSSHARED_EXPORT ShipLanesGraphics : public qmapcontrol::GeometryLineString
{
public:
    ShipLanesGraphics();
};

#endif // SHIPLANESGRAPHICS_H
