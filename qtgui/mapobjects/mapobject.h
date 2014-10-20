#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <memory>

namespace qmapcontrol {
class Geometry;
}

class MapObject
{
public:
    MapObject();
    virtual ~MapObject();


    virtual std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const = 0;
    virtual bool clicked();
    virtual void update();
};

#endif // MAPOBJECT_H
