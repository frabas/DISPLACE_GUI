#ifndef FISHFARMMAPMODEL_H
#define FISHFARMMAPMODEL_H

#include <vector>
#include <functional>

#include <mapcontrolgraphicsmodel.h>

namespace qmapcontrol { class LayerGeometry; class GeometryPointShape; }

class SinglePointMapModel : public MapControlGraphicsModel
{
public:
    explicit SinglePointMapModel(qmapcontrol::QMapControl *map);

    void addGraphicsData(int row, float lat, float lon) override;
    void updateGraphicsData(int row, float lat, float lon) override;

    using GeometryBuilder = std::function<std::shared_ptr<qmapcontrol::GeometryPointShape>(float lat, float lon)>;
    void setGeometryBuilder (GeometryBuilder builder);
private:
    GeometryBuilder mGeometryBuilder;
    std::vector<std::shared_ptr<qmapcontrol::GeometryPointShape>> mGraphics;
    std::shared_ptr<qmapcontrol::LayerGeometry> mGraphicsLayer;    
};

#endif // FISHFARMMAPMODEL_H
