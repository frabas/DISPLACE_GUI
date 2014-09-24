#ifndef NODEMAPOBJECT_H
#define NODEMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <QMapControl/GeometryPointCircle.h>
#include <Node.h>


class NodeMapObject : public MapObject
{
public:
    NodeMapObject(Node *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

private:
    Node *mNode;
    std::shared_ptr<qmapcontrol::GeometryPointCircle> mGeometry;

};

#endif // NODEMAPOBJECT_H
