#ifndef NODEGRAPHICS_H
#define NODEGRAPHICS_H

#include <QMapControl/GeometryPointShapeScaled.h>
#include <modelobjects/nodedata.h>

#include <QBrush>

class MapObjectsController;

class NodeGraphics : public qmapcontrol::GeometryPointShapeScaled {
protected:
    QColor c;

    NodeData *mNode;
    MapObjectsController *mController;
    int mModelIndex;
    QSizeF mGrid;
public:
    NodeGraphics (NodeData *node, MapObjectsController *controller, int indx);

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithPopStatsGraphics : public NodeGraphics {
public:
    enum Type { Population, Biomass, Impact };

    NodeWithPopStatsGraphics(Type type, NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx),
          mType(type)
    {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);

    Type getType() const { return mType; }
    double getValueForPop(int pop) const;

private:
    Type mType;
};

class NodeWithCumFTimeGraphics : public NodeGraphics {
public:
    NodeWithCumFTimeGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

#endif // NODEGRAPHICS_H
