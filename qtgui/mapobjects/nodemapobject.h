#ifndef NODEMAPOBJECT_H
#define NODEMAPOBJECT_H

#include <mapobjects/mapobject.h>
#include <modelobjects/nodedata.h>

#include <QMapControl/GeometryPointShapeScaled.h>

#include <QBrush>

namespace qmapcontrol {
    class RectWorldCoord;
}

class NodeMapObject : public MapObject
{

    class NodeGraphics : public qmapcontrol::GeometryPointShapeScaled {
        static const Qt::GlobalColor colors[];
        QColor c;

        NodeData *mNode;
    public:
        NodeGraphics (NodeData *node);
    protected:
        virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
    };


public:
    NodeMapObject(NodeData *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

private:
    NodeData *mNode;
    std::shared_ptr<NodeGraphics> mGeometry;

};

#endif // NODEMAPOBJECT_H
