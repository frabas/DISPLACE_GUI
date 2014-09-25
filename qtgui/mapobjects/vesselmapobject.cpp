#include "vesselmapobject.h"

#include <QMapControl/Point.h>
#include <QMapControl/Projection.h>
#include <m_constants.h>

#include <QPainter>
#include <QDebug>

VesselMapObject::VesselMapObject(Vessel *vessel)
    : mVessel(vessel)
{
    mGeometry = std::shared_ptr<VesselGraphics> (new VesselGraphics(mVessel));
}

void VesselMapObject::vesselUpdated()
{
    mGeometry->updated();
}


QBrush *VesselMapObject::VesselGraphics::color = 0;
QBrush *VesselMapObject::VesselGraphics::altColor = 0;

VesselMapObject::VesselGraphics::VesselGraphics(Vessel *vessel)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(vessel->get_x(), vessel->get_y()), QSizeF(20.0, 40.0), 11, 9, 17),
      mVessel(vessel)
{
    if (color == 0)
        color = new QBrush(Qt::darkYellow);
    if (altColor == 0)
        altColor = new QBrush(Qt::red);
}

void VesselMapObject::VesselGraphics::updated()
{
    setCoord(qmapcontrol::PointWorldCoord(mVessel->get_x(), mVessel->get_y()));
    emit positionChanged(this);
    emit requestRedraw();
}

void VesselMapObject::VesselGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.rotate(mVessel->get_course() / M_PI * 180.0);

    painter.setBrush(*color);
    painter.drawEllipse(-10, -20, 20, 40);

    // Ears
    painter.setBrush(mVessel->get_state() == 3 ? *color : *altColor);
    painter.drawEllipse(-6, -12, 16, 16);

    painter.rotate(-mVessel->get_course() / M_PI * 180.0);
}
