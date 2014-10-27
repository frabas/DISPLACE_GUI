#include "edgemapobject.h"

#include <mapobjectscontroller.h>
#include <displacemodel.h>

#include <QMessageBox>
#include <QPen>

QPen EdgeGraphics::mNormalPen(Qt::black);
QPen EdgeGraphics::mSelectedPen (QBrush(Qt::red), 3, Qt::SolidLine);

EdgeMapObject::EdgeMapObject(MapObjectsController *controller, int indx, NodeData *node)
    : QObject(),
      mController(controller),
      mEdgeIndex(indx),
      mNode(node)
{
    std::vector<qmapcontrol::PointWorldCoord> line;
    line.push_back(qmapcontrol::PointWorldCoord(mNode->get_x(), mNode->get_y()));

    NodeData *t = mNode->getModel()->getNodesList()[mNode->getAdiacencyByIdx(mEdgeIndex)];
    line.push_back(qmapcontrol::PointWorldCoord(t->get_x(), t->get_y()));

    mGeometry = std::shared_ptr<EdgeGraphics>(new EdgeGraphics(line));
    mGeometry->setFlags(qmapcontrol::Geometry::IsSelectable);
    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

void EdgeMapObject::onSelectionChanged()
{
    emit edgeSelectionHasChanged(this);
}

EdgeGraphics::EdgeGraphics(const std::vector<PointWorldCoord> &points)
    : qmapcontrol::GeometryLineString(points)
{
    setPen(mNormalPen);
}

void EdgeGraphics::draw(QPainter &painter, const qmapcontrol::RectWorldCoord &backbuffer_rect_coord, const int &controller_zoom)
{
    setPen(selected() ? mSelectedPen : mNormalPen);

    qmapcontrol::GeometryLineString::draw(painter, backbuffer_rect_coord, controller_zoom);
}
