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

    std::shared_ptr<NodeData::Edge> mEdge;
public:
    explicit EdgeGraphics(const std::vector<qmapcontrol::PointWorldCoord>& points, std::shared_ptr<NodeData::Edge> edge);

    virtual void draw(QPainter& painter, const qmapcontrol::RectWorldCoord& backbuffer_rect_coord, const int& controller_zoom);

    static const double minZoom, maxZoom, minTextZoom;

private:
    void buildArrow(const qmapcontrol::PointWorldCoord& from, const qmapcontrol::PointWorldCoord &to);
};

class EdgeMapObject : public QObject, public MapObject
{
    Q_OBJECT

public:
    EdgeMapObject(MapObjectsController *controller, std::shared_ptr<NodeData::Edge> edge);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    void onSelectionChanged() override;

    bool selected() const { return mGeometry->selected(); }

    std::shared_ptr<NodeData> node() const { return mEdge->source.lock(); }
    std::shared_ptr<NodeData> target() const { return mEdge->target.lock(); }
protected:

private slots:

signals:
    void edgeSelectionHasChanged(EdgeMapObject *object);

private:
    MapObjectsController *mController;
    std::shared_ptr<NodeData::Edge> mEdge;

    std::shared_ptr<EdgeGraphics> mGeometry;
    NodeDetailsWidget *mWidget;
};

#endif // EDGEMAPOBJECT_H
