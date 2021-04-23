//
// Created by happycactus on 01/08/20.
//

#ifndef QMAPCONTROL_LAYERRASTER_H
#define QMAPCONTROL_LAYERRASTER_H

#include "Layer.h"

#include "utils/spimpl.h"

namespace qmapcontrol {

class AdapterRaster;

class QMAPCONTROL_EXPORT LayerRaster : public Layer {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;

public:
    explicit LayerRaster(std::string layername);

    void addRaster(std::shared_ptr<AdapterRaster> adapter);

    bool mousePressEvent(const QMouseEvent *mouse_event, const PointWorldCoord &mouse_point_coord,
                         const int &controller_zoom) const override;

    void draw(QPainter &painter, const RectWorldPx &backbuffer_rect_px, const int &controller_zoom) const override;
};

}

#endif // QMAPCONTROL_LAYERRASTER_H
