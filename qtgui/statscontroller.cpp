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

#include <plots/benthosstatsplot.h>
#include <plots/fishfarmsstatsplot.h>
#include <plots/windfarmsstatsplot.h>
#include <plots/shipsstatsplot.h>
#include <plots/nationsstatsplot.h>
#include <plots/populationsstatplot.h>

#include <functional>

double StatsController::timelineMax = 1e20;
double StatsController::timelineMin = -1e20;

StatsController::StatsController(QObject *parent)
    : QObject(parent),
      mPalette(),
      mSelectedPopStat(displace::plot::PopulationStat::Aggregate),
      mPopPlot(0),
      mPlotHarbours(0),
      mHarbTimeLine(0),
      mSelectedNationsStat(displace::plot::NationsStat::Catches),
      mNatTimeLine(0),
      mPlotMetiers(0),
      mSelectedMetiersStat(M_Catches),
      mMetTimeLine(0),
      mLastModel(0)
{
    mPalette = PaletteManager::instance()->palette(PopulationRole);

    cout << "Stats controller is created" << endl;
}

void StatsController::setPopulationPlot(QCustomPlot *plot)
{
    if (mPopPlot != nullptr)
        delete mPopPlot;

    plot->legend->setVisible(true);
    mPopPlot = new PopulationsStatPlot(plot);
}

void StatsController::setHarboursPlot(QCustomPlot *plot)
{
    mPlotHarbours = plot;
    mPlotHarbours->legend->setVisible(true);

    if (mHarbTimeLine != 0)
        delete mHarbTimeLine;

    mHarbTimeLine = new QCPItemLine(mPlotHarbours);
    mPlotHarbours->addItem(mHarbTimeLine);
}

void StatsController::setMetiersPlot(QCustomPlot *plot)
{
    mPlotMetiers = plot;
    mPlotMetiers->legend->setVisible(true);

    if (mMetTimeLine != 0)
        delete mMetTimeLine;

    mMetTimeLine = new QCPItemLine(mPlotMetiers);
    mPlotMetiers->addItem(mMetTimeLine);
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

    mBenthosFuncGroupsPlot->addItem(mBenthosTimeLine);

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

    mfarmTypeGroupsPlot->addItem(mFishfarmsTimeLine);

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

    mWindfarmTypeGroupsPlot->addItem(mWindfarmsTimeLine);

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

    mShipTypeGroupsPlot->addItem(mShipsTimeLine);

    cout << "Set windfarm plot...ok" << endl;

    controller->setOnPopupMenuBuiltCallback(std::bind(&ShipsStatsPlot::createPopup, mShipsPlotController, std::placeholders::_1, std::placeholders::_2));
}

void StatsController::setNationsStatsPlot(QCustomPlot *plot, GraphInteractionController *controller)
{
    mNationsPlot = plot;
    mNationsPlot->legend->setVisible(true);
    if (mNatTimeLine != nullptr) delete mNatTimeLine;
    if (mNationsStatsPlotController != nullptr) delete mNationsStatsPlotController;

    mNatTimeLine = new QCPItemLine(mNationsPlot);
    mNationsStatsPlotController = new NationsStatsPlot(plot, mNatTimeLine);
    mNationsPlot->addItem(mNatTimeLine);
    controller->setOnPopupMenuBuiltCallback(std::bind(&NationsStatsPlot::createPopup, mNationsStatsPlotController, std::placeholders::_1, std::placeholders::_2));
}


void StatsController::updateStats(DisplaceModel *model)
{
    if (!model)
        return;

    updatePopulationStats(model, mSelectedPopStat);
    if (mNationsStatsPlotController) {
        updateNationStats(model, mSelectedNationsStat);
    }
    if (mPlotHarbours) {
        updateHarboursStats(model, mSelectedHarboursStat, mPlotHarbours, mHarbTimeLine);
    }
    if (mPlotMetiers) {
        updateMetiersStats(model, mSelectedMetiersStat, mPlotMetiers, mMetTimeLine);
    }
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

void StatsController::setHarbourStat(StatsController::HarboursStat stat)
{
    mSelectedHarboursStat = stat;
    updateStats(mLastModel);
}

void StatsController::setMetiersStat(StatsController::MetiersStat stat)
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

    mHarbTimeLine->start->setCoords(t, timelineMin);
    mHarbTimeLine->end->setCoords(t, timelineMax);

    mNatTimeLine->start->setCoords(t, timelineMin);
    mNatTimeLine->end->setCoords(t, timelineMax);

}

void StatsController::plotGraph(DisplaceModel *model, StatsController::StatType st, int subtype, QCustomPlot *plot, QCPItemLine *line)
{
    switch (st) {
    case Populations:
        updatePopulationStats(model, static_cast<displace::plot::PopulationStat>(subtype));
        break;
        /*
    case Nations:
        updateNationStats(model, static_cast<displace::plot::NationsStat>(subtype), plot, line);
        break;*/
    case Harbours:
        updateHarboursStats(model, static_cast<HarboursStat>(subtype), plot, line);
        break;
    case Metiers:
        updateMetiersStats(model, static_cast<MetiersStat>(subtype), plot, line);
        break;
    }
}

void StatsController::updatePopulationStats(DisplaceModel *model, displace::plot::PopulationStat popStat)
{
    if (!mPopPlot)
        return;

    mPopPlot->update(model, popStat);
}

void StatsController::updateNationStats(DisplaceModel *model, displace::plot::NationsStat nationsStat)
{
    mNationsStatsPlotController->update(model, nationsStat);
}

void StatsController::updateHarboursStats(DisplaceModel *model, HarboursStat stat, QCustomPlot *plot, QCPItemLine *timeline)
{
    static const QPen pen(QColor(0,0,255,200));
    plot->clearGraphs();

    auto ipl = model->getInterestingHarbours();

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();

    double t = model->getCurrentStep();
    if (timeline != nullptr ) {
        timeline->start->setCoords(t, timelineMin);
        timeline->end->setCoords(t, timelineMax);
    }

    foreach (auto ip, ipl) {
        if (col_it == mPalette.end())
            col_it = mPalette.begin();

        QVector<double> keyData;
        QVector<double> valueData;

        QCPGraph *graph = plot->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(QString::fromStdString(model->getHarbourData(ip.toIndex()).mHarbour->get_name()));

        int n = model->getHarboursStatsCount();
        DisplaceModel::HarboursStatsContainer::Container::const_iterator it = model->getHarboursStatsFirstValue();
        for (int i = 0; i <n; ++i) {
            if (it.value().size() > ip.toIndex()) {
                keyData << it.key();

                switch (stat) {
                case H_Catches:
                    valueData << it.value().at(ip.toIndex()).mCumCatches;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("Landings (kg)"));
                    break;
                case H_Discards:
                    valueData << it.value().at(ip.toIndex()).mCumDiscards;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("Discards (kg)"));
                    break;
                case H_Earnings:
                    valueData << it.value().at(ip.toIndex()).mCumProfit;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
                    break;
                case H_Gav:
                    valueData << it.value().at(ip.toIndex()).mGav;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("GAV (Euro)"));
                    break;
                case H_Vpuf:
                    valueData << it.value().at(ip.toIndex()).mVpuf;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
                    break;
                case H_SweptArea:
                    valueData << it.value().at(ip.toIndex()).mSweptArea;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
                    break;
                case H_RevenuePerSweptArea:
                    valueData << it.value().at(ip.toIndex()).mRevenuePerSweptArea;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("Revenue per Swept Area (Euro/km^2)"));
                    break;
                case H_GVA:
                    valueData << it.value().at(ip.toIndex()).GVA;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("euro"));
                    break;
                case H_GVAPerRevenue:
                    valueData << it.value().at(ip.toIndex()).GVAPerRevenue;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
                    break;
                case H_LabourSurplus:
                    valueData << it.value().at(ip.toIndex()).LabourSurplus;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("euro"));
                    break;
                case H_GrossProfit:
                    valueData << it.value().at(ip.toIndex()).GrossProfit;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("euro"));
                    break;
                case H_NetProfit:
                    valueData << it.value().at(ip.toIndex()).NetProfit;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("euro"));
                    break;
                case H_NetProfitMargin:
                    valueData << it.value().at(ip.toIndex()).NetProfitMargin;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("%"));
                    break;
                case H_GVAPerFTE:
                    valueData << it.value().at(ip.toIndex()).GVAPerFTE;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("euro"));
                    break;
                case H_RoFTA:
                    valueData << it.value().at(ip.toIndex()).RoFTA;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("%"));
                    break;
                case H_BER:
                    valueData << it.value().at(ip.toIndex()).BER;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("euro"));
                    break;
                case H_CRBER:
                    valueData << it.value().at(ip.toIndex()).CRBER;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("euro"));
                    break;
                case H_NetPresentValue:
                    valueData << it.value().at(ip.toIndex()).NetPresentValue;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("euro"));
                    break;
                case H_numTrips:
                    valueData << it.value().at(ip.toIndex()).numTrips;
                    plot->xAxis->setLabel(QObject::tr("Time (h)"));
                    plot->yAxis->setLabel(QObject::tr("#"));
                    break;


                }
            }
            ++it;
        }

        graph->setData(keyData, valueData);

        ++col_it;
    }

    plot->rescaleAxes();
    plot->replot();
}

void StatsController::updateMetiersStats(DisplaceModel *model, MetiersStat metStat, QCustomPlot *plotMetiers, QCPItemLine *metTimeLine)
{
    static const QPen pen(QColor(0,0,255,200));
    plotMetiers->clearGraphs();

    auto &dl = model->getMetiersList();

    double t = model->getCurrentStep();
    if (metTimeLine != nullptr) {
        metTimeLine->start->setCoords(t, timelineMin);
        metTimeLine->end->setCoords(t, timelineMax);
    }

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();

    switch (metStat) {
    case M_Catches:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case M_Discards:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Discards (kg)"));
        break;
    case M_Revenues:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case M_Gav:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("GAV (Euro)"));
        break;
    case M_Vpuf:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("VPUF (Euro per Litre)"));
        break;
    case M_SweptArea:
       plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
        break;
    case M_RevenuesPerSweptArea:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Revenue Per Swept Area (euro/m^2)"));
        break;
    case M_GVA:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("euro"));
        break;
    case M_GVAPerRevenue:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
        break;
    case M_LabourSurplus:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case M_GrossProfit:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case M_NetProfit:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case M_NetProfitMargin:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("%"));
        break;
    case M_GVAPerFTE:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case M_RoFTA:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case M_BER:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case M_CRBER:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Ratio"));
        break;
    case M_NetPresentValue:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case M_numTrips:
        plotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        plotMetiers->yAxis->setLabel(QObject::tr("#"));
        break;

    }

    foreach (auto  d, dl) {
        if (!d->ticked)
            continue;

        if (col_it == mPalette.end())
            col_it = mPalette.begin();

        QVector<double> keyData;
        QVector<double> valueData;

        QCPGraph *graph = plotMetiers->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(d->description());

        int n = model->getMetiersStatsCount();
        auto it = model->getMetiersStatsFirstValue();

        int ip = d->metierId;

        for (int i = 0; i <n; ++i) {
            if (it.value().size() > ip) {
                keyData << it.key();

                switch (metStat) {
                case M_Catches:
                    if (d->populationId == -1)
                        valueData << it.value().at(ip).mTotCatches;
                    else {
                        if (it.value().at(ip).mCatchesPerPop.size() > d->populationId)
                            valueData << it.value().at(ip).mCatchesPerPop[d->populationId];
                        else
                            valueData << 0;
                    }
                    break;
                case M_Discards:
                    if (d->populationId == -1)
                        valueData << it.value().at(ip).mTotDiscards;
                    else {
                        if (it.value().at(ip).mDiscardsPerPop.size() > d->populationId)
                            valueData << it.value().at(ip).mDiscardsPerPop[d->populationId];
                        else
                            valueData << 0;
                    }
                    break;
                case M_Revenues:
                    valueData << it.value().at(ip).revenueAV;
                    break;
                case M_Gav:
                    valueData << it.value().at(ip).gav;
                    break;
                case M_Vpuf:
                    valueData << it.value().at(ip).vpuf;
                    break;
                case M_SweptArea:
                    valueData << it.value().at(ip).mSweptArea;
                    break;
                case M_RevenuesPerSweptArea:
                    valueData << it.value().at(ip).mRevenuePerSweptArea;
                    break;                   
                case M_GVA:
                    valueData << it.value().at(ip).GVA;
                    break;
                case M_GVAPerRevenue:
                    valueData << it.value().at(ip).GVAPerRevenue;
                    break;
                case M_LabourSurplus:
                    valueData << it.value().at(ip).LabourSurplus;
                    break;
                case M_GrossProfit:
                    valueData << it.value().at(ip).GrossProfit;
                    break;
                case M_NetProfit:
                    valueData << it.value().at(ip).NetProfit;
                    break;
                case M_NetProfitMargin:
                    valueData << it.value().at(ip).NetProfitMargin;
                    break;
                case M_GVAPerFTE:
                    valueData << it.value().at(ip).GVAPerFTE;
                    break;
                case M_RoFTA:
                    valueData << it.value().at(ip).RoFTA;
                    break;
                case M_BER:
                    valueData << it.value().at(ip).BER;
                    break;
                case M_CRBER:
                    valueData << it.value().at(ip).CRBER;
                    break;
                case M_NetPresentValue:
                    valueData << it.value().at(ip).NetPresentValue;
                    break;
                case M_numTrips:
                    valueData << it.value().at(ip).numTrips;
                    break;



                }
            }
            ++it;
        }

        graph->setData(keyData, valueData);

        ++col_it;
    }

    plotMetiers->rescaleAxes();
    plotMetiers->replot();
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

