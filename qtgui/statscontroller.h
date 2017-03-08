// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef STATSCONTROLLER_H
#define STATSCONTROLLER_H

#include <QObject>
#include <qcustomplot.h>
#include <palettemanager.h>

#include <plots/plottypes.h>

class DisplaceModel;
class BenthosStatsPlot;

class StatsController : public QObject
{
    Q_OBJECT
public:
    explicit StatsController(QObject *parent = 0);

    void setPopulationPlot(QCustomPlot *plot);
    void setHarboursPlot(QCustomPlot *plot);
    void setNationsPlot(QCustomPlot *plot);
    void setMetiersPlot(QCustomPlot *plot);
    void setBenthosPlot(QCustomPlot *plot);

    void updateStats(DisplaceModel *model);

    enum StatType {
        Populations, Nations, Harbours, Metiers
    };

    enum PopulationStat { Aggregate, Mortality, SSB };
    void setPopulationStat(PopulationStat stat);
    PopulationStat getPopulationStat() const { return mSelectedPopStat; }

    enum NationsStat { Catches, Earnings, ExEarnings,TimeAtSea, Gav, Vpuf };
    void setNationsStat(NationsStat stat);
    NationsStat getNationsStat() const { return mSelectedNationsStat; }

    enum HarboursStat { H_Catches, H_Earnings, H_Gav, H_Vpuf };
    void setHarbourStat(HarboursStat stat);
    HarboursStat getHarboursStat() const { return mSelectedHarboursStat; }

    enum MetiersStat { M_Catches, M_Revenues, M_Gav };
    void setMetiersStat(MetiersStat stat);
    MetiersStat getMetiersStat() const { return mSelectedMetiersStat; }

    void setBenthosStat(displace::plot::BenthosStat stat);
    displace::plot::BenthosStat getBenthosStat() const { return mSelectedBenthosStat; }

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

    DisplaceModel *mLastModel;

    static double timelineMin, timelineMax;
};

#endif // STATSCONTROLLER_H
