#include "benthosstatsplot.h"

#include <displacemodel.h>
#include "storage/sqliteoutputstorage.h"

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

BenthosStatsPlot::BenthosStatsPlot(QCustomPlot *plot, QCPItemLine *timeline)
    : mPlot(plot),
      mTimeline(timeline),
      pen(QColor(0,0,255,200))
{
    mPalette = PaletteManager::instance()->palette(BenthosRole);
}

void BenthosStatsPlot::update(DisplaceModel *model, displace::plot::BenthosStat stat)
{
    mPlot->clearGraphs();

    QList<int>  interBenthosIDsList= model->getInterestingBenthos();

    auto benthosTypeGroups = model->getFunctionalGroupsList();
    QList<int> interBenthosTypesList = benthosTypeGroups->list();

    std::vector<int> btype;
    for (auto i : interBenthosIDsList) {
        btype.push_back(model->getBenthosList()[i]->getId());
    }

    QList<int> graphList;
    bool showtotal = benthosTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  benthosTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  benthosTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  benthosTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    /* If no farm type is selected, but aggregate is selected, select all farms */
    if (interBenthosTypesList.size() != 0) {
        for(int i = 0; i < model->getBenthosCount(); ++i) {
            if (!benthosTypeGroups->has(i))
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
        interBenthosTypesList.push_back(999);
        if (showmax)
            graphList.push_back(4999);
        if (showmin)
            graphList.push_back(3999);
        if (showavg)
            graphList.push_back(2999);
        if (showtotal)
            graphList.push_back(1999);
    }

    /* If no benthos is selected, select all benthos type */
    if (interBenthosIDsList.size() == 0) {
        for (int i = 0; i < model->getBenthosCount(); ++i) {
            interBenthosIDsList.push_back(i+1);
        }
    }

    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;

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

        AggregationType aggtype = AggregationType::None;

        QString group;
        auto grp = graphList[igraph] % 1000;
        if (grp == 999) {
            group = "[all]";
        } else {
            group = QString("Type %1").arg(grp);
        }

        switch (graphList[igraph] / 1000) {
        case 4:
            graph->setName(QString(QObject::tr("benthos id %1 Max")).arg(group));
            aggtype = AggregationType::Max;
            break;
        case 3:
            graph->setName(QString(QObject::tr("benthos id %1 Min")).arg(group));
            aggtype = AggregationType::Min;
            break;
        case 2:
            graph->setName(QString(QObject::tr("benthos id %1 Avg")).arg(group));
            aggtype = AggregationType::Avg;
            break;
        case 1:
            graph->setName(QString(QObject::tr("benthos id %1 Total")).arg(group));
            aggtype = AggregationType::Sum;
            break;
        }

        auto v = getData(model, stat, aggtype, grp, btype);
        graph->setData(std::get<0>(v), std::get<1>(v));
        graphs.push_back(graph);
    }

    switch (stat) { // stat is the index in the selecting combo box
    case BenthosStat::B_TotBiomass:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Biomass"));
        break;
    case BenthosStat::B_Number:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Number"));
        break;
    case BenthosStat::B_MeanWeight:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Mean Weight (gram/sq_m)"));
        break;
    case BenthosStat::B_TotBiomassOverK:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("B/K"));
        break;
    case BenthosStat::B_NumberOverK:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("N/K"));
        break;
    }

    mPlot->rescaleAxes();
    mPlot->replot();
}

void BenthosStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        // TODO add a menu action
        //menu->addAction(QObject::tr("Save Data"), std::bind(&BenthosStatsPlot::saveTo, this));
    }
}

std::tuple<QVector<double>, QVector<double> > BenthosStatsPlot::getData(DisplaceModel *model, BenthosStat stattype, AggregationType aggtype, int grpid, const std::vector<int> &btype)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    auto dt = db->getBenthosStatData(stattype, aggtype, grpid, btype);

    QVector<double> kd = QVector<double>::fromStdVector(dt.t), vd = QVector<double>::fromStdVector(dt.v);
    return std::make_tuple(kd, vd);
}
