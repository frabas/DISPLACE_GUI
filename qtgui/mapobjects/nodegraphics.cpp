#include "nodegraphics.h"

#include <displacemodel.h>
#include <mapobjects/nodegraphics.h>
#include <mapobjectscontroller.h>
#include <palettemanager.h>
#include <QMapControl/Projection.h>

#define PIE_W 40.0
#define PIE_H 40.0

NodeGraphics::NodeGraphics(NodeData *node, MapObjectsController *controller, int indx)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(node->mNode->get_x(), node->mNode->get_y()), QSizeF(PIE_W, PIE_H), 11, 7, 17),
      mNode(node),
      mController(controller),
      mModelIndex(indx),
      mGrid()
{
    int l = (mNode->get_marine_landscape() * 0x1000000) / 1000;
    c = QColor(QRgb(l & 0x00ffffff));

    double res_km = mNode->getModel()->scenario().getGraph_res();

    double psi = node->mNode->get_x() / 180.0 * M_PI;
    double phi = node->mNode->get_y() / 180.0 * M_PI;

    // one degree of latitude, in meters, is: 111132.954 - 559.822 cos (2 phi) + 1.175 cos (4 phi)
    // one degree of longitude, in meter, is: pi / 180  *  6,367,449 * cos psi.

    double dy = 1000.0 * res_km  / (111132.954 - 559.822 * std::cos(2.0 * phi) + 1.175 * std::cos(4.0 * phi));
    double dx = 1000.0 * res_km  / (M_PI / 180.0 * 6367449 * std::cos(psi) );

    PointWorldPx c2 = qmapcontrol::projection::get().toPointWorldPx(qmapcontrol::PointWorldCoord(psi - dx/2, phi - dy/2), baseZoom());
    PointWorldPx c1 = qmapcontrol::projection::get().toPointWorldPx(qmapcontrol::PointWorldCoord(psi + dx/2, phi + dy/2), baseZoom());

    mGrid.setHeight(c2.y() - c1.y());
    mGrid.setWidth(c2.x() - c1.x() );

    setSizePx(mGrid);
}

void NodeGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mSelected ? QBrush(Qt::red) : c);
    painter.drawEllipse(-PIE_W / 2, -PIE_W / 2, PIE_W, PIE_H);
    if (mSelected) {
        painter.setPen(QPen(QBrush(Qt::red), 3));
        painter.setBrush(Qt::transparent);
        painter.drawRect(-PIE_W, -PIE_W, 2*PIE_W, 2*PIE_H);
    }
}


void NodeWithPopStatsGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);
    double tot = 0.0f;

    QList<int> ilist = mNode->getModel()->getInterestingPops();

    for (int i = 0; i < ilist.size(); ++i) {
        tot += getValueForPop(ilist[i]);
    }

    int RADIUS = mGrid.width() / LastType * (LastType - mType);

    if (ilist.size() > 1) {
        if (tot > 1e-3) {
            double inc = 0.0;
            double v;
            for (int i = 0; i < ilist.size(); ++i) {
                v = getValueForPop(ilist[i]);
                v = v / tot * 360.0 * 16.0;
                painter.setBrush(mController->getPalette(mModelIndex, PopulationRole).color(ilist[i]));
                painter.drawPie(-RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS, inc, (v ));
                inc += v;
            }
        } else {
            /* Don't display "zero" values
            painter.setBrush(Qt::transparent);
            painter.setPen(c);
            painter.drawEllipse(-RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS);
            */
        }
    } else if (ilist.size() == 1) {
        double v = getValueForPop(ilist[0]);
        painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color(v));
        painter.drawRect(-RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS);
    } else {        // nothing to display.
        /*
        painter.setBrush(Qt::transparent);
        painter.setPen(c);
        painter.drawEllipse(-RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS);
        */
    }
}

double NodeWithPopStatsGraphics::getValueForPop(int pop) const
{
    switch (mType) {
    case Population:
        return mNode->getPop(pop);
    case Biomass:
        return mNode->getPopW(pop);
        break;
    case Impact:
        return mNode->getImpact(pop);
        break;
    case BenthosBiomass:
        return mNode->getBenthosBiomass(pop);
        break;
    default:
        Q_ASSERT(false);    /* Prevents this from happening */
    }

}


void NodeWithCumFTimeGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumftime()));
    painter.drawRect(-mGrid.width() / 2 , -mGrid.height() / 2, mGrid.width() , mGrid.height());
}


void NodeWithCumSweptAreaGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumsweptarea()));
    painter.drawRect(-mGrid.width() / 2 , -mGrid.height() / 2, mGrid.width() , mGrid.height());
}

void NodeWithCumCatchesGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumcatches()));
    painter.drawRect(-mGrid.width() / 2 , -mGrid.height() / 2, mGrid.width() , mGrid.height());
}

