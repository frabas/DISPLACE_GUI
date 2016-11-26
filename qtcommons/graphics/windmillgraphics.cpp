#include "windmillgraphics.h"

QBrush *WindmillGraphics::color = 0;

WindmillGraphics::WindmillGraphics(float lat, float lon)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(lon, lat), QSizeF(40.0, 40.0), 11, 7, 17),
      mLat(lat), mLon(lon)
{
    if (color == 0)
        color = new QBrush(Qt::darkYellow);

    setNonlinearZoomFactor(0.9);
}

void WindmillGraphics::updated(float lat, float lon)
{
    mLat = lat; mLon = lon;
    setCoord(qmapcontrol::PointWorldCoord(mLon, mLat));
    emit positionChanged(this);
    emit requestRedraw();
}

void WindmillGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(*color);
    painter.drawEllipse(-20, -20, 40, 40);
}
