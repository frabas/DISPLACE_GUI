// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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
#include <mapsdataprovider.h>
#include <palettemanager.h>
#include <QMapControl/Projection.h>

QSettings *NodeGraphics::settings = nullptr;


NodeGraphics::NodeGraphics(NodeData *node, MapObjectsController *controller, int indx)
    : qmapcontrol::GeometryPointShapeScaled(qmapcontrol::PointWorldCoord(node->mNode->get_x(), node->mNode->get_y()), QSizeF(piew(), pieh()), 11, 7, 17),
      mNode(node),
      mController(controller),
      mModelIndex(indx)
{
    int l = (mNode->get_marine_landscape() * 0x1000000) / 1000;
    c = QColor(QRgb(l & 0x00ffffff));
}

std::shared_ptr<types::EnvironmentData> NodeGraphics::getEnvtData()
{
    auto tstep = mController->getModel(mModelIndex).getCurrentStep();
    auto &m = mController->getModel(mModelIndex).getMapDataProvider();

    return m.getEnvironmentData(mNode->get_idx_node(), types::tstep_t(tstep));
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
    if (!settings) settings = new QSettings();
    return settings->value("piew", 100).toInt();
}

int NodeGraphics::pieh()
{
    if (!settings) settings = new QSettings();
    return settings->value("pieh", 100).toInt();
}

void NodeGraphics::setPieSize(int w, int h)
{
    if (!settings) settings = new QSettings();
    settings->setValue("piew", w);
    settings->setValue("pieh", h);
}

void NodeWithPopStatsGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);
    double tot = 0.0f;

    QList<int> ilist = getInterestingList();

    for (int i = 0; i < ilist.size(); ++i) {
        auto v = getValueForPop(ilist[i]);
        if (v.is_initialized()) {
            tot += v.value();
        }
    }

    //int RADIUS = piew() / LastType * (LastType - mType); // downsizing the nodegraphics size from the last layer
    int RADIUS = piew();

    if (ilist.size() > 1) {
        if (tot > 1e-3) {
            double inc = 0.0;
            boost::optional<double> v;
            for (int i = 0; i < ilist.size(); ++i) {
                v = getValueForPop(ilist[i]);

                if (v.is_initialized()) {
                    double vr = v.value() / tot * 360.0 * 16.0;
                    painter.setBrush(mController->getPalette(mModelIndex, PopulationRole).color(ilist[i]));
                    painter.drawPie(-RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS, inc, (vr));
                    inc += vr;
                }
            }
        } else {
            /* Don't display "zero" values
            painter.setBrush(Qt::transparent);
            painter.setPen(c);
            painter.drawEllipse(-RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS);
            */
        }
    } else if (ilist.size() == 1) {
        boost::optional<double> v = getValueForPop(ilist[0]);
        if (v.is_initialized()) {
            switch (mType) {
            case BenthosBiomass:
            case BenthosMeanweight:
            case BenthosNumber:
                painter.setBrush(mController->getPalette(mModelIndex, Value0to1BlueRole).color(v.value()));
              break;
            default:
                painter.setBrush(mController->getPalette(mModelIndex, ValueRole).color(v.value()));
            }
            painter.drawRect(-RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS);
        }
    } else {        // nothing to display.
        /*
        painter.setBrush(Qt::transparent);
        painter.setPen(c);
        painter.drawEllipse(-RADIUS / 2, -RADIUS / 2, RADIUS, RADIUS);
        */
    }
}

boost::optional<double> NodeWithPopStatsGraphics::getValueForPop(int pop) const
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
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}


void NodeWithCumSweptAreaGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumsweptarea()));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithCumSubsurfaceSweptAreaGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumsubsurfacesweptarea()));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithCumCatchesGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumcatches()));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithCumCatchesWithThresholdGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumcatches_with_threshold()));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithCumDiscardsGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,ValueRole).color((float)mNode->get_cumdiscards()));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithCumDiscardsRatioGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,Value0to1Role).color((double)mNode->get_cumdiscardsratio()));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithNbChokedGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    painter.setBrush(mController->getPalette(mModelIndex,Value0to1Role).color((double)mNode->get_nbchoked()));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}


void NodeWithTariffs0Graphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    vector<double>  tariffs =mNode->get_tariffs();
    painter.setBrush(mController->getPalette(mModelIndex,TariffsRole).color((float)tariffs.at(0) ));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}


void NodeWithTariffs1Graphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    vector<double>  tariffs =mNode->get_tariffs();
    painter.setBrush(mController->getPalette(mModelIndex,TariffsRole).color((float)tariffs.at(1) ));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithTariffs2Graphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    vector<double>  tariffs =mNode->get_tariffs();
    painter.setBrush(mController->getPalette(mModelIndex,TariffsRole).color((float)tariffs.at(2) ));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithSalinityGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto salinity = (r != nullptr ? r->salinity : 0);
    painter.setBrush(mController->getPalette(mModelIndex,SalinityRole).color(salinity));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithSSTGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto sst = (r != nullptr ? r->sst : 0);
    painter.setBrush(mController->getPalette(mModelIndex,SSTRole).color(sst));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithWindGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto wind = (r != nullptr ? r->wind : 0);
    painter.setBrush(mController->getPalette(mModelIndex,WindRole).color(wind));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithNitrogenGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto ni = (r != nullptr ? r->nitrogen : 0);
    painter.setBrush(mController->getPalette(mModelIndex,NitrogenRole).color(ni));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithPhosphorusGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto p = (r != nullptr ? r->phosphorus : 0);
    painter.setBrush(mController->getPalette(mModelIndex,PhosphorusRole).color(p));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithOxygenGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto o2 = (r != nullptr ? r->oxygen : 0);
    painter.setBrush(mController->getPalette(mModelIndex,OxygenRole).color(o2));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithDissolvedCarbonGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto ca = (r != nullptr ? r->dissolvedcarbon : 0);
    painter.setBrush(mController->getPalette(mModelIndex,DissolvedCarbonRole).color(ca));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithBathymetryGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto ca = (r != nullptr ? r->bathymetry : 0);
    //painter.setBrush(mController->getPalette(mModelIndex,BathyRole).color((float)mNode->get_bathymetry()));
    painter.setBrush(mController->getPalette(mModelIndex,BathyRole).color(ca));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithShippingdensityGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto ca = (r != nullptr ? r->shippingdensity : 0);
    //painter.setBrush(mController->getPalette(mModelIndex,ShippingdensityRole).color((float)mNode->get_shippingdensity()));
    painter.setBrush(mController->getPalette(mModelIndex,ShippingdensityRole).color(ca));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithSiltfractionGraphics::drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto ca = (r != nullptr ? r->siltfraction : 0);
    //painter.setBrush(mController->getPalette(mModelIndex,SiltfractionRole).color((float)mNode->get_siltfraction()));
    painter.setBrush(mController->getPalette(mModelIndex,SiltfractionRole).color(ca));
    painter.drawRect(-piew() / 2 , -pieh() / 2, piew() , pieh());
}

void NodeWithIcesrectanglecodeGraphics::drawShape(QPainter& painter, const qmapcontrol::RectWorldPx& rect)
{
    Q_UNUSED(rect);

    auto r = getEnvtData();
    auto ca = (r != nullptr ? r->icesrectanglecode : 0);
    //painter.setBrush(mController->getPalette(mModelIndex,SiltfractionRole).color((float)mNode->get_siltfraction()));
    painter.setBrush(mController->getPalette(mModelIndex, SiltfractionRole).color(ca));
    painter.drawRect(-piew() / 2, -pieh() / 2, piew(), pieh());
}


