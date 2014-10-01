#ifndef NODEMAPOBJECT_H
#define NODEMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <QMapControl/GeometryPointCircle.h>
#include <modelobjects/nodedata.h>


class NodeMapObject : public MapObject
{
public:
    NodeMapObject(NodeData *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

private:
    NodeData *mNode;
    std::shared_ptr<qmapcontrol::GeometryPointCircle> mGeometry;

};

#endif // NODEMAPOBJECT_H
