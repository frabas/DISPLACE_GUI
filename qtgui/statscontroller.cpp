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

#include "statscontroller.h"
#include <displacemodel.h>
#include <QVector>
#include <QtAlgorithms>

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

void StatsController::updateStats(DisplaceModel *model)
{
    if (!model)
        return;

    if (mPlotPopulations) {
        updatePopulationStats(model);
    }
    if (mPlotNations) {
        updateNationStats(model);
    }
    if (mPlotHarbours) {
        updateHarboursStats(model);
    }
    if (mPlotMetiers) {
        updateMetiersStats(model);
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

void StatsController::updatePopulationStats(DisplaceModel *model)
{
    static const QPen pen(QColor(0,0,255,200));
    mPlotPopulations->clearGraphs();
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
    mPopTimeLine->start->setCoords(t, timelineMin);
    mPopTimeLine->end->setCoords(t, timelineMax);

    foreach (int ipop, interPopList) {
        for (int igraph = 0; igraph < graphNum; ++igraph) {
            // Creates graph. Index in list are: ip * nsz + isz
            QCPGraph *graph = mPlotPopulations->addGraph();
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
                val = getPopStatValue(model, it.key(), interPopList[iinterpPop], interSizeList[iInterSize], mSelectedPopStat);
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
                    val = getPopStatValue(model, it.key(), interPopList[iinterpPop], graphList[isz], mSelectedPopStat);
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


    switch (mSelectedPopStat) {
    case Aggregate:
        mPlotPopulations->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlotPopulations->yAxis->setLabel(QObject::tr("Numbers"));
        break;
    case Mortality:
        mPlotPopulations->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlotPopulations->yAxis->setLabel(QObject::tr("F"));
        break;
    }


    mPlotPopulations->rescaleAxes();
    mPlotPopulations->replot();
}

double StatsController::getPopStatValue(DisplaceModel *model, int tstep, int popid, int szid, StatsController::PopulationStat stattype)
{
    switch (stattype) {
    case Aggregate:
        return model->getPopulationsAtStep(tstep, popid).getAggregate().at(szid);
    case Mortality:
        return model->getPopulationsAtStep(tstep, popid).getMortality().at(szid);
    }

    return 0;
}

void StatsController::updateNationStats(DisplaceModel *model)
{
    static const QPen pen(QColor(0,0,255,200));
    mPlotNations->clearGraphs();

    QList<int> ipl = model->getInterestingNations();

    double t = model->getCurrentStep();
    mNatTimeLine->start->setCoords(t, timelineMin);
    mNatTimeLine->end->setCoords(t, timelineMax);

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();
    foreach (int ip, ipl) {
        if (col_it == mPalette.end())
            col_it = mPalette.begin();

        QVector<double> keyData;
        QVector<double> valueData;

        QCPGraph *graph = mPlotNations->addGraph();
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

                switch (mSelectedNationsStat) {
                case Catches:
                    valueData << it.value().at(ip).mTotCatches;
                    mPlotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotNations->yAxis->setLabel(QObject::tr("Landings (kg)"));
                    break;
                case Earnings:
                    valueData << it.value().at(ip).mRevenues;
                    mPlotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotNations->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
                    break;
                case ExEarnings:
                    valueData << it.value().at(ip).mExRevenues;
                    mPlotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotNations->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
                    break;
                case TimeAtSea:
                    valueData << it.value().at(ip).mTimeAtSea;
                    mPlotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotNations->yAxis->setLabel(QObject::tr("Time at sea (h)"));
                    break;
                case Gav:
                    valueData << it.value().at(ip).mGav;
                    mPlotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotNations->yAxis->setLabel(QObject::tr("GAV (Euro)"));
                    break;
                case Vpuf:
                    valueData << it.value().at(ip).mVpuf;
                    mPlotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotNations->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
                    break;
                }
            }
            ++it;
        }

        graph->setData(keyData, valueData);

        ++col_it;
    }

    mPlotNations->rescaleAxes();
    mPlotNations->replot();    
}

void StatsController::updateHarboursStats(DisplaceModel *model)
{
    static const QPen pen(QColor(0,0,255,200));
    mPlotHarbours->clearGraphs();

    QList<int> ipl = model->getInterestingHarbours();

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();

    double t = model->getCurrentStep();
    mHarbTimeLine->start->setCoords(t, timelineMin);
    mHarbTimeLine->end->setCoords(t, timelineMax);

    foreach (int ip, ipl) {
        if (col_it == mPalette.end())
            col_it = mPalette.begin();

        QVector<double> keyData;
        QVector<double> valueData;

        QCPGraph *graph = mPlotHarbours->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(QString::fromStdString(model->getHarbourData(ip).mHarbour->get_name()));

        int n = model->getHarboursStatsCount();
        DisplaceModel::HarboursStatsContainer::Container::const_iterator it = model->getHarboursStatsFirstValue();
        for (int i = 0; i <n; ++i) {
            if (it.value().size() > ip) {
                keyData << it.key();

                switch (mSelectedHarboursStat) {
                case H_Catches:
                    valueData << it.value().at(ip).mCumCatches;
                    mPlotHarbours->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotHarbours->yAxis->setLabel(QObject::tr("Landings (kg)"));
                    break;
                case H_Earnings:
                    valueData << it.value().at(ip).mCumProfit;
                    mPlotHarbours->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotHarbours->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
                    break;
                case H_Gav:
                    valueData << it.value().at(ip).mGav;
                    mPlotHarbours->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotHarbours->yAxis->setLabel(QObject::tr("GAV (Euro)"));
                    break;
                case H_Vpuf:
                    valueData << it.value().at(ip).mVpuf;
                    mPlotHarbours->xAxis->setLabel(QObject::tr("Time (h)"));
                    mPlotHarbours->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
                    break;
                }
            }
            ++it;
        }

        graph->setData(keyData, valueData);

        ++col_it;
    }

    mPlotHarbours->rescaleAxes();
    mPlotHarbours->replot();    
}

void StatsController::updateMetiersStats(DisplaceModel *model)
{
    static const QPen pen(QColor(0,0,255,200));
    mPlotMetiers->clearGraphs();

    auto &dl = model->getMetiersList();

    double t = model->getCurrentStep();
    mMetTimeLine->start->setCoords(t, timelineMin);
    mMetTimeLine->end->setCoords(t, timelineMax);

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();

    switch (mSelectedMetiersStat) {
    case M_Catches:
        mPlotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlotMetiers->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case M_Revenues:
        mPlotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlotMetiers->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case M_Gav:
        mPlotMetiers->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlotMetiers->yAxis->setLabel(QObject::tr("GAV (Euro)"));
        break;
    }

    foreach (auto  d, dl) {
        if (!d->ticked)
            continue;

        if (col_it == mPalette.end())
            col_it = mPalette.begin();

        QVector<double> keyData;
        QVector<double> valueData;

        QCPGraph *graph = mPlotMetiers->addGraph();
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

                switch (mSelectedMetiersStat) {
                case M_Catches:
                    if (d->populationId == -1)
                        valueData << it.value().at(ip).mTotCatches;
                    else
                        valueData << it.value().at(ip).mCatchesPerPop[d->populationId];
                    break;
                case M_Revenues:
                    valueData << it.value().at(ip).revenueAV;
                    break;
                case M_Gav:
                    valueData << it.value().at(ip).gav;
                    break;
                }
            }
            ++it;
        }

        graph->setData(keyData, valueData);

        ++col_it;
    }

    mPlotMetiers->rescaleAxes();
    mPlotMetiers->replot();
}

