#include "harbourmapobject.h"

#include <mapobjectscontroller.h>

#include <QMapControl/QMapControl.h>

QPixmap *HarbourMapObject::symbol = 0;

HarbourMapObject::HarbourMapObject(MapObjectsController *controller, HarbourData *harbour)
    : mController(controller),
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

bool HarbourMapObject::clicked()
{
    if (!mWidget) {
        mWidget = new NodeDetailsWidget(mController->mapWidget());
        connect (mWidget, SIGNAL(destroyed()), this, SLOT(widgetClosed()));
    }

    update();
    mController->showDetailsWidget(mGeometry->coord(),mWidget);

    return true;
}

void HarbourMapObject::update()
{
    if (!mWidget)
        return;

    QString text = QString("<b>Name</b>: %1<br/>"
                           "<b>Coords: </b>%2 %3<br/>")
            .arg(QString::fromStdString(mHarbour->mHarbour->get_name()))
            .arg(mHarbour->mHarbour->get_y())
            .arg(mHarbour->mHarbour->get_x());

    text += "<br/>";
    /*
    text += QString("<b>Fuel:</b> %1<br/>").arg(mVessel->mVessel->get_cumfuelcons());
    text += QString("<b>State:</b> %1<br/>").arg(mVessel->mVessel->get_state());
    text += QString("<b>Cum Catches:</b> %1<br/>").arg(mVessel->mVessel->get_cumcatches());
    text += QString("<b>Time at sea:</b> %1<br/>").arg(mVessel->mVessel->get_timeatsea());
    text += QString("<b>Reason To go Back:</b> %1<br/>").arg(mVessel->mVessel->get_reason_to_go_back());
*/
    mWidget->setText(text);
}

void HarbourMapObject::widgetClosed()
{
    mWidget = 0;
}
