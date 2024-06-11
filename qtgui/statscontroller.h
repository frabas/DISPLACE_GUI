// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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


#ifndef STATSCONTROLLER_H
#define STATSCONTROLLER_H

#include <QObject>
#include <qcustomplot.h>
#include <palettemanager.h>

#include "plottypes.h"
#include <graphinteractioncontroller.h>
#include <StatsPlot.h>

class DisplaceModel;
class BenthosStatsPlot;
class FishfarmsStatsPlot;
class WindfarmsStatsPlot;
class ShipsStatsPlot;
class NationsStatsPlot;
class VesselsStatsPlot;
class PopulationsStatPlot;
class HarboursStatPlot;
class MetiersStatsPlot;
class PlotWidget;

class StatsController : public QObject
{
    Q_OBJECT
public:
    explicit StatsController(QObject *parent = 0);

    void setPopulationPlot(PlotWidget *plot, GraphInteractionController *controller);
    void setHarboursPlot(PlotWidget *plot);
    void setMetiersPlot(PlotWidget *plot);
    void setBenthosPlot(PlotWidget *plot, GraphInteractionController *controller);
    void setFishfarmsPlot(PlotWidget *plot, GraphInteractionController *controller = nullptr);
    void setWindfarmsPlot(PlotWidget *plot, GraphInteractionController *controller = nullptr);
    void setShipsPlot(PlotWidget *plot, GraphInteractionController *controller = nullptr);
    void setNationsStatsPlot(PlotWidget *plot, GraphInteractionController *controller = nullptr);
    void setVesselsStatsPlot(PlotWidget *plot, GraphInteractionController *controller = nullptr);

    void updateStats(DisplaceModel *model);

    enum StatType {
        Populations, Nations, Vessels, Harbours, Metiers
    };

    void setPopulationStat(displace::plot::PopulationStat stat);
    displace::plot::PopulationStat getPopulationStat() const { return mSelectedPopStat; }

    void setNationsStat(displace::plot::NationsStat stat);
    displace::plot::NationsStat getNationsStat() const { return mSelectedNationsStat; }

    void setVesselsStat(displace::plot::VesselsStat stat);
    displace::plot::VesselsStat getVesselsStat() const { return mSelectedVesselsStat; }

    void setHarbourStat(displace::plot::HarboursStat stat);
    displace::plot::HarboursStat getHarboursStat() const { return mSelectedHarboursStat; }

    void setMetiersStat(displace::plot::MetiersStat stat);
    displace::plot::MetiersStat getMetiersStat() const { return mSelectedMetiersStat; }

    void setBenthosStat(displace::plot::BenthosStat stat);
    displace::plot::BenthosStat getBenthosStat() const { return mSelectedBenthosStat; }

    void setFishfarmsStat(displace::plot::FishfarmsStat stat);
    displace::plot::FishfarmsStat getFishfarmsStat() const { return mSelectedFishfarmsStat; }

    void setWindfarmsStat(displace::plot::WindfarmsStat stat);
    displace::plot::WindfarmsStat getWindfarmsStat() const { return mSelectedWindfarmsStat; }

    void setShipsStat(displace::plot::ShipsStat stat);
    displace::plot::ShipsStat getShipsStat() const { return mSelectedShipsStat; }

    /* == */

    void setCurrentTimeStep(double t);

    void plotGraph (DisplaceModel *model, StatType st, int subtype, QCustomPlot *plot = nullptr);

protected:
    void updatePopulationStats(DisplaceModel *model, displace::plot::PopulationStat popStat, QCustomPlot *plot);
    void updateNationStats(DisplaceModel *model, displace::plot::NationsStat mSelectedNationsStat,
                               QCustomPlot *plot);
    void updateVesselStats(DisplaceModel *model, displace::plot::VesselsStat mSelectedVesselsStat,
                               QCustomPlot *plot);
    void updateHarboursStats (DisplaceModel *model, displace::plot::HarboursStat stat, QCustomPlot *plot);
    void updateMetiersStats(DisplaceModel *model, displace::plot::MetiersStat stat, QCustomPlot *plot);
    void updateBenthosStats(DisplaceModel *model, displace::plot::BenthosStat stat);
    void updateFishfarmsStats(DisplaceModel *model, displace::plot::FishfarmsStat stat);
    void updateWindfarmsStats(DisplaceModel *model, displace::plot::WindfarmsStat stat);
    void updateShipsStats(DisplaceModel *model, displace::plot::ShipsStat stat);

private:
    Palette mPalette;

    /* Populations stuff */
    displace::plot::PopulationStat mSelectedPopStat;
    PopulationsStatPlot *mPopPlot;

    /* Harbour stuff */
    displace::plot::HarboursStat mSelectedHarboursStat;
    HarboursStatPlot *mPlotHarbours;

    /* Nations */
    QCustomPlot *mNationsPlot;
    displace::plot::NationsStat mSelectedNationsStat;
    QCPItemLine *mNatTimeLine;
    NationsStatsPlot *mNationsStatsPlotController = nullptr;

    /* Vessels */
    QCustomPlot *mVesselsPlot;
    displace::plot::VesselsStat mSelectedVesselsStat;
    QCPItemLine *mVesTimeLine;
    VesselsStatsPlot *mVesselsStatsPlotController = nullptr;

    /* Metiers */
    displace::plot::MetiersStat mSelectedMetiersStat = displace::plot::MetiersStat::M_Catches;
    MetiersStatsPlot *mPlotMetiers;

    /* Benthos Functional Groups */
    QCustomPlot *mBenthosFuncGroupsPlot = nullptr;
    displace::plot::BenthosStat mSelectedBenthosStat = displace::plot::BenthosStat::B_TotBiomass;
    QCPItemLine *mBenthosTimeLine = nullptr;
    BenthosStatsPlot *mBenthosPlotController = nullptr;

    /* Fishfarm farmtype Groups */
    QCustomPlot *mfarmTypeGroupsPlot = nullptr;
    displace::plot::FishfarmsStat mSelectedFishfarmsStat = displace::plot::FishfarmsStat::FF_FishMeanWeight;
    QCPItemLine *mFishfarmsTimeLine = nullptr;
    FishfarmsStatsPlot *mFishfarmsPlotController = nullptr;

    /* Windfarm farmtype Groups */
    QCustomPlot *mWindfarmTypeGroupsPlot = nullptr;
    displace::plot::WindfarmsStat mSelectedWindfarmsStat = displace::plot::WindfarmsStat::WF_kWh;
    QCPItemLine *mWindfarmsTimeLine = nullptr;
    WindfarmsStatsPlot *mWindfarmsPlotController = nullptr;

    /* Ship farmtype Groups */
    QCustomPlot *mShipTypeGroupsPlot = nullptr;
    displace::plot::ShipsStat mSelectedShipsStat = displace::plot::ShipsStat::SH_NOxEmission;
    QCPItemLine *mShipsTimeLine = nullptr;
    ShipsStatsPlot *mShipsPlotController = nullptr;


    DisplaceModel *mLastModel;

    static double timelineMin, timelineMax;
};

#endif // STATSCONTROLLER_H
