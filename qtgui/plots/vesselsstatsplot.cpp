#include "vesselsstatsplot.h"

#include <displacemodel.h>
#include "stats/statsutils.h"
#include <storage/sqliteoutputstorage.h>
#include <storage/tables/vesselslogliketable.h>
#include <sqlitestatement.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

VesselsStatsPlot::VesselsStatsPlot(QCustomPlot *plotVessels, QCPItemLine *timeline_)
    : plotVessels(plotVessels), timeline(timeline_)
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void VesselsStatsPlot::update(DisplaceModel *model, displace::plot::VesselsStat stat, QCustomPlot *theplot)
{
    if (theplot != nullptr) {
        // do not cache
        update(theplot);
    } else {
        if (model != lastModel || stat != lastStat) {
            // need to properly update
            lastModel = model;
            lastStat = stat;
            invalidate();
        }
        if (isVisible())
            update(nullptr);
    }
}

void VesselsStatsPlot::update(QCustomPlot *plot)
{
    auto model = lastModel;
    auto stat = lastStat;

    qDebug() << "VesselsStatPlot UPDATE";

    auto db = model->getOutputStorage();
    if (db == nullptr)
        return;

    if (plot == nullptr)
        plot = plotVessels;

    static const QPen pen(QColor(0,0,255,200));
    plot->clearGraphs();

    QList<int> ipl = model->getInterestingVessels();

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

        QCPGraph *graph = plot->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        auto name = model->getVesselId(ip);
        graph->setName(name);

        auto v = getData(model, stat, name.toStdString());
        graph->setData(std::get<0>(v), std::get<1>(v));

        ++col_it;
    }

    switch (stat) {
    case VesselsStat::Catches:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case VesselsStat::Discards:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Discards (kg)"));
        break;
    case VesselsStat::Earnings:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case VesselsStat::ExEarnings:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case VesselsStat::TimeAtSea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Time at sea (h)"));
        break;
    case VesselsStat::Gav:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GVA (Euro)"));
        break;
    case VesselsStat::Vpuf:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
        break;
    case VesselsStat::SweptArea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
        break;
    case VesselsStat::RevenuePerSweptArea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue Per Swept Area (Euro/metre^2)"));
        break;
    case VesselsStat::GVA:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case VesselsStat::GVAPerRevenue:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
        break;
    case VesselsStat::LabourSurplus:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case VesselsStat::GrossProfit:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case VesselsStat::NetProfit:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case VesselsStat::NetProfitMargin:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("%"));
        break;
    case VesselsStat::GVAPerFTE:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case VesselsStat::RoFTA:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("%"));
        break;
    case VesselsStat::BER:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case VesselsStat::CRBER:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Ratio"));
        break;
    case VesselsStat::NetPresentValue:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case VesselsStat::numTrips:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("#"));
        break;
    }

    plot->rescaleAxes();
    plot->replot();
}

void VesselsStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&VesselsStatsPlot::saveTo, this));
    }
}

void VesselsStatsPlot::saveTo()
{
    if (!lastModel)
        return;

    auto db = lastModel->getOutputStorage();
    if (db == nullptr)
        return;

    QString fn = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save plot data"), QString(), QObject::tr("Csv file (*.csv)"));
    if (!fn.isEmpty()) {
        QList<int> ipl = lastModel->getInterestingVessels();

        QFile file(fn);
        if (!file.open(QIODevice::ReadWrite)) {
            QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Cannot save to %1: %2").arg(fn).arg(file.errorString()));
            return;
        }

        QTextStream strm(&file);

        for (auto ip : ipl) {
            auto name = lastModel->getVesselId(ip);
            strm << name << "\n";

            auto v = getData(lastModel, lastStat, name.toStdString());
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

std::tuple<QVector<double>, QVector<double> > VesselsStatsPlot::getData(DisplaceModel *model,
                                                                        displace::plot::VesselsStat stat,
                                                                        string vesselId)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    TimelineData dt;

    using NS = displace::plot::VesselsStat;
    switch (stat) {
    case NS::Catches:
    case NS::Discards:
    case NS::Earnings:
    case NS::ExEarnings:
    case NS::TimeAtSea:
    case NS::Gav:
    case NS::SweptArea:
    case NS::GVA:
    case NS::LabourSurplus:
    case NS::GrossProfit:
    case NS::NetProfit:
    case NS::NetPresentValue:
        dt = db->getVesselLoglikeDataByVessel(stat, vesselId,
                                              SQLiteOutputStorage::Operation::Sum);
        stats::runningSum(dt.v);
        break;
    case NS::numTrips:
        dt = db->getVesselLoglikeDataByVessel(stat, vesselId,
                                              SQLiteOutputStorage::Operation::Count);
        stats::runningSum(dt.v);
        break;
    case NS::Vpuf:
    case NS::RevenuePerSweptArea:
    case NS::GVAPerRevenue:
    case NS::NetProfitMargin:
    case NS::GVAPerFTE:
    case NS::RoFTA:
    case NS::BER:
    case NS::CRBER:
        dt = db->getVesselLoglikeDataByVessel(stat, vesselId,
                                              SQLiteOutputStorage::Operation::Average);
        stats::runningAvg(dt.v);
        break;
    }

    QVector<double> kd = QVector<double>::fromStdVector(dt.t), vd = QVector<double>::fromStdVector(dt.v);
    return std::make_tuple(kd, vd);
}

void VesselsStatsPlot::doUpdate()
{
    update(nullptr);
}
