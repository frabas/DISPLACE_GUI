#ifndef NODEGRAPHICS_H
#define NODEGRAPHICS_H

#include <QMapControl/GeometryPointShapeScaled.h>
#include <modelobjects/nodedata.h>

#include <QBrush>

class NodeGraphics : public qmapcontrol::GeometryPointShapeScaled {
protected:
    static const Qt::GlobalColor colors[];
    QColor c;

    NodeData *mNode;
public:
    NodeGraphics (NodeData *node);

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithPopStatsGraphics : public NodeGraphics {
public:
    NodeWithPopStatsGraphics(NodeData *node)
        : NodeGraphics(node) {}
protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithCumFTimeGraphics : public NodeGraphics {
public:
    NodeWithCumFTimeGraphics(NodeData *node)
        : NodeGraphics(node) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

#endif // NODEGRAPHICS_H
