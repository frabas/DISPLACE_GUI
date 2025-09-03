#include "shipsstatsplot.h"


#include <displacemodel.h>
#include "storage/sqliteoutputstorage.h"
#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

ShipsStatsPlot::ShipsStatsPlot(QCustomPlot *plot, QCPItemLine *timeline)
    : mPlot(plot),
      mTimeline(timeline),
      pen(QColor(0,0,255,200))
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void ShipsStatsPlot::update(DisplaceModel *model, displace::plot::ShipsStat stat, QCustomPlot *plot)
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

void ShipsStatsPlot::update(QCustomPlot *plot)
{
    qDebug() << "ShipsStat UPDATE";
    if (plot == nullptr)
        plot = mPlot;
    
    plot->clearGraphs();

    QList<int>  interShipsIDsList= lastModel->getInterestingShips();

    auto shipsTypeGroups = lastModel->getShipsTypesList();
    QList<int> interShipsTypesList = shipsTypeGroups->list();

    std::vector<int> shptype;
    for (auto i : interShipsIDsList) {
        shptype.push_back(lastModel->getShipsTypesList()->list()[i]);
    }

    QList<int> graphList;
    bool showtotal = shipsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  shipsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  shipsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  shipsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    /* If no farm type is selected, but aggregate is selected, select all farms */
    if (interShipsTypesList.size() != 0) {
        for(int i = 0; i < lastModel->getNumShipsTypes(); ++i) {
            if (!shipsTypeGroups->has(i))
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
        interShipsTypesList.push_back(999);
        if (showmax)
            graphList.push_back(4999);
        if (showmin)
            graphList.push_back(3999);
        if (showavg)
            graphList.push_back(2999);
        if (showtotal)
            graphList.push_back(1999);
    }

    /* If no Ships is selected, select all Ships type */
    if (interShipsIDsList.size() == 0) {
        for (int i = 0; i < lastModel->getShipsCount(); ++i) {
            interShipsIDsList.push_back(i+1);
        }
    }

    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;
    QList<QVector<double> >keyData;
    QList<QVector<double> >valueData;

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

        AggregationType aggtype = AggregationType::None;

        switch (graphList[igraph] / 1000) {
        case 4:
            graph->setName(QString(QObject::tr("ship id %1 Max")).arg(group));
            aggtype = AggregationType::Max;
            break;
        case 3:
            graph->setName(QString(QObject::tr("ship id %1 Min")).arg(group));
            aggtype = AggregationType::Min;
            break;
        case 2:
            graph->setName(QString(QObject::tr("ship id %1 Avg")).arg(group));
            aggtype = AggregationType::Avg;
            break;
        case 1:
            graph->setName(QString(QObject::tr("ship id %1 Total")).arg(group));
            aggtype = AggregationType::Sum;
            break;
        }

        auto v = getData(lastModel, lastStat,aggtype, grp, shptype);
        graph->setData(std::get<0>(v), std::get<1>(v));
        graphs.push_back(graph);
    }

    switch (lastStat) {
    case ShipsStat::SH_NbTransportedUnits:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Nb. Transported Units"));
        plot->rescaleAxes();
        plot->replot();
        break;
    case ShipsStat::SH_FuelPerHour:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Fuel Per Hour"));
        plot->rescaleAxes();
        plot->replot();
        break;
    case ShipsStat::SH_NOxEmission_gperkW:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("NOx Emission Factor (g per kW)"));
        plot->rescaleAxes();
        plot->replot();
        break;
    case ShipsStat::SH_SOxEmission_PercentPerFuelMass:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("SOx Emission Factor (g per kW)"));
        plot->rescaleAxes();   // funny enough it does not rescale right so that values are visible, unless a plot with setRange is selected before....
        plot->replot();
        break;
    case ShipsStat::SH_GHGEmission_gperkW:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GHG Emission Factor (g per kW)"));
        plot->rescaleAxes();
        plot->yAxis->setRange(0, 1);
        plot->replot();
        break;
    case ShipsStat::SH_PMEEmission_gperkW:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("PME Emission Factor (g per kW)"));
        plot->rescaleAxes();
        plot->yAxis->setRange(0, 1);
        plot->replot();
        break;
    case ShipsStat::SH_FuelUseLitre:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Fule Use (litre)"));
        plot->rescaleAxes();
         plot->replot();
        break;
    case ShipsStat::SH_NOxEmission:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("NOx Emission (g)"));
        plot->rescaleAxes();
        plot->replot();
        break;
    case ShipsStat::SH_SOxEmission:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("SOx Emission (g)"));
        plot->rescaleAxes();
        plot->replot();
        break;
    case ShipsStat::SH_GHGEmission:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GHG Emission (g)"));
        plot->rescaleAxes();
        plot->replot();
        break;
    case ShipsStat::SH_PMEEmission:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("PME Emission (g)"));
        plot->rescaleAxes();
        plot->replot();
        break;
    }


}

void ShipsStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&ShipsStatsPlot::saveTo, this));
    }
}

void ShipsStatsPlot::saveTo()
{
    mSaveFilename = "Ships.txt";
}

std::tuple<QVector<double>, QVector<double> > ShipsStatsPlot::getData(DisplaceModel *model, displace::plot::ShipsStat stattype, AggregationType aggtype, int shipid, std::vector<int> shiptypeid)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    TimelineData dt;
    dt = db->getShipsStatData(stattype, aggtype, shipid, shiptypeid);
    //stats::runningSum(dt.v);

    QVector<double> kd(dt.t.begin(), dt.t.end()), vd(dt.v.begin(), dt.v.end());
    return std::make_tuple(kd, vd);
}


#if 0
double ShipsStatsPlot::getStatValue(DisplaceModel *model, displace::plot::ShipsStat stattype, AggregationType aggtype, int shipid, std::vector<int> shiptypeid)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    TimelineData dt;

    db->getShipsStatData(tstep, shipid, shiptypeid), stattype;

    switch (stattype) {
    case ShipsStat::SH_NbTransportedUnits:
        return model->getShipsStatistics().getValue(tstep).NbTransportedUnitsForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_FuelPerHour:
        return model->getShipsStatistics().getValue(tstep).FuelPerHourForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_NOxEmission_gperkW:
        return model->getShipsStatistics().getValue(tstep).NOxEmissionFactorForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_SOxEmission_PercentPerFuelMass:
        return model->getShipsStatistics().getValue(tstep).SOxEmissionFactorForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_GHGEmission_gperkW:
        return model->getShipsStatistics().getValue(tstep).GHGEmissionFactorForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_PMEEmission_gperkW:
        return model->getShipsStatistics().getValue(tstep).PMEEmissionFactorForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_FuelUseLitre:
        return model->getShipsStatistics().getValue(tstep).FuelUseLitreForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_NOxEmission:
        return model->getShipsStatistics().getValue(tstep).NOxEmissionForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_SOxEmission:
        return model->getShipsStatistics().getValue(tstep).SOxEmissionForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_GHGEmission:
        return model->getShipsStatistics().getValue(tstep).GHGEmissionForShipAndShipGroup(shipid, shiptypeid);
    case ShipsStat::SH_PMEEmission:
        return model->getShipsStatistics().getValue(tstep).PMEEmissionForShipAndShipGroup(shipid, shiptypeid);
    }

    return 0;
}
#endif
