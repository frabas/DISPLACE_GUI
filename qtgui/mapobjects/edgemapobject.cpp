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
    : qmapcontrol::GeometryLineString(points, minZoom, maxZoom),
      node(nd), edgeIdx(edge)
{
    setPen(mNormalPen);
}

void EdgeGraphics::draw(QPainter &painter, const qmapcontrol::RectWorldCoord &backbuffer_rect_coord, const int &controller_zoom)
{
    setPen(selected() ? mSelectedPen : mNormalPen);

    qmapcontrol::GeometryLineString::draw(painter, backbuffer_rect_coord, controller_zoom);

    if (selected()) {
        QPointF p1 = projection::get().toPointWorldPx(m_points[0], controller_zoom).rawPoint();
        QPointF p2 = projection::get().toPointWorldPx(m_points[1], controller_zoom).rawPoint();

        double dx = p2.x() - p1.x();
        double dy = p2.y() - p1.y();
        double angle = ::acos(dx
                              / std::sqrt(dx*dx + dy*dy));
        double arrowSize = 20.0;
        if (dy >= 0)
            angle = (M_PI * 2) - angle;

        QPointF arrowP1 = p1 + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                                cos(angle + M_PI / 3) * arrowSize);
        QPointF arrowP2 = p1 + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                                cos(angle + M_PI - M_PI / 3) * arrowSize);

        QPolygonF arrowHead;
        arrowHead.clear();
        arrowHead << p1 << arrowP1 << arrowP2;
//        painter.drawLine(line());
        painter.drawPolygon(arrowHead);
        /*
        if (isSelected()) {
            painter->setPen(QPen(myColor, 1, Qt::DashLine));
            QLineF myLine = line();
            myLine.translate(0, 4.0);
            painter->drawLine(myLine);
            myLine.translate(0,-8.0);
            painter->drawLine(myLine);
        }*/
    }

    if (controller_zoom >= minTextZoom) {
        QPointF p1 = projection::get().toPointWorldPx(m_points[0], controller_zoom).rawPoint();
        QPointF p2 = projection::get().toPointWorldPx(m_points[1], controller_zoom).rawPoint();

        QPointF mid (p1.x() + 0.3 * (p2.x() - p1.x()) , p1.y() + 0.3 * (p2.y() - p1.y()));

        painter.drawText(mid, QString::number(node->getAdiacencyWeight(edgeIdx)));
    }
}
