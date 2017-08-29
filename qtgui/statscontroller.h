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


#ifndef STATSCONTROLLER_H
#define STATSCONTROLLER_H

#include <QObject>
#include <qcustomplot.h>
#include <palettemanager.h>

#include <plots/plottypes.h>
#include <graphinteractioncontroller.h>

class DisplaceModel;
class BenthosStatsPlot;
class FishfarmsStatsPlot;
class WindfarmsStatsPlot;
class ShipsStatsPlot;

class StatsController : public QObject
{
    Q_OBJECT
public:
    explicit StatsController(QObject *parent = 0);

    void setPopulationPlot(QCustomPlot *plot);
    void setHarboursPlot(QCustomPlot *plot);
    void setNationsPlot(QCustomPlot *plot);
    void setMetiersPlot(QCustomPlot *plot);
    void setBenthosPlot(QCustomPlot *plot, GraphInteractionController *controller);
    void setFishfarmsPlot(QCustomPlot *plot, GraphInteractionController *controller = nullptr);
    void setWindfarmsPlot(QCustomPlot *plot, GraphInteractionController *controller = nullptr);
    void setShipsPlot(QCustomPlot *plot, GraphInteractionController *controller = nullptr);

    void updateStats(DisplaceModel *model);

    enum StatType {
        Populations, Nations, Harbours, Metiers
    };

    enum PopulationStat { Aggregate, Mortality, SSB };
    void setPopulationStat(PopulationStat stat);
    PopulationStat getPopulationStat() const { return mSelectedPopStat; }

    enum NationsStat { Catches, Discards, Earnings, ExEarnings,TimeAtSea, Gav, Vpuf, SweptArea, RevenuePerSweptArea,  GVA, GVAPerRevenue,
                       LabourSurplus, GrossProfit,NetProfit, NetProfitMargin, GVAPerFTE, RoFTA, BER, CRBER, NetPresentValue, numTrips

  };
    void setNationsStat(NationsStat stat);
    NationsStat getNationsStat() const { return mSelectedNationsStat; }

    enum HarboursStat { H_Catches, H_Discards, H_Earnings, H_Gav, H_Vpuf, H_SweptArea, H_RevenuePerSweptArea,
                        H_GVA, H_GVAPerRevenue, H_LabourSurplus, H_GrossProfit, H_NetProfit, H_NetProfitMargin, H_GVAPerFTE, H_RoFTA, H_BER, H_CRBER, H_NetPresentValue, H_numTrips};

    void setHarbourStat(HarboursStat stat);
    HarboursStat getHarboursStat() const { return mSelectedHarboursStat; }

    enum MetiersStat { M_Catches, M_Discards, M_Revenues, M_Gav, M_Vpuf, M_SweptArea, M_RevenuesPerSweptArea,
                       M_GVA, M_GVAPerRevenue, M_LabourSurplus, M_GrossProfit, M_NetProfit, M_NetProfitMargin, M_GVAPerFTE, M_RoFTA, M_BER, M_CRBER, M_NetPresentValue, M_numTrips};
    void setMetiersStat(MetiersStat stat);
    MetiersStat getMetiersStat() const { return mSelectedMetiersStat; }

    void setBenthosStat(displace::plot::BenthosStat stat);
    displace::plot::BenthosStat getBenthosStat() const { return mSelectedBenthosStat; }

    void setFishfarmsStat(displace::plot::FishfarmsStat stat);
    displace::plot::FishfarmsStat getFishfarmsStat() const { return mSelectedFishfarmsStat; }

    void setWindfarmsStat(displace::plot::WindfarmsStat stat);
    displace::plot::WindfarmsStat getWindfarmsStat() const { return mSelectedWindfarmsStat; }

    void setShipsStat(displace::plot::ShipsStat stat);
    displace::plot::ShipsStat getShipsStat() const { return mSelectedShipsStat; }

    /* == */

    void initPlots();

    void setCurrentTimeStep(double t);

    void plotGraph (DisplaceModel *model, StatType st, int subtype, QCustomPlot *plot, QCPItemLine *line);

protected:
    void updatePopulationStats(DisplaceModel *model, PopulationStat popStat, QCustomPlot *plotPopulations, QCPItemLine *timeline);
    double getPopStatValue (DisplaceModel *model, int tstep, int popid, int szid, PopulationStat stattype);

    void updateNationStats(DisplaceModel *model, NationsStat mSelectedNationsStat, QCustomPlot *mPlotNations, QCPItemLine *timeLine);
    void updateHarboursStats (DisplaceModel *model, HarboursStat mSelectedNationsStat, QCustomPlot *mPlotNations, QCPItemLine *timeLine);
    void updateMetiersStats(DisplaceModel *model, MetiersStat metStat, QCustomPlot *plotMetiers, QCPItemLine *metTimeLine);
    void updateBenthosStats(DisplaceModel *model, displace::plot::BenthosStat stat);
    void updateFishfarmsStats(DisplaceModel *model, displace::plot::FishfarmsStat stat);
    void updateWindfarmsStats(DisplaceModel *model, displace::plot::WindfarmsStat stat);
    void updateShipsStats(DisplaceModel *model, displace::plot::ShipsStat stat);

private:
    Palette mPalette;

    /* Populations stuff */
    QCustomPlot *mPlotPopulations;
    PopulationStat mSelectedPopStat;
    QCPItemLine *mPopTimeLine;

    /* Harbour stuff */
    QCustomPlot *mPlotHarbours;
    HarboursStat mSelectedHarboursStat;
    QCPItemLine *mHarbTimeLine;

    /* Nations */
    QCustomPlot *mPlotNations;
    NationsStat mSelectedNationsStat;
    QCPItemLine *mNatTimeLine;

    /* Metiers */
    QCustomPlot *mPlotMetiers;
    MetiersStat mSelectedMetiersStat = M_Catches;
    QCPItemLine *mMetTimeLine;

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
