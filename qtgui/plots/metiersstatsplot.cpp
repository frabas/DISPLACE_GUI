#include "metiersstatsplot.h"

#include <displacemodel.h>
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

void MetiersStatsPlot::update(DisplaceModel *model)
{
    static const QPen pen(QColor(0,0,255,200));
    plot->clearGraphs();

    auto &dl = model->getMetiersList();

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();

    switch (metStat) {
    case MetiersStat::M_Catches:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case MetiersStat::M_Discards:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Discards (kg)"));
        break;
    case MetiersStat::M_Revenues:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case MetiersStat::M_Gav:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GAV (Euro)"));
        break;
    case MetiersStat::M_Vpuf:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("VPUF (Euro per Litre)"));
        break;
    case MetiersStat::M_SweptArea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
        break;
    case MetiersStat::M_RevenuesPerSweptArea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue Per Swept Area (euro/m^2)"));
        break;
    case MetiersStat::M_GVA:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case MetiersStat::M_GVAPerRevenue:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
        break;
    case MetiersStat::M_LabourSurplus:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_GrossProfit:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_NetProfit:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_NetProfitMargin:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("%"));
        break;
    case MetiersStat::M_GVAPerFTE:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_RoFTA:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_BER:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_CRBER:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Ratio"));
        break;
    case MetiersStat::M_NetPresentValue:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case MetiersStat::M_numTrips:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("#"));
        break;

    }

    foreach (auto  d, dl) {
        if (!d->ticked)
            continue;

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

        graph->setName(d->description());

        int n = model->getMetiersStatsCount();
        auto it = model->getMetiersStatsFirstValue();

        int ip = d->metierId;

        for (int i = 0; i <n; ++i) {
            if (it.value().size() > ip) {
                keyData << it.key();

                switch (metStat) {
                case MetiersStat::M_Catches:
                    if (d->populationId == -1)
                        valueData << it.value().at(ip).mTotCatches;
                    else {
                        if (it.value().at(ip).mCatchesPerPop.size() > d->populationId)
                            valueData << it.value().at(ip).mCatchesPerPop[d->populationId];
                        else
                            valueData << 0;
                    }
                    break;
                case MetiersStat::M_Discards:
                    if (d->populationId == -1)
                        valueData << it.value().at(ip).mTotDiscards;
                    else {
                        if (it.value().at(ip).mDiscardsPerPop.size() > d->populationId)
                            valueData << it.value().at(ip).mDiscardsPerPop[d->populationId];
                        else
                            valueData << 0;
                    }
                    break;
                case MetiersStat::M_Revenues:
                    valueData << it.value().at(ip).revenueAV;
                    break;
                case MetiersStat::M_Gav:
                    valueData << it.value().at(ip).gav;
                    break;
                case MetiersStat::M_Vpuf:
                    valueData << it.value().at(ip).vpuf;
                    break;
                case MetiersStat::M_SweptArea:
                    valueData << it.value().at(ip).mSweptArea;
                    break;
                case MetiersStat::M_RevenuesPerSweptArea:
                    valueData << it.value().at(ip).mRevenuePerSweptArea;
                    break;
                case MetiersStat::M_GVA:
                    valueData << it.value().at(ip).GVA;
                    break;
                case MetiersStat::M_GVAPerRevenue:
                    valueData << it.value().at(ip).GVAPerRevenue;
                    break;
                case MetiersStat::M_LabourSurplus:
                    valueData << it.value().at(ip).LabourSurplus;
                    break;
                case MetiersStat::M_GrossProfit:
                    valueData << it.value().at(ip).GrossProfit;
                    break;
                case MetiersStat::M_NetProfit:
                    valueData << it.value().at(ip).NetProfit;
                    break;
                case MetiersStat::M_NetProfitMargin:
                    valueData << it.value().at(ip).NetProfitMargin;
                    break;
                case MetiersStat::M_GVAPerFTE:
                    valueData << it.value().at(ip).GVAPerFTE;
                    break;
                case MetiersStat::M_RoFTA:
                    valueData << it.value().at(ip).RoFTA;
                    break;
                case MetiersStat::M_BER:
                    valueData << it.value().at(ip).BER;
                    break;
                case MetiersStat::M_CRBER:
                    valueData << it.value().at(ip).CRBER;
                    break;
                case MetiersStat::M_NetPresentValue:
                    valueData << it.value().at(ip).NetPresentValue;
                    break;
                case MetiersStat::M_numTrips:
                    valueData << it.value().at(ip).numTrips;
                    break;



                }
            }
            ++it;
        }

        graph->setData(keyData, valueData);

        ++col_it;
        plot->rescaleAxes();
        plot->replot();
    }
}

void MetiersStatsPlot::setCurrentTimeStep(double t)
{
    if (timeline != nullptr ) {
        timeline->start->setCoords(t, timelineMin);
        timeline->end->setCoords(t, timelineMax);
    }
}

