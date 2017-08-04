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

#include <functional>

double StatsController::timelineMax = 1e20;
double StatsController::timelineMin = -1e20;

StatsController::StatsController(QObject *parent)
    : QObject(parent),
      mPalette(),
      mPlotPopulations(0),
      mSelectedPopStat(Aggregate),
      mPopTimeLine(0),
      mPlotHarbours(0),
      mHarbTimeLine(0),
      mPlotNations(0),
      mSelectedNationsStat(Catches),
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
    mPlotPopulations = plot;
    mPlotPopulations->legend->setVisible(true);

    if (mPopTimeLine != 0)
        delete mPopTimeLine;

    mPopTimeLine = new QCPItemLine(mPlotPopulations);
    mPlotPopulations->addItem(mPopTimeLine);
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

void StatsController::setNationsPlot(QCustomPlot *plot)
{
    mPlotNations = plot;
    mPlotNations->legend->setVisible(true);

    if (mNatTimeLine != 0)
        delete mNatTimeLine;

    mNatTimeLine = new QCPItemLine(mPlotNations);
    mPlotNations->addItem(mNatTimeLine);
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



void StatsController::updateStats(DisplaceModel *model)
{
    if (!model)
        return;

    if (mPlotPopulations) {
        updatePopulationStats(model, mSelectedPopStat, mPlotPopulations, mPopTimeLine);
    }
    if (mPlotNations) {
        updateNationStats(model, mSelectedNationsStat, mPlotNations, mNatTimeLine);
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


    mLastModel = model;
}

void StatsController::setPopulationStat(StatsController::PopulationStat stat)
{
    mSelectedPopStat = stat;
    updateStats(mLastModel);
}

void StatsController::setNationsStat(StatsController::NationsStat stat)
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


void StatsController::initPlots()
{
    if (mPlotPopulations) {

    }
}

void StatsController::setCurrentTimeStep(double t)
{
    mPopTimeLine->start->setCoords(t, timelineMin);
    mPopTimeLine->end->setCoords(t, timelineMax);

    mHarbTimeLine->start->setCoords(t, timelineMin);
    mHarbTimeLine->end->setCoords(t, timelineMax);

    mNatTimeLine->start->setCoords(t, timelineMin);
    mNatTimeLine->end->setCoords(t, timelineMax);

}

void StatsController::plotGraph(DisplaceModel *model, StatsController::StatType st, int subtype, QCustomPlot *plot, QCPItemLine *line)
{
    switch (st) {
    case Populations:
        updatePopulationStats(model, static_cast<PopulationStat>(subtype), plot, line);
        break;
    case Nations:
        updateNationStats(model, static_cast<NationsStat>(subtype), plot, line);
        break;
    case Harbours:
        updateHarboursStats(model, static_cast<HarboursStat>(subtype), plot, line);
        break;
    case Metiers:
        updateMetiersStats(model, static_cast<MetiersStat>(subtype), plot, line);
        break;



    }
}

void StatsController::updatePopulationStats(DisplaceModel *model, PopulationStat popStat, QCustomPlot *plotPopulations, QCPItemLine *timeline)
{
    static const QPen pen(QColor(0,0,255,200));
    plotPopulations->clearGraphs();
    double val;

    QList<int> interPopList = model->getInterestingPops();
    QList<int> interSizeList = model->getInterestingSizes();
    QList<int> graphList;
    bool showtotal = model->isInterestingSizeTotal();
    bool showavg =  model->isInterestingSizeAvg();
    bool showmin =  model->isInterestingSizeMin();
    bool showmax =  model->isInterestingSizeMax();

    if (showmax)
        graphList.push_front(-4);
    if (showmin)
        graphList.push_front(-3);
    if (showavg)
        graphList.push_front(-2);
    if (showtotal)
        graphList.push_front(-1);

    if (graphList.size() == 0)
        graphList.append(interSizeList);

    /* If no size is selected, but aggregate is selected, select all sizes */
    if (interSizeList.size() == 0 && graphList.size() != 0) {
        for(int i = 0; i < model->getSzGrupsCount(); ++i)
            interSizeList.push_back(i);
    }

    int szNum = interSizeList.size();
    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;
    QList<QVector<double> >keyData;
    QList<QVector<double> >valueData;

    double t = model->getCurrentStep();
    if (timeline != nullptr) {
        timeline->start->setCoords(t, timelineMin);
        timeline->end->setCoords(t, timelineMax);
    }

    foreach (int ipop, interPopList) {
        for (int igraph = 0; igraph < graphNum; ++igraph) {
            // Creates graph. Index in list are: ip * nsz + isz
            QCPGraph *graph = plotPopulations->addGraph();
            QColor col = mPalette.colorByIndex(ipop);

            graph->setLineStyle(QCPGraph::lsLine);
            graph->setPen(QPen(QBrush(col),2));

            col.setAlpha(128);
            graph->setBrush(QBrush(col));

            switch (graphList[igraph]) {
            case -4:
                graph->setName(QString(QObject::tr("Pop %1 Max")).arg(ipop));
                break;
            case -3:
                graph->setName(QString(QObject::tr("Pop %1 Min")).arg(ipop));
                break;
            case -2:
                graph->setName(QString(QObject::tr("Pop %1 Avg")).arg(ipop));
                break;
            case -1:
                graph->setName(QString(QObject::tr("Pop %1 Total")).arg(ipop));
                break;
            default:
                graph->setName(QString(QObject::tr("Pop %1 Group %2")).arg(ipop).arg(graphList[igraph]+1));
            }

            graphs.push_back(graph);
            keyData.push_back(QVector<double>());
            valueData.push_back(QVector<double>());
        }
    }

    int nsteps = model->getPopulationsValuesCount();

    DisplaceModel::PopulationStatContainer::Container::const_iterator it = model->getPopulationsFirstValue();
    for (int istep = 0; istep <nsteps; ++istep) {
        int ninterPop = interPopList.size();
        for (int iinterpPop = 0; iinterpPop < ninterPop; ++iinterpPop) {

            // calculate transversal values...
            double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0;
            for (int iInterSize = 0; iInterSize < interSizeList.size(); ++iInterSize) {
                val = getPopStatValue(model, it.key(), interPopList[iinterpPop], interSizeList[iInterSize], popStat);
                if (iInterSize == 0) {
                    mMin = val;
                    mMax = val;
                } else {
                    if (mMin > val)
                        mMin = val;
                    if (mMax < val)
                        mMax = val;
                }
                mAvg += val;
                mTot += val;
            }
            if (szNum > 0)
                mAvg /= szNum;

            for (int isz = 0; isz < graphNum; ++isz) {
                int gidx = iinterpPop * graphNum + isz;

                keyData[gidx] << it.key();
                switch (graphList[isz]) {
                case -4:
                    val = mMax;
                    break;
                case -3:
                    val = mMin;
                    break;
                case -2:
                    val = mAvg;
                    break;
                case -1:
                    val = mTot;
                    break;
                default:
                    val = getPopStatValue(model, it.key(), interPopList[iinterpPop], graphList[isz], popStat);
                    break;
                }

                valueData[gidx] << val;
            }
        }
        ++it;
    }

    for (int i = 0; i < graphs.size(); ++i) {
        graphs[i]->setData(keyData.at(i), valueData.at(i));
    }


    switch (popStat) {
    case Aggregate:
        plotPopulations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotPopulations->yAxis->setLabel(QObject::tr("Numbers ('000)"));
        break;
    case Mortality:
        plotPopulations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotPopulations->yAxis->setLabel(QObject::tr("F"));
        break;
    case SSB:
        plotPopulations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotPopulations->yAxis->setLabel(QObject::tr("SSB (kg)"));
        break;
    }


    plotPopulations->rescaleAxes();
    plotPopulations->replot();
}

double StatsController::getPopStatValue(DisplaceModel *model, int tstep, int popid, int szid, StatsController::PopulationStat stattype)
{
    switch (stattype) {
    case Aggregate:
        return model->getPopulationsAtStep(tstep, popid).getAggregateAt(szid);
    case Mortality:
        return model->getPopulationsAtStep(tstep, popid).getMortalityAt(szid);
    case SSB: {
        auto &x = model->getPopulationsAtStep(tstep, popid).getSSB();
        if (szid < x.size())
            return x.at(szid);
        return 0;
        }
    }

    return 0;
}

void StatsController::updateNationStats(DisplaceModel *model, NationsStat nationsStat, QCustomPlot *plotNations, QCPItemLine *timeLine)
{
    static const QPen pen(QColor(0,0,255,200));
    plotNations->clearGraphs();

    QList<int> ipl = model->getInterestingNations();

    double t = model->getCurrentStep();
    if (timeLine != nullptr ) {
        timeLine->start->setCoords(t, timelineMin);
        timeLine->end->setCoords(t, timelineMax);
    }

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();
    foreach (int ip, ipl) {
        if (col_it == mPalette.end())
            col_it = mPalette.begin();

        QVector<double> keyData;
        QVector<double> valueData;

        QCPGraph *graph = plotNations->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(QString(model->getNation(ip).getName()));

        int n = model->getNationsStatsCount();
        DisplaceModel::NationsStatsContainer::Container::const_iterator it = model->getNationsStatsFirstValue();
        for (int i = 0; i <n; ++i) {
            if (it.value().size() > ip) {
                keyData << it.key();

                switch (nationsStat) {
                case Catches:
                    valueData << it.value().at(ip).mTotCatches;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Landings (kg)"));
                    break;
                case Discards:
                    valueData << it.value().at(ip).mTotDiscards;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Discards (kg)"));
                    break;
                case Earnings:
                    valueData << it.value().at(ip).mRevenues;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
                    break;
                case ExEarnings:
                    valueData << it.value().at(ip).mExRevenues;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
                    break;
                case TimeAtSea:
                    valueData << it.value().at(ip).mTimeAtSea;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Time at sea (h)"));
                    break;
                case Gav:
                    valueData << it.value().at(ip).mGav;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("GAV (Euro)"));
                    break;
                case Vpuf:
                    valueData << it.value().at(ip).mVpuf;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
                    break;
                case SweptArea:
                    valueData << it.value().at(ip).mSweptArea;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
                    break;
                case RevenuePerSweptArea:
                    valueData << it.value().at(ip).mRevenuePerSweptArea;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Revenue Per Swept Area (euro/m^2)"));
                    break;
                }
            }
            ++it;
        }

        graph->setData(keyData, valueData);

        ++col_it;
    }

    plotNations->rescaleAxes();
    plotNations->replot();
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
