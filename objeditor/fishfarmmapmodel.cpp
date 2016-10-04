#include "fishfarmmapmodel.h"

#include <QMapControl.h>
#include <LayerGeometry.h>

#include <graphics/fishfarmgraphics.h>

FishfarmMapModel::FishfarmMapModel(qmapcontrol::QMapControl *map)
    : MapControlGraphicsModel(map)
{
    mGraphicsLayer = std::make_shared<qmapcontrol::LayerGeometry>("Geometry");
    map->addLayer(mGraphicsLayer);
}

void FishfarmMapModel::addGraphicsData(int id, float lat, float lon)
{
    auto gr = std::make_shared<FishfarmGraphics>(lat, lon);
    while (mGraphics.size() <= (size_t)id)
        mGraphics.push_back(nullptr);
    mGraphics[id] = gr;
    mGraphicsLayer->addGeometry(gr);
}

