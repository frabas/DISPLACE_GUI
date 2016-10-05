#include "singlepointmapmodel.h"

#include <QMapControl.h>
#include <LayerGeometry.h>

#include <graphics/fishfarmgraphics.h>

SinglePointMapModel::SinglePointMapModel(qmapcontrol::QMapControl *map)
    : MapControlGraphicsModel(map)
{
    mGraphicsLayer = std::make_shared<qmapcontrol::LayerGeometry>("Geometry");
    map->addLayer(mGraphicsLayer);
}

void SinglePointMapModel::addGraphicsData(int id, float lat, float lon)
{
    auto gr = mGeometryBuilder(lat, lon);
    while (mGraphics.size() <= (size_t)id)
        mGraphics.push_back(nullptr);
    mGraphics[id] = gr;
    mGraphicsLayer->addGeometry(gr);
}

void SinglePointMapModel::setGeometryBuilder(SinglePointMapModel::GeometryBuilder builder)
{
    mGeometryBuilder = builder;
}
