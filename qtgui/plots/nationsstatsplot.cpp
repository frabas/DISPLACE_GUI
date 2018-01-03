#include "nationsstatsplot.h"

#include <displacemodel.h>
#include <storage/sqliteoutputstorage.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

NationsStatsPlot::NationsStatsPlot(QCustomPlot *plot_, QCPItemLine *timeline_)
    : plotNations(plot_), timeline(timeline_)
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void NationsStatsPlot::update(DisplaceModel *model, displace::plot::NationsStat stat)
{
    static const QPen pen(QColor(0,0,255,200));
    plotNations->clearGraphs();

    QList<int> ipl = model->getInterestingNations();

    double t = model->getCurrentStep();
    if (timeline != nullptr ) {
        timeline->start->setCoords(t, timelineMin);
        timeline->end->setCoords(t, timelineMax);
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

                switch (stat) {
                case NationsStat::Catches:
                    valueData << it.value().at(ip).mTotCatches;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Landings (kg)"));
                    break;
                case NationsStat::Discards:
                    valueData << it.value().at(ip).mTotDiscards;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Discards (kg)"));
                    break;
                case NationsStat::Earnings:
                    valueData << it.value().at(ip).mRevenues;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
                    break;
                case NationsStat::ExEarnings:
                    valueData << it.value().at(ip).mExRevenues;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
                    break;
                case NationsStat::TimeAtSea:
                    valueData << it.value().at(ip).mTimeAtSea;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Time at sea (h)"));
                    break;
                case NationsStat::Gav:
                    valueData << it.value().at(ip).mGav;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("GVA (Euro)"));
                    break;
                case NationsStat::Vpuf:
                    valueData << it.value().at(ip).mVpuf;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
                    break;
                case NationsStat::SweptArea:
                    valueData << it.value().at(ip).mSweptArea;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
                    break;
                case NationsStat::RevenuePerSweptArea:
                    valueData << it.value().at(ip).mRevenuePerSweptArea;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Revenue Per Swept Area (Euro/metre^2)"));
                    break;
                case NationsStat::GVA:
                    valueData << it.value().at(ip).GVA;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Euro"));
                    break;
                case NationsStat::GVAPerRevenue:
                    valueData << it.value().at(ip).GVAPerRevenue;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
                    break;
                case NationsStat::LabourSurplus:
                    valueData << it.value().at(ip).LabourSurplus;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Euro"));
                    break;
                case NationsStat::GrossProfit:
                    valueData << it.value().at(ip).GrossProfit;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Euro"));
                    break;
                case NationsStat::NetProfit:
                    valueData << it.value().at(ip).NetProfit;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Euro"));
                    break;
                case NationsStat::NetProfitMargin:
                    valueData << it.value().at(ip).NetProfitMargin;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("%"));
                    break;
                case NationsStat::GVAPerFTE:
                    valueData << it.value().at(ip).GVAPerFTE;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Euro"));
                    break;
                case NationsStat::RoFTA:
                    valueData << it.value().at(ip).RoFTA;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("%"));
                    break;
                case NationsStat::BER:
                    valueData << it.value().at(ip).BER;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Euro"));
                    break;
                case NationsStat::CRBER:
                    valueData << it.value().at(ip).CRBER;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Ratio"));
                    break;
                case NationsStat::NetPresentValue:
                    valueData << it.value().at(ip).NetPresentValue;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("Euro"));
                    break;
                case NationsStat::numTrips:
                    valueData << it.value().at(ip).numTrips;
                    plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
                    plotNations->yAxis->setLabel(QObject::tr("#"));
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

void NationsStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&NationsStatsPlot::saveTo, this));
    }
}

void NationsStatsPlot::saveTo()
{

}
