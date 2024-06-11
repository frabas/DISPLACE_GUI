// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "firmmapobject.h"

#include <mapobjectscontroller.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/Point.h>
#include <QMapControl/Projection.h>
#include <QMapControl/LayerGeometry.h>

#include <m_constants.h>

#include <QPainter>
#include <QDebug>

FirmMapObject::FirmMapObject(MapObjectsController *controller, FirmData *firm)
    : mController(controller),
      mFirm(firm),
      mWidget(0)
{
    mGeometry = std::shared_ptr<FirmGraphics> (new FirmGraphics(mFirm));

    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

bool FirmMapObject::showProperties()
{
    if (!mWidget) {
        mWidget = new NodeDetailsWidget(mController->mapWidget());
        connect (mWidget, SIGNAL(destroyed()), this, SLOT(widgetClosed()));
    }

    updateProperties();
    mController->showDetailsWidget(mGeometry->coord(),mWidget);

    return true;
}

void FirmMapObject::updateProperties()
{
    if (!mWidget)
        return;

    QString text = QString("<b>Name</b>: %1<br/>"
                           "<b>Coords: </b>%2 %3<br/>")
            .arg(QString::number(mFirm->mFirm->get_idx()))
            .arg(mFirm->mFirm->get_y())
            .arg(mFirm->mFirm->get_x());

    text += "<br/>";
    //text += QString("<b>Name again:</b> %1<br/>").arg(mFirm->mFirm->get_name());

    mWidget->setText(text);
}

void FirmMapObject::firmUpdated()
{
    mGeometry->layer()->removeGeometry(mGeometry);
    mGeometry->setCoord(qmapcontrol::PointWorldCoord(mFirm->mFirm->get_x(), mFirm->mFirm->get_y()));
    mGeometry->layer()->addGeometry(mGeometry);
}




void FirmMapObject::widgetClosed()
{
    mWidget = 0;
}


QBrush *FirmMapObject::FirmGraphics::color = 0;

FirmMapObject::FirmGraphics::FirmGraphics(FirmData *firm)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(firm->mFirm->get_x(), firm->mFirm->get_y()), QSizeF(20.0, 40.0), 11, 7, 17),
      mFirm(firm)
{
    if (color == 0)
        color = new QBrush(Qt::darkBlue);

    setNonlinearZoomFactor(0.9);
}

void FirmMapObject::FirmGraphics::updated()
{
    setCoord(qmapcontrol::PointWorldCoord(mFirm->mFirm->get_x(), mFirm->mFirm->get_y()));
    emit positionChanged(this);
    emit requestRedraw();
}

void FirmMapObject::FirmGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);


    painter.setBrush(*color);
    painter.drawEllipse(-10, -20, 20, 40);


}
