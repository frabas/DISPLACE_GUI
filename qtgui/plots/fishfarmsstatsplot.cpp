#include "fishfarmsstatsplot.h"
#include <storage/sqliteoutputstorage.h>
#include <displacemodel.h>
#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

FishfarmsStatsPlot::FishfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeline)
    : mPlot(plot),
      mTimeline(timeline),
      pen(QColor(0,0,255,200))
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void FishfarmsStatsPlot::update(DisplaceModel *model, displace::plot::FishfarmsStat stat, QCustomPlot *theplot)
{
    checkUpdate(theplot,
    [&, model, stat]() {
        return (model != lastModel || stat != lastStat);
    },
    [&, model, stat]() {
        lastModel = model;
        lastStat = stat;
    });
}

void FishfarmsStatsPlot::update(QCustomPlot *plot)
{
    try {
        if (plot == nullptr)
            plot = mPlot;

        qDebug() << "Fishfarm plot UPDATE";
        plot->clearGraphs();
        displayPlot(lastModel, lastStat);
    } catch (std::exception &x ) {
        // add the text label at the top:
        QCPItemText *textLabel = new QCPItemText(mPlot);
        textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
        textLabel->position->setCoords(0.5, 0.5); // place position at center/top of axis rect
        textLabel->setText(QString("Error: %1").arg(x.what()));
        textLabel->setFont(QFont(mPlot->font().family(), 16)); // make font a bit larger
        textLabel->setPen(QPen(Qt::black)); // show black border around text
    }
}

void FishfarmsStatsPlot::displayPlot(DisplaceModel *model, displace::plot::FishfarmsStat stat)
{
    double val;

    QList<int>  interFishfarmsIDsList= model->getInterestingFishfarms();

    auto farmsTypeGroups = model->getFishfarmsTypesList();
    QList<int> interFishfarmsTypesList = farmsTypeGroups->list();

    QList<int> graphList;
    bool showtotal = farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    /* If no farm type is selected, but aggregate is selected, select all farms */
    if (interFishfarmsTypesList.size() != 0) {
        for(int i = 0; i < model->getNumFishfarmsTypes(); ++i) {
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
        interFishfarmsTypesList.push_back(999);
        if (showmax)
            graphList.push_back(4999);
        if (showmin)
            graphList.push_back(3999);
        if (showavg)
            graphList.push_back(2999);
        if (showtotal)
            graphList.push_back(1999);
    }

    /* If no fishfarms is selected, select all fishfarms type */
    if (interFishfarmsIDsList.size() == 0) {
        for (int i = 0; i < model->getFishfarmsCount(); ++i) {
            interFishfarmsIDsList.push_back(i+1);
        }
    }

    std::vector<int> ftypes;
    for (auto i : interFishfarmsTypesList)
        ftypes.push_back(i);

    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;
    QList<QVector<double> >keyData;
    QList<QVector<double> >valueData;

    double t = model->getCurrentStep();
    if (mTimeline != nullptr) {
        mTimeline->start->setCoords(t, timelineMin);
        mTimeline->end->setCoords(t, timelineMax);
    }

    for (int igraph = 0; igraph < graphNum; ++igraph) {
        // Creates graph. Index in list are: ip * nsz + isz
        QCPGraph *graph = mPlot->addGraph();
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

        AggregationType aggtype = AggregationType::None;

        switch (graphList[igraph] / 1000) {
        case 4:
            graph->setName(QString(QObject::tr("farm id %1 Max")).arg(group));
                aggtype = AggregationType::Max;
            break;
        case 3:
            graph->setName(QString(QObject::tr("farm id %1 Min")).arg(group));
                aggtype = AggregationType::Min;
            break;
        case 2:
            graph->setName(QString(QObject::tr("farm id %1 Avg")).arg(group));
            aggtype = AggregationType::Avg;
            break;
        case 1:
            graph->setName(QString(QObject::tr("farm id %1 Total")).arg(group));
            aggtype = AggregationType::Sum;
            break;
        }

        auto v = getData(model, stat, aggtype, grp, ftypes);
        graph->setData(std::get<0>(v), std::get<1>(v));
        graphs.push_back(graph);
    }

    switch (stat) {
    case FishfarmsStat::FF_FishMeanWeight:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Fish mean weight (kg)"));
        break;
    case FishfarmsStat::FF_FishHarvestedKg:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Fish Harvested (kg)"));
        break;
    case FishfarmsStat::FF_EggsHarvestedKg:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Eggs Harvested (kg)"));
        break;
    case FishfarmsStat::FF_AnnualProfit:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Annual Profit (euro)"));
        break;
    case FishfarmsStat::FF_NetDischargeN:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Net discharge N (kg)"));
        break;
    case FishfarmsStat::FF_NetDischargeP:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Net discharge P (kg)"));
        break;
    case FishfarmsStat::FF_CumulNetDischargeN:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Acc. Net discharge N (kg)"));
        break;
    case FishfarmsStat::FF_CumulNetDischargeP:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Acc. Net discharge P (kg)"));
        break;
    }

    mPlot->rescaleAxes();
    mPlot->replot();
}

void FishfarmsStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&FishfarmsStatsPlot::saveTo, this));
    }
}

void FishfarmsStatsPlot::saveTo()
{
    mSaveFilename = "fishfarms.txt";
}

double FishfarmsStatsPlot::getStatValue(DisplaceModel *model, int tstep, int farmid, int farmtype, displace::plot::FishfarmsStat stattype)
{
    switch (stattype) {
    case FishfarmsStat::FF_FishMeanWeight:
        return model->getFishfarmsStatistics().getValue(tstep).meanwForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_FishHarvestedKg:
        return model->getFishfarmsStatistics().getValue(tstep).fishharvestedkgForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_EggsHarvestedKg:
        return model->getFishfarmsStatistics().getValue(tstep).eggsharvestedkgForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_AnnualProfit:
        return model->getFishfarmsStatistics().getValue(tstep).annualprofitForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_NetDischargeN:
        return model->getFishfarmsStatistics().getValue(tstep).netdischargeNForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_NetDischargeP:
        return model->getFishfarmsStatistics().getValue(tstep).netdischargePForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_CumulNetDischargeN:
        return model->getFishfarmsStatistics().getValue(tstep).cumulnetdischargeNForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_CumulNetDischargeP:
        return model->getFishfarmsStatistics().getValue(tstep).cumulnetdischargePForFishfarmAndFarmGroup(farmid, farmtype);
    }

    return 0;
}

std::tuple<QVector<double>, QVector<double> > FishfarmsStatsPlot::getData(DisplaceModel *model,
                                                                           displace::plot::FishfarmsStat stattype,
                                                                           displace::plot::AggregationType aggtype,
                                                                           int ffid, vector<int> fftypes)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    auto dt = db->getFishFarmStatData(stattype, aggtype, ffid, fftypes);

    QVector<double> kd = QVector<double>::fromStdVector(dt.t), vd = QVector<double>::fromStdVector(dt.v);
    return std::make_tuple(kd, vd);
}

