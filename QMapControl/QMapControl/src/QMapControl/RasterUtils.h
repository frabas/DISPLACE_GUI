//
// Created by happycactus on 07/08/20.
//

#ifndef QMAPCONTROL_RASTERUTILS_H
#define QMAPCONTROL_RASTERUTILS_H

#include "qmapcontrol_global.h"

#include <QImage>

class GDALDataset;

class QMAPCONTROL_EXPORT RasterUtils {
public:
    QImage imageFromRaster(GDALDataset *datasource);
};


#endif //QMAPCONTROL_RASTERUTILS_H
