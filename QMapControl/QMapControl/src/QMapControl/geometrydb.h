#ifndef GEOMETRYDB_H
#define GEOMETRYDB_H

#include <memory>
#include <set>
#include "Point.h"

namespace qmapcontrol {

class Geometry;
class RectWorldCoord;

namespace impl {
class GeometryDBImpl;
}

class GeometryDB
{
    std::shared_ptr<impl::GeometryDBImpl> impl;
public:
    using GeometryPtr = std::shared_ptr<Geometry>;

    GeometryDB();

    void query(std::set<GeometryPtr>& return_points, const RectWorldCoord& range_coord) const;
    bool insert(const PointWorldCoord& point_coord, const GeometryPtr& object);
    void erase(const PointWorldCoord& point_coord, const GeometryPtr& object);
    void clear();
};

}

#endif // GEOMETRYDB_H
