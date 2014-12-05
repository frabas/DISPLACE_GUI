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
#ifndef EDGEMAPOBJECT_H
#define EDGEMAPOBJECT_H

#include <QObject>

#include <mapobjects/mapobject.h>
#include <modelobjects/nodedata.h>
#include <mapobjects/nodegraphics.h>
#include <QMapControl/GeometryLineString.h>
#include <QMapControl/Geometry.h>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;
class NodeDetailsWidget;

class EdgeGraphics;

class EdgeGraphics : public qmapcontrol::GeometryLineString {
    static QPen mNormalPen, mSelectedPen;

    NodeData *node;
    int edgeIdx;
public:
    explicit EdgeGraphics(const std::vector<qmapcontrol::PointWorldCoord>& points, NodeData *nd, int edge);

    virtual void draw(QPainter& painter, const qmapcontrol::RectWorldCoord& backbuffer_rect_coord, const int& controller_zoom);

    static const double minZoom, maxZoom, minTextZoom;

private:
    void buildArrow(const qmapcontrol::PointWorldCoord& from, const qmapcontrol::PointWorldCoord &to);
};

class EdgeMapObject : public QObject, public MapObject
{
    Q_OBJECT

public:
    EdgeMapObject(MapObjectsController *controller, int indx, NodeData *node);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    void onSelectionChanged() override;

    bool selected() const { return mGeometry->selected(); }

    NodeData* node() const { return mNode; }
    NodeData* target() const { return mTarget; }
protected:

private slots:

signals:
    void edgeSelectionHasChanged(EdgeMapObject *object);

private:

    MapObjectsController *mController;
    NodeData* mNode, *mTarget;

    std::shared_ptr<EdgeGraphics> mGeometry;
    NodeDetailsWidget *mWidget;
};

#endif // EDGEMAPOBJECT_H
