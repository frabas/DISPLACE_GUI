#include "vesselmapobject.h"

#include <QMapControl/Point.h>
#include <QMapControl/Projection.h>
#include <QMapControl/LayerGeometry.h>

#include <m_constants.h>

#include <QPainter>
#include <QDebug>

VesselMapObject::VesselMapObject(VesselData *vessel)
    : mVessel(vessel)
{
    mGeometry = std::shared_ptr<VesselGraphics> (new VesselGraphics(mVessel));
}

void VesselMapObject::vesselUpdated()
{
    mGeometry->layer()->removeGeometry(mGeometry);
    mGeometry->setCoord(qmapcontrol::PointWorldCoord(mVessel->mVessel->get_x(), mVessel->mVessel->get_y()));
    mGeometry->layer()->addGeometry(mGeometry);
}


QBrush *VesselMapObject::VesselGraphics::color = 0;
QBrush *VesselMapObject::VesselGraphics::statFishing= 0;
QBrush *VesselMapObject::VesselGraphics::statHarbour = 0;
QBrush *VesselMapObject::VesselGraphics::statSteaming = 0;

VesselMapObject::VesselGraphics::VesselGraphics(VesselData *vessel)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(vessel->mVessel->get_x(), vessel->mVessel->get_y()), QSizeF(20.0, 40.0), 11, 7, 17),
      mVessel(vessel)
{
    if (color == 0)
        color = new QBrush(Qt::darkYellow);
    if (statHarbour == 0)
        statHarbour = new QBrush(Qt::green);
    if (statFishing == 0)
        statFishing = new QBrush(Qt::red);
    if (statSteaming == 0)
        statSteaming = new QBrush(Qt::black);

    setNonlinearZoomFactor(0.9);
}

void VesselMapObject::VesselGraphics::updated()
{
    setCoord(qmapcontrol::PointWorldCoord(mVessel->mVessel->get_x(), mVessel->mVessel->get_y()));
    emit positionChanged(this);
    emit requestRedraw();
}

void VesselMapObject::VesselGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.rotate(mVessel->mVessel->get_course());

    painter.setBrush(*color);
    painter.drawEllipse(-10, -20, 20, 40);

    // Ears
    switch (mVessel->mVessel->get_state()) {
    case 1:
        painter.setBrush(*statFishing);
        break;
    case 2:
        painter.setBrush(*statSteaming);
        break;
    default:
    case 3:
        painter.setBrush(*statHarbour);
        break;
    }

    painter.drawEllipse(-6, -12, 12, 24);

    painter.rotate(-mVessel->mVessel->get_course());
}
