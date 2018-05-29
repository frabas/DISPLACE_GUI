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

#include "statscontroller.h"
#include <displacemodel.h>
#include <QVector>
#include <QtAlgorithms>

#include "PlotWidget.h"
#include <plots/benthosstatsplot.h>
#include <plots/fishfarmsstatsplot.h>
#include <plots/windfarmsstatsplot.h>
#include <plots/shipsstatsplot.h>
#include <plots/nationsstatsplot.h>
#include <plots/populationsstatplot.h>
#include <plots/harboursstatplot.h>
#include <plots/metiersstatsplot.h>

#include <functional>

double StatsController::timelineMax = 1e20;
double StatsController::timelineMin = -1e20;

StatsController::StatsController(QObject *parent)
    : QObject(parent),
      mPalette(),
      mSelectedPopStat(displace::plot::PopulationStat::Aggregate),
      mPopPlot(0),
      mSelectedHarboursStat(displace::plot::HarboursStat::H_Catches),
      mPlotHarbours(0),
      mSelectedNationsStat(displace::plot::NationsStat::Catches),
      mNatTimeLine(0),
      mPlotMetiers(0),
      mSelectedMetiersStat(displace::plot::MetiersStat::M_Catches),
      mLastModel(0)
{
    mPalette = PaletteManager::instance()->palette(PopulationRole);

    cout << "Stats controller is created" << endl;
}

void StatsController::setPopulationPlot(PlotWidget *plot, GraphInteractionController *controller)
{
    if (mPopPlot != nullptr)
        delete mPopPlot;

    mPopPlot = new PopulationsStatPlot(plot);
    plot->setStatsPlot(mPopPlot);
    plot->legend->setVisible(true);

    controller->setOnPopupMenuBuiltCallback(std::bind(&PopulationsStatPlot::createPopup, mPopPlot, std::placeholders::_1, std::placeholders::_2));
}

void StatsController::setHarboursPlot(QCustomPlot *plot)
{
    if (mPlotHarbours != nullptr)
        delete mPlotHarbours;
    mPlotHarbours = new HarboursStatPlot(plot);
}

void StatsController::setMetiersPlot(QCustomPlot *plot)
{
    if (mPlotMetiers != nullptr)
        delete mPlotMetiers;
    mPlotMetiers = new MetiersStatsPlot(plot);
}

void StatsController::setBenthosPlot(QCustomPlot *plot, GraphInteractionController *controller)
{
    mBenthosFuncGroupsPlot = plot;
    mBenthosFuncGroupsPlot->legend->setVisible(true);

    if (mBenthosTimeLine != 0)
        delete mBenthosTimeLine;

    if (mBenthosPlotController != nullptr)
        delete mBenthosPlotController;

    mBenthosTimeLine = new QCPItemLine(mBenthosFuncGroupsPlot);
    mBenthosPlotController = new BenthosStatsPlot(plot, mBenthosTimeLine);

    controller->setOnPopupMenuBuiltCallback(std::bind(&BenthosStatsPlot::createPopup, mBenthosPlotController, std::placeholders::_1, std::placeholders::_2));

}

void StatsController::setFishfarmsPlot(QCustomPlot *plot, GraphInteractionController *controller)
{
   cout << "Set fishfarm plot" << endl;

    mfarmTypeGroupsPlot = plot;
    mfarmTypeGroupsPlot->legend->setVisible(true);

    if (mFishfarmsTimeLine != 0)
        delete mFishfarmsTimeLine;

    if (mFishfarmsPlotController != nullptr)
        delete mFishfarmsPlotController;

    mFishfarmsTimeLine = new QCPItemLine(mfarmTypeGroupsPlot);
    mFishfarmsPlotController = new FishfarmsStatsPlot(plot, mFishfarmsTimeLine);

    cout << "Set fishfarm plot...ok" << endl;

    controller->setOnPopupMenuBuiltCallback(std::bind(&FishfarmsStatsPlot::createPopup, mFishfarmsPlotController, std::placeholders::_1, std::placeholders::_2));
}


void StatsController::setWindfarmsPlot(QCustomPlot *plot, GraphInteractionController *controller)
{
   cout << "Set windfarm plot" << endl;

    mWindfarmTypeGroupsPlot = plot;
    mWindfarmTypeGroupsPlot->legend->setVisible(true);

    if (mWindfarmsTimeLine != 0)
        delete mWindfarmsTimeLine;

    if (mWindfarmsPlotController != nullptr)
        delete mWindfarmsPlotController;

    mWindfarmsTimeLine = new QCPItemLine(mWindfarmTypeGroupsPlot);
    mWindfarmsPlotController = new WindfarmsStatsPlot(plot, mWindfarmsTimeLine);

    cout << "Set windfarm plot...ok" << endl;

    controller->setOnPopupMenuBuiltCallback(std::bind(&WindfarmsStatsPlot::createPopup, mWindfarmsPlotController, std::placeholders::_1, std::placeholders::_2));
}


void StatsController::setShipsPlot(QCustomPlot *plot, GraphInteractionController *controller)
{
   cout << "Set ships plot" << endl;

    mShipTypeGroupsPlot = plot;
    mShipTypeGroupsPlot->legend->setVisible(true);

    if (mShipsTimeLine != 0)
        delete mShipsTimeLine;

    if (mShipsPlotController != nullptr)
        delete mShipsPlotController;

    mShipsTimeLine = new QCPItemLine(mShipTypeGroupsPlot);
    mShipsPlotController = new ShipsStatsPlot(plot, mShipsTimeLine);

    cout << "Set windfarm plot...ok" << endl;

    controller->setOnPopupMenuBuiltCallback(std::bind(&ShipsStatsPlot::createPopup, mShipsPlotController, std::placeholders::_1, std::placeholders::_2));
}

void StatsController::setNationsStatsPlot(PlotWidget *plot, GraphInteractionController *controller)
{
    mNationsPlot = plot;
    mNationsPlot->legend->setVisible(true);
    if (mNatTimeLine != nullptr) delete mNatTimeLine;
    if (mNationsStatsPlotController != nullptr) delete mNationsStatsPlotController;

    mNatTimeLine = new QCPItemLine(mNationsPlot);
    mNationsStatsPlotController = new NationsStatsPlot(plot, mNatTimeLine);
    plot->setStatsPlot(mNationsStatsPlotController);
    controller->setOnPopupMenuBuiltCallback(std::bind(&NationsStatsPlot::createPopup, mNationsStatsPlotController, std::placeholders::_1, std::placeholders::_2));
}


void StatsController::updateStats(DisplaceModel *model)
{
    if (!model)
        return;

    updatePopulationStats(model, mSelectedPopStat, nullptr);
    if (mNationsStatsPlotController) {
        updateNationStats(model, mSelectedNationsStat, nullptr);
    }

    updateHarboursStats(model, nullptr);
    updateMetiersStats(model, nullptr);

    if (mBenthosFuncGroupsPlot) {
        updateBenthosStats(model, mSelectedBenthosStat);
    }
    if (mfarmTypeGroupsPlot) {
        updateFishfarmsStats(model, mSelectedFishfarmsStat);
    }
    if (mShipTypeGroupsPlot) {
        updateShipsStats(model, mSelectedShipsStat);
    }
    if (mWindfarmTypeGroupsPlot) {
        updateWindfarmsStats(model, mSelectedWindfarmsStat);
    }

    mLastModel = model;
}

void StatsController::setPopulationStat(displace::plot::PopulationStat stat)
{
    mSelectedPopStat = stat;
    updateStats(mLastModel);
}

void StatsController::setNationsStat(displace::plot::NationsStat stat)
{
    mSelectedNationsStat = stat;
    updateStats(mLastModel);
}

void StatsController::setHarbourStat(displace::plot::HarboursStat stat)
{
    mSelectedHarboursStat = stat;
    updateStats(mLastModel);
}

void StatsController::setMetiersStat(displace::plot::MetiersStat stat)
{
    mSelectedMetiersStat = stat;
    updateStats(mLastModel);
}

void StatsController::setBenthosStat(displace::plot::BenthosStat stat)
{
    mSelectedBenthosStat = stat;
    updateStats(mLastModel);
}

void StatsController::setFishfarmsStat(displace::plot::FishfarmsStat stat)
{
    mSelectedFishfarmsStat = stat;
    updateStats(mLastModel);
}

void StatsController::setWindfarmsStat(displace::plot::WindfarmsStat stat)
{
    mSelectedWindfarmsStat = stat;
    updateStats(mLastModel);
}

void StatsController::setShipsStat(displace::plot::ShipsStat stat)
{
    mSelectedShipsStat = stat;
    updateStats(mLastModel);
}

void StatsController::setCurrentTimeStep(double t)
{
    mPopPlot->setCurrentTimeStep(t);
    mPlotHarbours->setCurrentTimeStep(t);
    mPlotMetiers->setCurrentTimeStep(t);

    mNatTimeLine->start->setCoords(t, timelineMin);
    mNatTimeLine->end->setCoords(t, timelineMax);

}

void StatsController::plotGraph(DisplaceModel *model, StatsController::StatType st, int subtype, QCustomPlot *plot)
{
    switch (st) {
    case Populations:
        updatePopulationStats(model, static_cast<displace::plot::PopulationStat>(subtype), plot);
        break;
    case Nations:
        updateNationStats(model, static_cast<displace::plot::NationsStat>(subtype), plot);
        break;
    case Harbours:
        updateHarboursStats(model,plot);
        break;
    case Metiers:
        updateMetiersStats(model,plot);
        break;
    }
}

void StatsController::updatePopulationStats(DisplaceModel *model, displace::plot::PopulationStat popStat, QCustomPlot *plot)
{
    if (!mPopPlot)
        return;

    mPopPlot->update(model, popStat, plot);
}

void StatsController::updateNationStats(DisplaceModel *model, displace::plot::NationsStat nationsStat, QCustomPlot *plot)
{
    mNationsStatsPlotController->update(model, nationsStat, plot);
}

void StatsController::updateHarboursStats(DisplaceModel *model, QCustomPlot *plot)
{
    if (mPlotHarbours) {
        mPlotHarbours->setStat(mSelectedHarboursStat);
        mPlotHarbours->update(model, plot);
    }
}

void StatsController::updateMetiersStats(DisplaceModel *model, QCustomPlot *plot)
{
    if (mPlotMetiers) {
        mPlotMetiers->setStat (mSelectedMetiersStat);
        mPlotMetiers->update(model,plot);
    }
}

void StatsController::updateBenthosStats(DisplaceModel *model, displace::plot::BenthosStat stat)
{
    mBenthosPlotController->update(model, stat);
}

void StatsController::updateFishfarmsStats(DisplaceModel *model, displace::plot::FishfarmsStat stat)
{
    mFishfarmsPlotController->update(model, stat);
}

void StatsController::updateWindfarmsStats(DisplaceModel *model, displace::plot::WindfarmsStat stat)
{
    mWindfarmsPlotController->update(model, stat);
}

void StatsController::updateShipsStats(DisplaceModel *model, displace::plot::ShipsStat stat)
{
    mShipsPlotController->update(model, stat);
}

