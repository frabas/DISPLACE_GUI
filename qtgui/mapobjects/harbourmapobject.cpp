#include "harbourmapobject.h"

QPixmap *HarbourMapObject::symbol = 0;

HarbourMapObject::HarbourMapObject(Harbour *harbour)
    : mHarbour(harbour),
      mGeometry()
{
    if (symbol == 0)
        symbol = new QPixmap(":/icons/harbour.png");
    mGeometry = std::shared_ptr<qmapcontrol::GeometryPointImageScaled> (new qmapcontrol::GeometryPointImageScaled(
                                                                       qmapcontrol::PointWorldCoord(
                                                                           mHarbour->get_x(), mHarbour->get_y()
                                                                           ),
                                                                            *symbol, 10, 9, 17
                                                                       ));

    mGeometry->setMetadata(std::string("name"), QString::fromStdString(mHarbour->get_name()));
    mGeometry->setMetadataDisplayed(std::string("name"));
}
