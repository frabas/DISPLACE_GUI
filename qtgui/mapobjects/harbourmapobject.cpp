// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "harbourmapobject.h"

#include <mapobjectscontroller.h>
#include <displacemodel.h>
#include <QMapControl/QMapControl.h>

QPixmap *HarbourMapObject::symbol = 0;

HarbourMapObject::HarbourMapObject(MapObjectsController *controller, DisplaceModel *model, HarbourData *harbour)
    : mController(controller),
      mModel(model),
      mHarbour(harbour),
      mGeometry(),
      mWidget(0)
{
    if (symbol == 0)
        symbol = new QPixmap(":/icons/harbour.png");
    mGeometry = std::shared_ptr<qmapcontrol::GeometryPointImageScaled> (new qmapcontrol::GeometryPointImageScaled(
                                                                       qmapcontrol::PointWorldCoord(
                                                                           mHarbour->mHarbour->get_x(), mHarbour->mHarbour->get_y()
                                                                           ),
                                                                            *symbol, 10, 9, 17
                                                                       ));

    mGeometry->setMetadata(std::string("name"), QString::fromStdString(mHarbour->mHarbour->get_name()));
    mGeometry->setMetadataDisplayed(std::string("name"));

    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

bool HarbourMapObject::showProperties()
{
    if (!mWidget) {
        mWidget = new NodeDetailsWidget(mController->mapWidget());
        connect (mWidget, SIGNAL(destroyed()), this, SLOT(widgetClosed()));
    }

    updateProperties();
    mController->showDetailsWidget(mGeometry->coord(),mWidget);

    return true;
}

void HarbourMapObject::updateProperties()
{
    if (!mWidget)
        return;

    QString text = QString("<b>Name</b>: %1<br/>"
                           "<b>Node Id: %2<br />"
                           "<b>Coords: </b>%3 %4<br/>")
            .arg(QString::fromStdString(mHarbour->mHarbour->get_name()))
            .arg(mHarbour->mHarbour->get_idx_node().toIndex())
            .arg(mHarbour->mHarbour->get_y())
            .arg(mHarbour->mHarbour->get_x());

    text += "<br/>";

    HarbourStats s = mModel->retrieveHarbourIdxStatAtStep(mHarbour->mHarbour->get_idx_node().toIndex(), mModel->getCurrentStep());

    text += QString("<b>Cumul Catches:</b> %1<br/>").arg(s.mCumCatches);
    text += QString("<b>Profit:</b> %1<br/>").arg(s.mCumProfit);

    for (int i = 0; i < s.szCatches.size(); ++i) {
        text += QString("<b>Pop %1 Catches:</b> %2<br/>")
                .arg(i).arg(s.szCatches[i]);
    }

    mWidget->setText(text);
}

void HarbourMapObject::widgetClosed()
{
    mWidget = 0;
}
