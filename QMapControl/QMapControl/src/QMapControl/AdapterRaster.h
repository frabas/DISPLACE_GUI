//
// Created by happycactus on 01/08/20.
//

#ifndef QMAPCONTROL_ADAPTERRASTER_H
#define QMAPCONTROL_ADAPTERRASTER_H

#include "utils/spimpl.h"
#include "Point.h"

#include <gdal_priv.h>
#include <QObject>

QT_BEGIN_NAMESPACE
class QPainter;

QT_END_NAMESPACE

namespace qmapcontrol {
class RectWorldPx;

class QMAPCONTROL_EXPORT AdapterRaster : public QObject {
Q_OBJECT
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:

    explicit AdapterRaster(GDALDataset *datasource, OGRSpatialReference *spatialReference,
                           std::string layer_name, QObject *parent = nullptr);

    void draw(QPainter &painter, RectWorldPx const &backbuffer_rect_px, int controller_zoom);

    PointWorldCoord getOrigin() const;

    PointWorldCoord getCenter() const;
};

} // namespace qmapcontrol

#endif // QMAPCONTROL_ADAPTERRASTER_H
