#include "firmgraphics.h"

QBrush *FirmGraphics::color = 0;

FirmGraphics::FirmGraphics(float lat, float lon)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(lon, lat), QSizeF(40.0, 40.0), 11, 7, 17),
      mLat(lat), mLon(lon)
{
    if (color == 0)
        color = new QBrush(Qt::darkYellow);

    setNonlinearZoomFactor(0.9);
}

void FirmGraphics::updated(float lat, float lon)
{
    mLat = lat; mLon = lon;
    setCoord(qmapcontrol::PointWorldCoord(mLon, mLat));
    emit positionChanged(this);
    emit requestRedraw();
}

void FirmGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(*color);
    painter.drawEllipse(-20, -20, 40, 40);
}
