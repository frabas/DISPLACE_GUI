#include "shiplanesgraphics.h"

ShipLanesGraphics::ShipLanesGraphics()
{
    auto brush = std::make_shared<QBrush>(Qt::red);
    setBrush(brush);

    setPen(QPen(Qt::red,1));
}
