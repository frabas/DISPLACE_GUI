// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2018 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "edgemapobject.h"

#include <mapobjectscontroller.h>
#include <displacemodel.h>
#include <QMapControl/Projection.h>

#include <QMessageBox>
#include <QPen>

QPen EdgeGraphics::mNormalPen(Qt::black);
QPen EdgeGraphics::mSelectedPen (QBrush(Qt::red), 3, Qt::SolidLine);

namespace {
    QPen HighlightedPen (QBrush(Qt::yellow), 3, Qt::SolidLine);
}

const double EdgeGraphics::minZoom = 7;
const double EdgeGraphics::maxZoom = 17;
const double EdgeGraphics::minTextZoom = 11;

EdgeMapObject::EdgeMapObject(MapObjectsController *controller, std::shared_ptr<NodeData::Edge> edge)
    : QObject(),
      mController(controller),
      mEdge(edge)
{
    // use of member functions is not recommended.
    std::shared_ptr<NodeData> src = mEdge->source.lock();
    std::shared_ptr<NodeData> tgt = mEdge->target.lock();

    std::vector<qmapcontrol::PointWorldCoord> line;
    line.push_back(qmapcontrol::PointWorldCoord(src->get_x(), src->get_y()));
    line.push_back(qmapcontrol::PointWorldCoord(tgt->get_x(), tgt->get_y()));

    mGeometry = std::shared_ptr<EdgeGraphics>(new EdgeGraphics(line, mEdge));
    mGeometry->setFlags(qmapcontrol::Geometry::IsSelectable);
    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

EdgeGraphics::EdgeGraphics(const std::vector<PointWorldCoord> &points, std::shared_ptr<NodeData::Edge> edge)
    : qmapcontrol::GeometryLineString(minZoom, maxZoom),
      mEdge(edge)
{
    buildArrow(points[0], points[1]);

    setPen(mNormalPen);
}

void EdgeGraphics::draw(QPainter &painter, const qmapcontrol::RectWorldCoord &backbuffer_rect_coord, const int &controller_zoom)
{
    if (mEdge->source.expired() || mEdge->target.expired())
        return;

    if (!selected()) {
        if (mEdge->highlighted) {
            setPen(HighlightedPen);
        } else {
            setPen(mNormalPen);
        }
    } else {
        setPen(mSelectedPen);
    }

    qmapcontrol::GeometryLineString::draw(painter, backbuffer_rect_coord, controller_zoom);
    if (controller_zoom >= minTextZoom) {
        QPointF p1 = projection::get().toPointWorldPx(m_points[0], controller_zoom).rawPoint();
        QPointF p2 = projection::get().toPointWorldPx(m_points[1], controller_zoom).rawPoint();

        QPointF mid (p1.x() + 0.3 * (p2.x() - p1.x()) , p1.y() + 0.3 * (p2.y() - p1.y()));

        painter.drawText(mid, QString::number(mEdge->weight));
    }
}

void EdgeGraphics::buildArrow(const qmapcontrol::PointWorldCoord &to, const qmapcontrol::PointWorldCoord &from)
{
    double dx = to.longitude() - from.longitude();
    double dy = to.latitude() - from.latitude();
    double len = std::sqrt(dx*dx + dy*dy);

    qmapcontrol::PointWorldCoord arrowHead;
    arrowHead.setLongitude(from.longitude() + 0.25 * dx);
    arrowHead.setLatitude(from.latitude() + 0.25 * dy);

    double angle = ::acos(dx / len);

    double arrowSize = 0.01; // len / 10;
    if (dy >= 0)
        angle = (M_PI * 2) - angle;

    qmapcontrol::PointWorldCoord a1;
    a1.setLongitude(arrowHead.longitude() + sin(angle + M_PI / 3) * arrowSize);
    a1.setLatitude(arrowHead.latitude() + cos(angle + M_PI / 3) * arrowSize);
    qmapcontrol::PointWorldCoord a2;
    a2.setLongitude(arrowHead.longitude() + sin(angle + M_PI - M_PI / 3) * arrowSize);
    a2.setLatitude(arrowHead.latitude() + cos(angle + M_PI - M_PI / 3) * arrowSize);

    addPoint(from);
    addPoint(arrowHead);
    addPoint(a1);
    addPoint(a2);
    addPoint(arrowHead);
    addPoint(to);
}
