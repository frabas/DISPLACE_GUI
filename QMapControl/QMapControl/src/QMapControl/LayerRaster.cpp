//
// Created by happycactus on 01/08/20.
//

#include "LayerRaster.h"
#include "AdapterRaster.h"

namespace qmapcontrol {

struct LayerRaster::Impl {
    std::string layerName;
    std::shared_ptr<AdapterRaster> adapter;
};

LayerRaster::LayerRaster(std::string layername)
        : Layer(Layer::LayerType::LayerUnknown, layername), p(spimpl::make_unique_impl<Impl>())
{
    p->layerName = std::move(layername);
}

void LayerRaster::addRaster(std::shared_ptr<AdapterRaster> adapter)
{
    p->adapter = adapter;
}

void LayerRaster::draw(QPainter &painter, const RectWorldPx &backbuffer_rect_px, const int &controller_zoom) const
{
    painter.save();
    if (isVisible()) {
        p->adapter->draw(painter, backbuffer_rect_px, controller_zoom);
    }
    painter.restore();
}

bool LayerRaster::mousePressEvent(const QMouseEvent *mouse_event, const PointWorldCoord &mouse_point_coord,
                                  const int &controller_zoom) const
{
    // nothing special to do. Simply return false
    return false;
}

}