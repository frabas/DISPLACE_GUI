#include "nodemapobject.h"

#include <displacemodel.h>

const Qt::GlobalColor NodeMapObject::NodeGraphics::colors[] = {
    Qt::red, Qt::blue, Qt::yellow, Qt::black, Qt::white, Qt::green
};

NodeMapObject::NodeMapObject(NodeData *node)
    : mNode(node),
     mGeometry()
{
    mGeometry = std::shared_ptr<NodeGraphics>(
                new NodeGraphics(mNode));
}

#define PIE_W 40.0
#define PIE_H 40.0

NodeMapObject::NodeGraphics::NodeGraphics(NodeData *node)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(node->mNode->get_x(), node->mNode->get_y()), QSizeF(PIE_W, PIE_H), 11, 7, 17),
      mNode(node)
{
    int l = (mNode->get_marine_landscape() * 0x1000000) / 1000;
    c = QColor(QRgb(l & 0x00ffffff));
}

void NodeMapObject::NodeGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    double tot = 0.0;
    const QList<int> & ilist =mNode->getModel()->getInterestingPops();
    foreach (int x, ilist) {
        tot += mNode->getPop(x);
    }

    if (tot > 1e-3) {
        double inc = 0.0;
        double v;
        for (int i = 0; i < ilist.size(); ++i) {
            v = mNode->getPop(ilist[i]) / tot;
            painter.setBrush(colors[i]);
            painter.drawPie(0, 0, PIE_W, PIE_H, inc, (v * 360.0));
            inc += v;
        }
    }

    painter.setBrush(c);
    painter.drawEllipse(0,0, PIE_W / 2, PIE_H / 2);
}
