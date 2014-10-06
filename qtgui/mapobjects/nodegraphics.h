#ifndef NODEGRAPHICS_H
#define NODEGRAPHICS_H

#include <QMapControl/GeometryPointShapeScaled.h>
#include <modelobjects/nodedata.h>

#include <QBrush>

class MapObjectsController;

class NodeGraphics : public qmapcontrol::GeometryPointShapeScaled {
protected:
    static const Qt::GlobalColor colors[];
    QColor c;

    NodeData *mNode;
    MapObjectsController *mController;
public:
    NodeGraphics (NodeData *node, MapObjectsController *controller);

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithPopStatsGraphics : public NodeGraphics {
public:
    NodeWithPopStatsGraphics(NodeData *node, MapObjectsController *controller)
        : NodeGraphics(node, controller) {}
protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithCumFTimeGraphics : public NodeGraphics {
public:
    NodeWithCumFTimeGraphics(NodeData *node, MapObjectsController *controller)
        : NodeGraphics(node, controller) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

#endif // NODEGRAPHICS_H
