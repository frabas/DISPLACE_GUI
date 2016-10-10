#ifndef MAPCONTROLGRAPHICSMODEL_H
#define MAPCONTROLGRAPHICSMODEL_H

#include <memory>

namespace qmapcontrol { class QMapControl; }

class MapControlGraphicsModel
{
public:
    explicit MapControlGraphicsModel(qmapcontrol::QMapControl *);
    MapControlGraphicsModel(const MapControlGraphicsModel &) = default;
    MapControlGraphicsModel(MapControlGraphicsModel &&) = default;

    virtual ~MapControlGraphicsModel() = default;

    MapControlGraphicsModel &operator =(const MapControlGraphicsModel &) = default;
    MapControlGraphicsModel &operator =(MapControlGraphicsModel &&) = default;

    virtual void addGraphicsData (int row, int id, float lat, float lon) = 0;
    virtual void updateGraphicsData (int row, float lat, float lon) = 0;
public:
    qmapcontrol::QMapControl *map() const { return mMap; }

private:
    qmapcontrol::QMapControl *mMap;
};

#endif // MAPCONTROLGRAPHICSMODEL_H
