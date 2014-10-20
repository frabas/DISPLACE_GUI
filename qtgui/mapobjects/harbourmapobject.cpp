#include "harbourmapobject.h"

QPixmap *HarbourMapObject::symbol = 0;

HarbourMapObject::HarbourMapObject(HarbourData *harbour)
    : mHarbour(harbour),
      mGeometry()
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
}
