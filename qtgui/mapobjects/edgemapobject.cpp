#include "edgemapobject.h"

#include <mapobjectscontroller.h>
#include <displacemodel.h>
#include <QMapControl/Projection.h>

#include <QMessageBox>
#include <QPen>

QPen EdgeGraphics::mNormalPen(Qt::black);
QPen EdgeGraphics::mSelectedPen (QBrush(Qt::red), 3, Qt::SolidLine);

const double EdgeGraphics::minZoom = 7;
const double EdgeGraphics::maxZoom = 17;
const double EdgeGraphics::minTextZoom = 11;

EdgeMapObject::EdgeMapObject(MapObjectsController *controller, int indx, NodeData *node)
    : QObject(),
      mController(controller),
      mNode(node)
{
    std::vector<qmapcontrol::PointWorldCoord> line;
    line.push_back(qmapcontrol::PointWorldCoord(mNode->get_x(), mNode->get_y()));

    mTarget = mNode->getModel()->getNodesList()[mNode->getAdiacencyByIdx(indx)].get();
    line.push_back(qmapcontrol::PointWorldCoord(mTarget->get_x(), mTarget->get_y()));

    mGeometry = std::shared_ptr<EdgeGraphics>(new EdgeGraphics(line, mNode, indx));
    mGeometry->setFlags(qmapcontrol::Geometry::IsSelectable);
    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

void EdgeMapObject::onSelectionChanged()
{
    emit edgeSelectionHasChanged(this);
}

EdgeGraphics::EdgeGraphics(const std::vector<PointWorldCoord> &points, NodeData *nd, int edge)
    : qmapcontrol::GeometryLineString(minZoom, maxZoom),
      node(nd), edgeIdx(edge)
{
    buildArrow(points[0], points[1]);

    setPen(mNormalPen);
}

void EdgeGraphics::draw(QPainter &painter, const qmapcontrol::RectWorldCoord &backbuffer_rect_coord, const int &controller_zoom)
{
    setPen(selected() ? mSelectedPen : mNormalPen);

    qmapcontrol::GeometryLineString::draw(painter, backbuffer_rect_coord, controller_zoom);

    if (controller_zoom >= minTextZoom) {
        QPointF p1 = projection::get().toPointWorldPx(m_points[0], controller_zoom).rawPoint();
        QPointF p2 = projection::get().toPointWorldPx(m_points[1], controller_zoom).rawPoint();

        QPointF mid (p1.x() + 0.3 * (p2.x() - p1.x()) , p1.y() + 0.3 * (p2.y() - p1.y()));

        painter.drawText(mid, QString::number(node->getAdiacencyWeight(edgeIdx)));
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
