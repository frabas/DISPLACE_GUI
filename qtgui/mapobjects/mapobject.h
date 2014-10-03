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
};

#endif // MAPOBJECT_H
