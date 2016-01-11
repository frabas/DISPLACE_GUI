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

class DisplaceModel;

class StatsController : public QObject
{
    Q_OBJECT
public:
    explicit StatsController(QObject *parent = 0);

    void setPopulationPlot(QCustomPlot *plot);
    void setHarboursPlot(QCustomPlot *plot);
    void setNationsPlot(QCustomPlot *plot);

    void updateStats(DisplaceModel *model);

    enum PopulationStat { Aggregate, Mortality };
    void setPopulationStat(PopulationStat stat);
    PopulationStat getPopulationStat() const { return mSelectedPopStat; }

    enum NationsStat { Catches, Earnings, ExEarnings,TimeAtSea, Gav, Vpuf };
    void setNationsStat(NationsStat stat);
    NationsStat getNationsStat() const { return mSelectedNationsStat; }

    enum HarboursStat { H_Catches, H_Earnings, H_Gav, H_Vpuf };
    void setHarbourStat(HarboursStat stat);
    HarboursStat getHarboursStat() const { return mSelectedHarboursStat; }

    /* == */

    void initPlots();

    void setCurrentTimeStep(double t);

protected:
    void updatePopulationStats(DisplaceModel *model);
    double getPopStatValue (DisplaceModel *model, int tstep, int popid, int szid, PopulationStat stattype);

    void updateNationStats(DisplaceModel *model);
    void updateHarboursStats (DisplaceModel *model);

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

    DisplaceModel *mLastModel;

    static double timelineMin, timelineMax;
};

#endif // STATSCONTROLLER_H
