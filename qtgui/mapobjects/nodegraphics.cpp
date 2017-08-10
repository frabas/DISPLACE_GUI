// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "nodegraphics.h"

#include <displacemodel.h>
#include <mapobjects/nodegraphics.h>
#include <mapobjectscontroller.h>
#include <palettemanager.h>
#include <QMapControl/Projection.h>

QSettings NodeGraphics::settings;

NodeGraphics::NodeGraphics(NodeData *node, MapObjectsController *controller, int indx)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(node->mNode->get_x(), node->mNode->get_y()), QSizeF(piew(), pieh()), 11, 7, 17),
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
    painter.drawEllipse(-piew() / 2, -piew() / 2, piew(), pieh());
    if (mSelected) {
        painter.setPen(QPen(QBrush(Qt::red), 3));
        painter.setBrush(Qt::transparent);
        painter.drawRect(-piew(), -piew(), 2*piew(), 2*pieh());
    }
}

int NodeGraphics::piew()
{
    return settings.value("piew", 10).toInt();
}

int NodeGraphics::pieh()
{
    return settings.value("pieh", 10).toInt();
}

void NodeGraphics::setPieSize(int w, int h)
{
    settings.setValue("piew", w);
    settings.setValue("pieh", h);
}



void NodeWithPopStatsGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);
    double tot = 0.0f;

    QList<int> ilist = getInterestingList();

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
         break;
    case Biomass:
        return mNode->getPopW(pop);
        break;
    case Impact:
        return mNode->getImpact(pop);
        break;
    case CumcatchesPerPop:
        return mNode->getCumcatchesPerPop(pop);
        break;
    case BenthosBiomass:
        return mNode->getBenthosBiomass(pop);
        break;
    case BenthosNumber:
        return mNode->getBenthosNumber(pop);
        break;
    case BenthosMeanweight:
        return mNode->getBenthosMeanweight(pop);
        break;
    default:
        Q_ASSERT(false);    /* Prevents this from happening */
    }


    throw std::runtime_error("Unexpected switch in getValueForPop");
}

QList<int> NodeWithPopStatsGraphics::getInterestingList() const
{
    QList<int> ilist ;

    switch (mType) {
    case BenthosBiomass:
    case BenthosMeanweight:
    case BenthosNumber:
        ilist = mNode->getModel()->getFunctionalGroupsList()->list();
        if (ilist.size() == 0) {
            for (int i = 0; i < mNode->getModel()->getBenthosPopulationsCount(); ++i)
                ilist.push_back(i);
        }
        break;
    default:
        ilist = mNode->getModel()->getInterestingPops();
        if (ilist.size() == 0) {
            for (int i = 0; i < mNode->getModel()->getPopulationsCount(); ++i)
                ilist.push_back(i);
        }
        break;
    }
    return ilist;
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

void NodeWithCumSubsurfaceSweptAreaGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumsubsurfacesweptarea()));
    painter.drawRect(-mGrid.width() / 2 , -mGrid.height() / 2, mGrid.width() , mGrid.height());
}

void NodeWithCumCatchesGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumcatches()));
    painter.drawRect(-mGrid.width() / 2 , -mGrid.height() / 2, mGrid.width() , mGrid.height());
}

void NodeWithCumCatchesWithThresholdGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumcatches_with_threshold()));
    painter.drawRect(-mGrid.width() / 2 , -mGrid.height() / 2, mGrid.width() , mGrid.height());
}


void NodeWithTariffsGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    vector<double>  tariffs =mNode->get_tariffs();
    painter.setBrush(mController->getPalette(mModelIndex,TariffsRole).color((float)tariffs.at(0) ));
    painter.drawRect(-mGrid.width() / 2 , -mGrid.height() / 2, mGrid.width() , mGrid.height());
}
