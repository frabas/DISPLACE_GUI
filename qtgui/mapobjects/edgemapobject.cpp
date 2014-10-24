#include "edgemapobject.h"

#include <mapobjectscontroller.h>
#include <displacemodel.h>

#include <QMessageBox>
#include <QPen>

QPen EdgeMapObject::mNormalPen(Qt::black);
QPen EdgeMapObject::mSelectedPen (QBrush(Qt::red), 3, Qt::SolidLine);

EdgeMapObject::EdgeMapObject(MapObjectsController *controller, int indx, NodeData *node)
    : QObject(),
      mController(controller),
      mEdgeIndex(indx),
      mSelected(false),
      mNode(node)
{
    std::vector<qmapcontrol::PointWorldCoord> line;
    line.push_back(qmapcontrol::PointWorldCoord(mNode->get_x(), mNode->get_y()));

    NodeData *t = mNode->getModel()->getNodesList()[mNode->getAdiacencyByIdx(mEdgeIndex)];
    line.push_back(qmapcontrol::PointWorldCoord(t->get_x(), t->get_y()));

    mGeometry = std::shared_ptr<qmapcontrol::GeometryLineString>(new qmapcontrol::GeometryLineString(line));
    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));

    mGeometry->setPen(mNormalPen);
}

bool EdgeMapObject::clicked()
{
    mSelected = !mSelected;
    mGeometry->setPen(mSelected ? mSelectedPen : mNormalPen);

//    mGeometry->requestRedraw();
    emit selected( /*this,*/ mSelected);

    qDebug() << "Selected: " << mNode->get_idx_node() << mEdgeIndex;
    return true;
}
