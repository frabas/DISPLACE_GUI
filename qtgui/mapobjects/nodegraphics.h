/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef NODEGRAPHICS_H
#define NODEGRAPHICS_H

#include <QMapControl/GeometryPointShapeScaled.h>
#include <modelobjects/nodedata.h>
#include <modeltypes.h>

#include <QBrush>
#include <QSettings>

class MapObjectsController;

class NodeGraphics : public qmapcontrol::GeometryPointShapeScaled {
protected:
    QColor c;

    NodeData *mNode;
    MapObjectsController *mController;
    int mModelIndex;
    QSizeF mGrid;

    std::shared_ptr<types::EnvironmentData> getEnvtData();
public:
    NodeGraphics (NodeData *node, MapObjectsController *controller, int indx);

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);

public:
    static int piew();
    static int pieh();
    static void setPieSize(int w, int h);

private:
    static QSettings *settings;
};

class NodeWithPopStatsGraphics : public NodeGraphics {
public:
    enum Type { Population, Biomass, Impact, CumcatchesPerPop, BenthosBiomass, BenthosNumber, BenthosMeanweight,
              LastType };

    NodeWithPopStatsGraphics(Type type, NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx),
          mType(type)
    {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);

    Type getType() const { return mType; }
    boost::optional<double> getValueForPop(int pop) const;
    QList<int> getInterestingList() const;

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

class NodeWithCumSweptAreaGraphics : public NodeGraphics {
public:
    NodeWithCumSweptAreaGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};


class NodeWithCumSubsurfaceSweptAreaGraphics : public NodeGraphics {
public:
    NodeWithCumSubsurfaceSweptAreaGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};


class NodeWithCumCatchesGraphics : public NodeGraphics {
public:
    NodeWithCumCatchesGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithCumCatchesWithThresholdGraphics : public NodeGraphics {
public:
    NodeWithCumCatchesWithThresholdGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithCumDiscardsGraphics : public NodeGraphics {
public:
    NodeWithCumDiscardsGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};


class NodeWithCumDiscardsRatioGraphics : public NodeGraphics {
public:
    NodeWithCumDiscardsRatioGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithTariffs0Graphics : public NodeGraphics {
public:
    NodeWithTariffs0Graphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};


class NodeWithTariffs1Graphics : public NodeGraphics {
public:
    NodeWithTariffs1Graphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithTariffs2Graphics : public NodeGraphics {
public:
    NodeWithTariffs2Graphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithSalinityGraphics : public NodeGraphics {
public:
    NodeWithSalinityGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};


class NodeWithSSTGraphics : public NodeGraphics {
public:
    NodeWithSSTGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithWindGraphics : public NodeGraphics {
public:
    NodeWithWindGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};


class NodeWithNitrogenGraphics : public NodeGraphics {
public:
    NodeWithNitrogenGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};


class NodeWithPhosphorusGraphics : public NodeGraphics {
public:
    NodeWithPhosphorusGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithOxygenGraphics : public NodeGraphics {
public:
    NodeWithOxygenGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithDissolvedCarbonGraphics : public NodeGraphics {
public:
    NodeWithDissolvedCarbonGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};

class NodeWithBathymetryGraphics : public NodeGraphics {
public:
    NodeWithBathymetryGraphics(NodeData *node, MapObjectsController *controller, int indx)
        : NodeGraphics(node, controller, indx) {}

protected:
    virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
};


#endif // NODEGRAPHICS_H
