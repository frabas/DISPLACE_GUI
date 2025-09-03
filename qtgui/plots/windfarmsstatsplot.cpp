#include "windfarmsstatsplot.h"

#include <displacemodel.h>
#include <storage/sqliteoutputstorage.h>
#include <storage/tables/windfarmstable.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

WindfarmsStatsPlot::WindfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeline)
    : mPlot(plot),
      mTimeline(timeline),
      pen(QColor(0,0,255,200))
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void WindfarmsStatsPlot::update(DisplaceModel *model, displace::plot::WindfarmsStat stat, QCustomPlot *plot)
{
    checkUpdate(plot,
                [&, model, stat]() {
                    return (model != lastModel || stat != lastStat);
                },
                [&, model, stat]() {
                    lastModel = model;
                    lastStat = stat;
                });
}

void WindfarmsStatsPlot::update(QCustomPlot *plot)
{
    qDebug() << "WindfoarmsStatPlot UPDATE";

    if (plot == nullptr)
        plot = mPlot;

    plot->clearGraphs();

    auto db = lastModel->getOutputStorage();
    if (db == nullptr)
        return;

    auto table = db->getWindfarmTable();

    QList<int>  interWindfarmsIDsList= lastModel->getInterestingWindfarms();

    auto farmsTypeGroups = lastModel->getWindfarmsTypesList();
    QList<int> interWindfarmsTypesList = farmsTypeGroups->list();

    QList<int> graphList;
    bool showtotal = farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    /* If no farm type is selected, but aggregate is selected, select all farms */
    if (interWindfarmsTypesList.size() != 0) {
        for(int i = 0; i < lastModel->getNumWindfarmsTypes(); ++i) {
            if (!farmsTypeGroups->has(i))
                continue;

            if (showmax)
                graphList.push_back(4000 + i);
            if (showmin)
                graphList.push_back(3000 + i);
            if (showavg)
                graphList.push_back(2000 + i);
            if (showtotal)
                graphList.push_back(1000 + i);
        }
    } else {
        interWindfarmsTypesList.push_back(999);
        if (showmax)
            graphList.push_back(4999);
        if (showmin)
            graphList.push_back(3999);
        if (showavg)
            graphList.push_back(2999);
        if (showtotal)
            graphList.push_back(1999);
    }

    /* If no Windfarms is selected, select all Windfarms type */
    if (interWindfarmsIDsList.size() == 0) {
        for (int i = 0; i < lastModel->getWindmillCount(); ++i) {
            interWindfarmsIDsList.push_back(lastModel->getWindmillId(i).toInt());
        }
    }

    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;

    double t = lastModel->getCurrentStep();
    if (mTimeline != nullptr) {
        mTimeline->start->setCoords(t, timelineMin);
        mTimeline->end->setCoords(t, timelineMax);
    }

    for (int igraph = 0; igraph < graphNum; ++igraph) {
        // Creates graph. Index in list are: ip * nsz + isz
        QCPGraph *graph = plot->addGraph();
        QColor col = mPalette.colorByIndex(igraph);

        graph->setLineStyle(QCPGraph::lsLine);
        graph->setPen(QPen(QBrush(col),2));

        col.setAlpha(128);
        graph->setBrush(QBrush(col));

        QString group;
        auto grp = graphList[igraph] % 1000;
        if (grp == 999) {
            group = "[all]";
        } else {
            group = QString("Type %1").arg(grp);
        }

        WindfarmsTable::StatType statType = WindfarmsTable::StatType::Kwh;
        WindfarmsTable::Aggreg aggregType = WindfarmsTable::Aggreg::Sum;

        switch (lastStat) {
        case WindfarmsStat::WF_kWh:
            statType = WindfarmsTable::StatType::Kwh;
            break;
        case WindfarmsStat::WF_kWProduction:
            statType = WindfarmsTable::StatType::KwhProduction;
            break;
        }

        WindfarmsTable::StatData data;

        switch (graphList[igraph] / 1000) {
        case 4:
            graph->setName(QString(QObject::tr("farm id %1 Max")).arg(group));
            aggregType = WindfarmsTable::Aggreg::Max;
            break;
        case 3:
            graph->setName(QString(QObject::tr("farm id %1 Min")).arg(group));
            aggregType = WindfarmsTable::Aggreg::Min;
            break;
        case 2:
            graph->setName(QString(QObject::tr("farm id %1 Avg")).arg(group));
            aggregType = WindfarmsTable::Aggreg::Avg;
            break;
        case 1:
            graph->setName(QString(QObject::tr("farm id %1 Total")).arg(group));
            aggregType = WindfarmsTable::Aggreg::Sum;
            break;
        }

        // normal data
        if (grp == 999)
            data = table->getStatData(statType, aggregType);
        else
            data = table->getStatData(statType, aggregType, grp);

        graphs.push_back(graph);
        graphs[igraph]->setData(QVector<double>(data.t.begin(), data.t.end()), QVector<double>(data.v.begin(), data.v.end()));
    }

    switch (lastStat) {
    case WindfarmsStat::WF_kWh:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("kWh"));
        break;
    case WindfarmsStat::WF_kWProduction:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("kW Production"));
        break;
    }

    plot->rescaleAxes();
    plot->replot();
}

void WindfarmsStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&WindfarmsStatsPlot::saveTo, this));
    }
}

void WindfarmsStatsPlot::saveTo()
{
    mSaveFilename = "Windfarms.txt";
}

