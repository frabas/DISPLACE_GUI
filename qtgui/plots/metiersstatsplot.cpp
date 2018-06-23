#include "metiersstatsplot.h"

#include <displacemodel.h>
#include <stats/statsutils.h>
#include <storage/sqliteoutputstorage.h>
#include <storage/tables/vesselslogliketable.h>
#include <sqlitestatement.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

MetiersStatsPlot::MetiersStatsPlot(QCustomPlot *plot_)
    : plot(plot_)
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);

    timeline = new QCPItemLine(plot);
    plot->legend->setVisible(true);
}

void MetiersStatsPlot::update(DisplaceModel *model, QCustomPlot *theplot)
{
    if (theplot != nullptr) {
        // do not cache
        update(theplot);
    } else {
        if (model != lastModel) {
            // need to properly update
            lastModel = model;
            invalidate();
        }
        if (isVisible())
            update((QCustomPlot *)nullptr);
    }
}

void MetiersStatsPlot::update(QCustomPlot *theplot)
{
    auto model = lastModel;

    if (theplot == nullptr)
        theplot = plot;
    
    static const QPen pen(QColor(0,0,255,200));
    theplot->clearGraphs();

    auto &dl = model->getMetiersList();

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();

    switch (metStat) {
    case MetiersStat::M_Catches:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case MetiersStat::M_Discards:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Discards (kg)"));
        break;
    case MetiersStat::M_Revenues:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case MetiersStat::M_Gav:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("GAV (Euro)"));
        break;
    case MetiersStat::M_Vpuf:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("VPUF (Euro per Litre)"));
        break;
    case MetiersStat::M_SweptArea:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
        break;
    case MetiersStat::M_RevenuesPerSweptArea:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Revenue Per Swept Area (euro/m^2)"));
        break;
    case MetiersStat::M_GVA:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case MetiersStat::M_GVAPerRevenue:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
        break;
    case MetiersStat::M_LabourSurplus:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_GrossProfit:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_NetProfit:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_NetProfitMargin:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("%"));
        break;
    case MetiersStat::M_GVAPerFTE:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_RoFTA:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_BER:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_CRBER:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Ratio"));
        break;
    case MetiersStat::M_NetPresentValue:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_numTrips:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("#"));
        break;

    }

    foreach (auto  d, dl) {
        if (!d->ticked)
            continue;

        if (col_it == mPalette.end())
            col_it = mPalette.begin();

        QCPGraph *graph = theplot->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(d->description());

        auto v = getData(model, metStat,  d->metierId);
        graph->setData(std::get<0>(v), std::get<1>(v));

        ++col_it;
    }

    theplot->rescaleAxes();
    theplot->replot();
}

void MetiersStatsPlot::setCurrentTimeStep(double t)
{
    if (timeline != nullptr ) {
        timeline->start->setCoords(t, timelineMin);
        timeline->end->setCoords(t, timelineMax);
    }
}

std::tuple<QVector<double>, QVector<double> > MetiersStatsPlot::getData(DisplaceModel *model, MetiersStat stat, int metier)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    TimelineData dt;

    using MS = displace::plot::MetiersStat;
    switch (stat) {
    case MS::M_Catches:
    case MS::M_Discards:
    case MS::M_Revenues:
    case MS::M_Gav:
    case MS::M_SweptArea:
    case MS::M_GVA:
    case MS::M_LabourSurplus:
    case MS::M_GrossProfit:
    case MS::M_NetProfit:
    case MS::M_NetPresentValue:
        dt = db->getVesselLoglikeDataByMetier(stat, metier,
                                              SQLiteOutputStorage::Operation::Sum);
        stats::runningSum(dt.v);
        break;
    case MS::M_numTrips:
        dt = db->getVesselLoglikeDataByMetier(stat, metier,
                                              SQLiteOutputStorage::Operation::Count);
        stats::runningSum(dt.v);
        break;
    case MS::M_Vpuf:
    case MS::M_RevenuesPerSweptArea:
    case MS::M_GVAPerRevenue:
    case MS::M_NetProfitMargin:
    case MS::M_GVAPerFTE:
    case MS::M_RoFTA:
    case MS::M_BER:
    case MS::M_CRBER:
        dt = db->getVesselLoglikeDataByMetier(stat, metier,
                                              SQLiteOutputStorage::Operation::Average);
        stats::runningAvg(dt.v);
        break;
    }

    QVector<double> kd = QVector<double>::fromStdVector(dt.t), vd = QVector<double>::fromStdVector(dt.v);
    return std::make_tuple(kd, vd);
}

void MetiersStatsPlot::doUpdate()
{
    update((QCustomPlot *)nullptr);
}
