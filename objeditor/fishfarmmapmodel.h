#ifndef FISHFARMMAPMODEL_H
#define FISHFARMMAPMODEL_H

#include <vector>

#include <mapcontrolgraphicsmodel.h>

namespace qmapcontrol { class LayerGeometry; }

class FishfarmGraphics;

class FishfarmMapModel : public MapControlGraphicsModel
{
public:
    explicit FishfarmMapModel(qmapcontrol::QMapControl *map);

    void addGraphicsData(int id, float lat, float lon) override;

private:
    std::vector<std::shared_ptr<FishfarmGraphics>> mGraphics;
    std::shared_ptr<qmapcontrol::LayerGeometry> mGraphicsLayer;
};

#endif // FISHFARMMAPMODEL_H
