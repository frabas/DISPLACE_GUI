// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "nodemapobject.h"

#include <displacemodel.h>
#include <inputfileparser.h>

#include <mapobjectscontroller.h>
#include <mapobjects/nodegraphics.h>
#include <mapobjects/nodedetailswidget.h>

#include <QMapControl/QMapControl.h>

#include <stdexcept>

NodeMapObject::NodeMapObject(MapObjectsController *controller, int indx, Role role, std::shared_ptr<NodeData> node)
    : mController(controller),
      mNode(node),
      mRole(role),
      mGeometry(),
      mWidget(0)
{
    switch (role) {

    case GraphNodeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeGraphics(mNode.get(), mController, indx));
        mGeometry->setFlags(qmapcontrol::Geometry::IsSelectable);
        break;

    case GraphNodeWithPopStatsRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Population, mNode.get(), mController, indx));
        break;

    case GraphNodeWithBiomass:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Biomass, mNode.get(), mController, indx));
        break;

    case GraphNodeWithPopImpact:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::Impact, mNode.get(), mController, indx));
        break;

    case GraphNodeWithPopCumcatches:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::CumcatchesPerPop, mNode.get(), mController, indx));
        break;

    case GraphNodeWithBenthosBiomass:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::BenthosBiomass, mNode.get(), mController, indx));
        break;

    case GraphNodeWithBenthosNumber:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::BenthosNumber, mNode.get(), mController, indx));
        break;

    case GraphNodeWithBenthosMeanweight:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPopStatsGraphics(NodeWithPopStatsGraphics::BenthosMeanweight, mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumFTimeRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumFTimeGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumSweptAreaRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumSweptAreaGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumSubsurfaceSweptAreaRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumSubsurfaceSweptAreaGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumCatchesRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumCatchesGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumCatchesWithThresholdRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumCatchesWithThresholdGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumDiscardsRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumDiscardsGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithCumDiscardsRatioRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithCumDiscardsRatioGraphics(mNode.get(), mController, indx));
        break;
    case GraphNodeWithNbChokedRole:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithNbChokedGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithTariffs0:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithTariffs0Graphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithTariffs1:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithTariffs1Graphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithTariffs2:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithTariffs2Graphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithSalinity:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithSalinityGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithSST:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithSSTGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithWind:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithWindGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithNitrogen:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithNitrogenGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithPhosphorus:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithPhosphorusGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithOxygen:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithOxygenGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithDissolvedCarbon:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithDissolvedCarbonGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithBathymetry:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithBathymetryGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithShippingdensity:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithShippingdensityGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithSiltfraction:
        mGeometry = std::shared_ptr<NodeGraphics>(
                    new NodeWithSiltfractionGraphics(mNode.get(), mController, indx));
        break;

    case GraphNodeWithIcesrectanglecode:
        mGeometry = std::shared_ptr<NodeGraphics>(
            new NodeWithIcesrectanglecodeGraphics(mNode.get(), mController, indx));
        break;

    default:
        Q_ASSERT(false);        /* Disallow creating "unknown" nodes */
        break;
    }

    mGeometry->setAncillaryData(new MapObjectsController::WidgetAncillaryData(this));
}

bool NodeMapObject::showProperties()
{
    mWidget = new NodeDetailsWidget(mController->mapWidget());
    connect (mWidget, SIGNAL(destroyed()), this, SLOT(widgetClosed()));

    updateProperties();
    mController->showDetailsWidget(mGeometry->coord(),mWidget);
    return true;
}

void NodeMapObject::updateProperties()
{
    if (!mWidget)
        return;

    QString text = QString("<b>Name</b>: %1 <b>id</b>: %4<br/>"
                           "<b>Coords: </b>%2 %3<br/>"
                           "<b>Landscape: </b>%5<br/>"
                           "<b>Init Benthos Biomass: </b>%6<br/>"
                           "<b>Init Benthos Number: </b>%7<br/>"
                           "<b>Code Area: </b>%8<br/>")
            .arg(QString::fromStdString(mNode->get_name()))
            .arg(mNode->get_y())
            .arg(mNode->get_x())
            .arg(mNode->get_idx_node().toIndex())
            .arg(mNode->get_marine_landscape())
            .arg(mNode->get_init_benthos_biomass())
            .arg(mNode->get_init_benthos_number())
            .arg(mNode->get_code_area());





    std::ostringstream ss;
    vector <double> tariffs=mNode->get_tariffs();
    text += QString("<br/><b>Tariffs</b><br/>");
    for (size_t gr=0; gr<tariffs.size(); ++gr) {
       ss << tariffs.at(gr) << ",";
    }
    text += QString::fromStdString(ss.str());

    switch (mRole) {
    default:
    case GraphNodeRole:
        if (mNode->getModel()->isShortestPathFolderLinked()) {
            InputFileParser parser;

            if (parser.isShortestPathPresent(mNode->getModel()->linkedShortestPathFolder(), mNode->get_idx_node())) {
                connect (mWidget, SIGNAL(toolButtonClicked()), this, SLOT(toolButtonClicked()));
                mWidget->showTool(true);
            }
        }
        text += QString("<br/><b>Adjacencies</b><br/>");
        for (int i = 0; i < mNode->getAdiacencyCount(); ++i) {
            std::shared_ptr<NodeData::Edge> e = mNode->getAdiacencyByIdx(i);
            std::shared_ptr<NodeData> t = e->target.lock();
            if (t.get() != nullptr)
                text += QString("Node <b>%1</b> weight <b>%2</b><br/>")
                        .arg(t->get_idx_node().toIndex())
                        .arg(e->weight);
        }
        break;

    case GraphNodeWithPopStatsRole:
        text += updateStatText("Population");
        break;
    case GraphNodeWithBiomass:
        text += updateStatText("Biomass");
        break;
    case GraphNodeWithPopImpact:
        text += updateStatText("Impact");
        break;

    case GraphNodeWithPopCumcatches:
        text += updateStatText("Accum.catches(kg. land.+disc.):");
        break;

    case GraphNodeWithBenthosBiomass:
        text += updateStatText("Benthos Biomass");
        break;

    case GraphNodeWithBenthosNumber:
        text += updateStatText("Benthos Number");
        break;

    case GraphNodeWithBenthosMeanweight:
        text += updateStatText("Benthos Mean Weight");
        break;

    case GraphNodeWithTariffs0:
        text += QString("<br/><b>Tariffs (type All):</b> %1<br/>")
                .arg(tariffs.at(0));
        break;

    case GraphNodeWithTariffs1:
        text += QString("<br/><b>Tariffs (type Pop):</b> %1<br/>")
                .arg(tariffs.at(1));
        break;

    case GraphNodeWithTariffs2:
        text += QString("<br/><b>Tariffs (type Benthos):</b> %1<br/>")
                .arg(tariffs.at(2));
        break;

    case GraphNodeWithSalinity:
        text += QString("<br/><b>Salinity:</b> %1<br/>")
                .arg(mNode->get_salinity());
        break;

    case GraphNodeWithSST:
        text += QString("<br/><b>SST:</b> %1<br/>")
                .arg(mNode->get_sst());
        break;

    case GraphNodeWithWind:
        text += QString("<br/><b>Wind:</b> %1<br/>")
                .arg(mNode->get_wind());
        break;

    case GraphNodeWithNitrogen:
        text += QString("<br/><b>Nitrogen:</b> %1<br/>")
                .arg(mNode->get_Nitrogen());
        break;

    case GraphNodeWithPhosphorus:
        text += QString("<br/><b>Phosphorus:</b> %1<br/>")
                .arg(mNode->get_Phosphorus());
        break;

    case GraphNodeWithOxygen:
        text += QString("<br/><b>Oxygen:</b> %1<br/>")
                .arg(mNode->get_Oxygen());
        break;

    case GraphNodeWithDissolvedCarbon:
        text += QString("<br/><b>Dissolved Carbon:</b> %1<br/>")
                .arg(mNode->get_DissolvedCarbon());
        break;

    case GraphNodeWithBathymetry:
        text += QString("<br/><b>Bathymetry:</b> %1<br/>")
                .arg(mNode->get_bathymetry());
        break;

    case GraphNodeWithShippingdensity:
        text += QString("<br/><b>Shipping density:</b> %1<br/>")
                .arg(mNode->get_shippingdensity());
        break;

    case GraphNodeWithSiltfraction:
        text += QString("<br/><b>Silt fraction:</b> %1<br/>")
                .arg(mNode->get_siltfraction());
        break;

    case GraphNodeWithIcesrectanglecode:
        text += QString("<br/><b>ICES Rectangle coding:</b> %1<br/>")
            .arg(mNode->get_icesrectanglecode());
        break;

    case GraphNodeWithCumFTimeRole:
        text += QString("<br/><b>Fishing Effort (hours):</b> %1<br/>")
                .arg(mNode->get_cumftime());
        break;

    case GraphNodeWithCumSweptAreaRole:
        text += QString("<br/><b>Swept Area (km2):</b> %1<br/>")
                .arg(mNode->get_cumsweptarea());
        break;

    case GraphNodeWithCumSubsurfaceSweptAreaRole:
        text += QString("<br/><b>Subsurface Swept Area (km2):</b> %1<br/>")
                .arg(mNode->get_cumsubsurfacesweptarea());
        break;

    case GraphNodeWithCumCatchesRole:
        text += QString("<br/><b>Accum.catches(kg land.+disc.):</b> %1<br/>")
                .arg(mNode->get_cumcatches());
        break;

    case GraphNodeWithCumCatchesWithThresholdRole:
        text += QString("<br/><b>Accum.catches(kg land.+disc.):</b> %1<br/>")
                .arg(mNode->get_cumcatches());
        break;

    case GraphNodeWithCumDiscardsRole:
        text += QString("<br/><b>Accum. discards:</b> %1<br/>")
                .arg(mNode->get_cumdiscards());
        break;

    case GraphNodeWithCumDiscardsRatioRole:
        text += QString("<br/><b>Discards ratio:</b> %1<br/>")
                .arg(mNode->get_cumdiscardsratio());
        break;

    case GraphNodeWithNbChokedRole:
        text += QString("<br/><b>Nb choked stks:</b> %1<br/>")
                .arg(mNode->get_nbchoked());
        break;
    }

    mWidget->setText(text);
}

QString NodeMapObject::updateStatText(QString prefix)
{
    QString text = "<br/>";

    QList<int> ilist = getInterestingList();
    double tot = 0.0;

    for(int i : ilist) {
        boost::optional<double> val;

        switch (mRole) {
        case GraphNodeWithPopStatsRole:
            val = mNode->getPop(i);
            break;
        case GraphNodeWithBiomass:
            val = mNode->getPopW(i);
            break;
        case GraphNodeWithPopImpact:
            val = mNode->getImpact(i);
            break;
        case GraphNodeWithPopCumcatches:
            val = mNode->getCumcatchesPerPop(i);
            break;
        case GraphNodeWithBenthosBiomass:
            val = mNode->getBenthosBiomass(i);
            break;
        case GraphNodeWithBenthosNumber:
            val = mNode->getBenthosNumber(i);
            break;
        case GraphNodeWithBenthosMeanweight:
            val = mNode->getBenthosMeanweight(i);
            break;
        default:
            throw std::runtime_error("Unhandled case in updateStatText");
        }

        text += QString("<b>%1 %2:</b> %3<br/>")
                .arg(prefix)
                .arg(i)
                .arg(val.is_initialized() ? QString::number(val.value()) : "-");
        if (val.is_initialized()) {
            tot += val.value();
        }
    }
    text += QString("<b>Total:</b> %1<br/>")
            .arg(tot);

    return text;
}

QList<int> NodeMapObject::getInterestingList() const
{
    QList<int> ilist ;

    switch (mRole) {
    case GraphNodeWithBenthosBiomass:
    case GraphNodeWithBenthosNumber:
    case GraphNodeWithBenthosMeanweight:
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


void NodeMapObject::toolButtonClicked()
{
    QMessageBox::warning(mWidget, tr("Unimplemented"), tr("This function is not implemented yet."));
}

void NodeMapObject::widgetClosed()
{
    mWidget = 0; // detach
}
