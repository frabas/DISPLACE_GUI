#include "fishfarmsstatsplot.h"
#include <storage/sqliteoutputstorage.h>
#include <displacemodel.h>
#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

FishfarmsStatsPlot::FishfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeline)
    : mPlot(plot),
      mTimeline(timeline),
      pen(QColor(0,0,255,200)),
      textLabel(new QCPItemText(mPlot))

{
    textLabel->setVisible(false);
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
        textLabel->setVisible(false);
        plot->clearGraphs();
        displayPlot(lastModel, lastStat);
    } catch (std::exception &x ) {
        textLabel->setVisible(true);
        // add the text label at the top:
        textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
        textLabel->position->setCoords(0.5, 0.5); // place position at center/top of axis rect
        textLabel->setText(QString("Error: %1").arg(x.what()));
        textLabel->setFont(QFont(mPlot->font().family(), 16)); // make font a bit larger
        textLabel->setPen(QPen(Qt::black)); // show black border around text
    }
    plot->rescaleAxes();
    plot->replot();
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
    if (interFishfarmsIDsList.size() != 0) {
        for(int i = 0; i < model->getFishfarmList().size(); ++i) {
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
        interFishfarmsIDsList.push_back(999);
        if (showmax)
            graphList.push_back(4999);
        if (showmin)
            graphList.push_back(3999);
        if (showavg)
            graphList.push_back(2999);
        if (showtotal)
            graphList.push_back(1999);
    }

    /* If no fishfarmsTypes is selected, select all fishfarms type */
    if (interFishfarmsTypesList.size() == 0) {
        for (int i = 0; i < model->getFishfarmsTypesList()->count(); ++i) {
            interFishfarmsTypesList.push_back(i+1);
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
    case FishfarmsStat::FF_NetDischargeC:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Net discharge C (kg)"));
        break;
    case FishfarmsStat::FF_CumulNetDischargeN:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Acc. Net discharge N (kg)"));
        break;
    case FishfarmsStat::FF_CumulNetDischargeP:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Acc. Net discharge P (kg)"));
        break;
    case FishfarmsStat::FF_CumulNetDischargeC:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Acc. Net discharge C (kg)"));
        break;
    }
}

void FishfarmsStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&FishfarmsStatsPlot::saveTo, this));
    }
}

void FishfarmsStatsPlot::saveTo()
{
    if (!lastModel)
        return;

    auto db = lastModel->getOutputStorage();
    if (db == nullptr)
        return;

    QString fn = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save plot data"), QString(), QObject::tr("Csv file (*.csv)"));
    if (!fn.isEmpty()) {
        QList<int> ipl = lastModel->getInterestingFishfarms();

        QFile file(fn);
        if (!file.open(QIODevice::ReadWrite)) {
            QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Cannot save to %1: %2").arg(fn).arg(file.errorString()));
            return;
        }

        QTextStream strm(&file);

        auto farmsTypeGroups = lastModel->getFishfarmsTypesList();
        QList<int> interFishfarmsTypesList = farmsTypeGroups->list();
        std::vector<int> ftypes;
        for (auto i : interFishfarmsTypesList)
            ftypes.push_back(i);

        // TODO change this accordingly to type.
        AggregationType aggtype = AggregationType::None;

        for (auto ip : ipl) {
            auto name = lastModel->getFishfarmId(ip);
            strm << name << "\n";

            auto v = getData(lastModel, lastStat, aggtype, ip, ftypes);
            auto &k = std::get<0>(v);
            auto &d = std::get<1>(v);

            strm << "t";
            for (int i = 0; i < k.size(); ++i) {
                strm << "," << k[i];
            }
            strm << "\nv";
            for (int i = 0; i < d.size(); ++i) {
                strm << "," << d[i];
            }
            strm << "\n\n";
        }
    }
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

    QVector<double> kd(dt.t.begin(), dt.t.end()), vd(dt.v.begin(), dt.v.end());
    return std::make_tuple(kd, vd);
}

