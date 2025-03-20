// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "shipmapobject.h"

#include <mapobjectscontroller.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/Point.h>
#include <QMapControl/Projection.h>
#include <QMapControl/LayerGeometry.h>

#include <m_constants.h>

#include <QPainter>
#include <QDebug>

ShipMapObject::ShipMapObject(MapObjectsController *controller, ShipData *ship)
    : mController(controller),
      mShip(ship),
      mWidget(0)
{
    mGeometry = std::shared_ptr<ShipGraphics> (new ShipGraphics(mShip));

    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

bool ShipMapObject::showProperties()
{
    if (!mWidget) {
        mWidget = new NodeDetailsWidget(mController->mapWidget());
        connect (mWidget, SIGNAL(destroyed()), this, SLOT(widgetClosed()));
    }

    updateProperties();
    mController->showDetailsWidget(mGeometry->coord(),mWidget);

    return true;
}

void ShipMapObject::updateProperties()
{
    if (!mWidget)
        return;

    QString text = QString("<b>Name</b>: %1<br/>"
                           "<b>Coords: </b>%2 %3<br/>")
            .arg(QString::fromStdString(mShip->mShip->get_name()))
            .arg(mShip->mShip->get_y())
            .arg(mShip->mShip->get_x());

    text += "<br/>";
    text += QString("<b>Type:</b> %1<br/>").arg(QString::fromStdString(mShip->mShip->get_type()));
    text += QString("<b>Flag:</b> %1<br/>").arg(QString::fromStdString(mShip->mShip->get_flag()));
    text += QString("<b>IMO:</b> %1<br/>").arg(mShip->mShip->get_imo());
    text += QString("<b>Year Build:</b> %1<br/>").arg(mShip->mShip->get_yearbuild());
    text += QString("<b>GT:</b> %1<br/>").arg(mShip->mShip->get_grosstonnage());
    text += QString("<b>LOA:</b> %1<br/>").arg(mShip->mShip->get_loa());
    text += QString("<b>KW:</b> %1<br/>").arg(mShip->mShip->get_KW());
    text += QString("<b>Breadth:</b> %1<br/>").arg(mShip->mShip->get_breadth());
    text += QString("<b>Number Units:</b> %1<br/>").arg(mShip->mShip->get_nbunits());
    text += QString("<b>Fuel use (ton/h):</b> %1<br/>").arg(mShip->mShip->get_fueluse());
    text += QString("<b>NOx emission (g/KWh):</b> %1<br/>").arg(mShip->mShip->get_NOxEmission_gperKWh());
    text += QString("<b>SOx emission (%m/m):</b> %1<br/>").arg(mShip->mShip->get_SOxEmission_percentpertotalfuelmass());
    text += QString("<b>GHG emission (ton/h):</b> %1<br/>").arg(mShip->mShip->get_GHGEmission());
    text += QString("<b>PM emission(ton/h; PM2.5):</b> %1<br/>").arg(mShip->mShip->get_PMEEmission());
    text += QString("<b>Max Speed:</b> %1<br/>").arg(mShip->mShip->get_vmax());

    mWidget->setText(text);
}

void ShipMapObject::shipUpdated()
{
    mGeometry->layer()->removeGeometry(mGeometry);
    mGeometry->setCoord(qmapcontrol::PointWorldCoord(mShip->mShip->get_x(), mShip->mShip->get_y()));
    mGeometry->layer()->addGeometry(mGeometry);
}

void ShipMapObject::widgetClosed()
{
    mWidget = 0;
}


QBrush *ShipMapObject::ShipGraphics::color = 0;

ShipMapObject::ShipGraphics::ShipGraphics(ShipData *ship)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(ship->mShip->get_x(), ship->mShip->get_y()), QSizeF(20.0, 40.0), 11, 7, 17),
      mShip(ship)
{
    if (color == 0)
        color = new QBrush(Qt::black);

    setNonlinearZoomFactor(0.9);
}

void ShipMapObject::ShipGraphics::updated()
{
    setCoord(qmapcontrol::PointWorldCoord(mShip->mShip->get_x(), mShip->mShip->get_y()));
    emit positionChanged(this);
    emit requestRedraw();
}

void ShipMapObject::ShipGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.rotate(mShip->mShip->get_course());

    painter.setBrush(*color);
    painter.drawEllipse(-10, -20, 20, 40);

    painter.rotate(-mShip->mShip->get_course());
}
