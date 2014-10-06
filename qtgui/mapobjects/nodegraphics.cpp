#include "nodegraphics.h"

#include <displacemodel.h>
#include <mapobjects/nodegraphics.h>
#include <mapobjectscontroller.h>
#include <palettemanager.h>

#define PIE_W 40.0
#define PIE_H 40.0

const Qt::GlobalColor NodeGraphics::colors[] = {
    Qt::red, Qt::blue, Qt::yellow, Qt::black, Qt::white, Qt::green
};

NodeGraphics::NodeGraphics(NodeData *node, MapObjectsController *controller)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(node->mNode->get_x(), node->mNode->get_y()), QSizeF(PIE_W, PIE_H), 11, 7, 17),
      mNode(node),
      mController(controller)
{
    int l = (mNode->get_marine_landscape() * 0x1000000) / 1000;
    c = QColor(QRgb(l & 0x00ffffff));
}

void NodeGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(c);
    painter.drawEllipse(0,0, PIE_W / 2, PIE_H / 2);
}


void NodeWithPopStatsGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    double tot = mNode->getPopTot();
    const QList<int> & ilist = mNode->getModel()->getInterestingPops();

    if (tot > 1e-3) {
        double inc = 0.0;
        double v;
        for (int i = 0; i < ilist.size(); ++i) {
            v = mNode->getPop(ilist[i]) / tot;
            painter.setBrush(colors[i]);
            painter.drawPie(0, 0, PIE_W, PIE_H, inc, (v * 360.0 * 16.0));
            inc += v;
        }
    }
}


void NodeWithCumFTimeGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(0,0).color(mNode->get_cumftime()));

    int d = mNode->get_cumftime() * PIE_W / 10;

    painter.drawRect(-d/2, -d/2, d, d);
}
