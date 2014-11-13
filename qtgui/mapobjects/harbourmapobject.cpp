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
            .arg(mHarbour->mHarbour->get_idx_node())
            .arg(mHarbour->mHarbour->get_y())
            .arg(mHarbour->mHarbour->get_x());

    text += "<br/>";

    HarbourStats s = mModel->retrieveHarbourIdxStatAtStep(mHarbour->mHarbour->get_idx_node(), mModel->getCurrentStep());

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
