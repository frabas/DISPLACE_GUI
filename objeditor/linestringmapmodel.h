#ifndef LINESTRINGMAPMODEL_H
#define LINESTRINGMAPMODEL_H

#include <vector>
#include <functional>

#include <mapcontrolgraphicsmodel.h>

namespace qmapcontrol { class GeometryLineString; class LayerGeometry; }

class LineStringMapModel  : public MapControlGraphicsModel
{
public:
    LineStringMapModel(qmapcontrol::QMapControl *);

    void addGraphicsData(int row, int id, float lat, float lon) override;
    void updateGraphicsData(int row, float lat, float lon) override;

    using GeometryBuilder = std::function<std::shared_ptr<qmapcontrol::GeometryLineString>(float lat, float lon)>;
    void setGeometryBuilder (GeometryBuilder builder);
private:
    struct LaneData {
        int lane;
        int coordid;
    };

    GeometryBuilder mGeometryBuilder;

    std::vector<std::shared_ptr<qmapcontrol::GeometryLineString>> mGraphics;
    std::vector<LaneData> mLaneData;
    std::shared_ptr<qmapcontrol::LayerGeometry> mGraphicsLayer;
};

#endif // LINESTRINGMAPMODEL_H
