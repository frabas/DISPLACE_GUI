#include "singlepointmapmodel.h"

#include <QMapControl.h>
#include <LayerGeometry.h>
#include <GeometryPointShape.h>

#include <graphics/fishfarmgraphics.h>

SinglePointMapModel::SinglePointMapModel(qmapcontrol::QMapControl *map)
    : MapControlGraphicsModel(map)
{
    mGraphicsLayer = std::make_shared<qmapcontrol::LayerGeometry>("Geometry");
    map->addLayer(mGraphicsLayer);
}

void SinglePointMapModel::addGraphicsData(int row, float lat, float lon)
{
    auto gr = mGeometryBuilder(lat, lon);
    while (mGraphics.size() <= (size_t)row)
        mGraphics.push_back(nullptr);
    mGraphics[row] = gr;
    mGraphicsLayer->addGeometry(gr);
}

void SinglePointMapModel::setGeometryBuilder(SinglePointMapModel::GeometryBuilder builder)
{
    mGeometryBuilder = builder;
}


void SinglePointMapModel::updateGraphicsData(int row, float lat, float lon)
{
    mGraphicsLayer->removeGeometry(mGraphics[row]);
    mGraphics[row]->setCoord(qmapcontrol::PointWorldCoord(lon, lat));
    mGraphicsLayer->addGeometry(mGraphics[row]);
}
