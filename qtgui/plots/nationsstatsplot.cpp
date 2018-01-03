#include "nationsstatsplot.h"

#include <displacemodel.h>
#include <storage/sqliteoutputstorage.h>
#include <storage/tables/vesselslogliketable.h>
#include <sqlitestatement.h>

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
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return;

    lastModel = model;
    lastStat = stat;

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

        QCPGraph *graph = plotNations->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(QString(model->getNation(ip).getName()));

        auto v = getData(model, stat, ip);
        graph->setData(std::get<0>(v), std::get<1>(v));

        ++col_it;
    }

    switch (stat) {
    case NationsStat::Catches:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case NationsStat::Discards:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Discards (kg)"));
        break;
    case NationsStat::Earnings:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case NationsStat::ExEarnings:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case NationsStat::TimeAtSea:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Time at sea (h)"));
        break;
    case NationsStat::Gav:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("GVA (Euro)"));
        break;
    case NationsStat::Vpuf:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
        break;
    case NationsStat::SweptArea:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
        break;
    case NationsStat::RevenuePerSweptArea:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Revenue Per Swept Area (Euro/metre^2)"));
        break;
    case NationsStat::GVA:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::GVAPerRevenue:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
        break;
    case NationsStat::LabourSurplus:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::GrossProfit:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::NetProfit:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::NetProfitMargin:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("%"));
        break;
    case NationsStat::GVAPerFTE:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::RoFTA:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("%"));
        break;
    case NationsStat::BER:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::CRBER:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Ratio"));
        break;
    case NationsStat::NetPresentValue:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::numTrips:
        plotNations->xAxis->setLabel(QObject::tr("Time (h)"));
        plotNations->yAxis->setLabel(QObject::tr("#"));
        break;
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
    if (!lastModel)
        return;

    auto db = lastModel->getOutputStorage();
    if (db == nullptr)
        return;

    QString fn = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save plot data"), QString(), QObject::tr("Csv file (*.csv)"));
    if (!fn.isEmpty()) {
        QList<int> ipl = lastModel->getInterestingNations();

        QFile file(fn);
        if (!file.open(QIODevice::ReadWrite)) {
            QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Cannot save to %1: %2").arg(fn).arg(file.errorString()));
            return;
        }

        QTextStream strm(&file);

        for (auto ip : ipl) {
            strm << QString(lastModel->getNation(ip).getName()) << "\n";

            auto v = getData(lastModel, lastStat, ip);
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

std::tuple<QVector<double>, QVector<double> > NationsStatsPlot::getData(DisplaceModel *model, displace::plot::NationsStat stat, int nation)
{
    /*
  SELECT VesselLogLike.RowId,
       VesselLogLike.TStep,
       VesselLogLike.Id,
       VesselDef.VesselId,
       VesselDef.Nationality,
       SUM(VesselLogLikeCatches.Catches)
  FROM VesselLogLike
       JOIN
       VesselDef ON VesselLogLike.Id = VesselDef.Id
       JOIN
       VesselLogLikeCatches ON VesselLogLike.RowId = VesselLogLikeCatches.LoglikeId
 WHERE VesselDef.Nationality = "DNK"
 GROUP BY VesselLogLike.TStep
    */

    auto db = model->getOutputStorage();
    if (db == nullptr)
        throw std::runtime_error("null db");

    sqlite::SQLiteStatement stmt(db->getDb(), "SELECT "
                                 "VesselLogLike.TStep,"
                                 "SUM(VesselLogLikeCatches.Catches) "
                            "FROM VesselLogLike "
                                 "JOIN "
                                 "VesselDef ON VesselLogLike.Id = VesselDef.VesselId "
                                 "JOIN "
                                 "VesselLogLikeCatches ON VesselLogLike.RowId = VesselLogLikeCatches.LoglikeId "
                           "WHERE VesselDef.Nationality = ? "
                           "GROUP BY VesselLogLike.TStep");

    stmt.bind(std::make_tuple(model->getNation(nation).getName().toStdString()));

    std::vector<double> keyData;
    std::vector<double> valueData;

    stmt.execute([&stmt, &keyData, &valueData](){
        keyData.push_back(stmt.getIntValue(0));
        valueData.push_back(stmt.getDoubleValue(1));
        return true;
    });

    QVector<double> kd = QVector<double>::fromStdVector(keyData), vd = QVector<double>::fromStdVector(valueData);

#if 0
    int n = model->getNationsStatsCount();
    DisplaceModel::NationsStatsContainer::Container::const_iterator it = model->getNationsStatsFirstValue();
    for (int i = 0; i <n; ++i) {
        if (it.value().size() > nation) {
            keyData << it.key();

            switch (stat) {
            case NationsStat::Catches:
                valueData << it.value().at(nation).mTotCatches;
                break;
            case NationsStat::Discards:
                valueData << it.value().at(nation).mTotDiscards;
                break;
            case NationsStat::Earnings:
                valueData << it.value().at(nation).mRevenues;
                break;
            case NationsStat::ExEarnings:
                valueData << it.value().at(nation).mExRevenues;
                break;
            case NationsStat::TimeAtSea:
                valueData << it.value().at(nation).mTimeAtSea;
                break;
            case NationsStat::Gav:
                valueData << it.value().at(nation).mGav;
                break;
            case NationsStat::Vpuf:
                valueData << it.value().at(nation).mVpuf;
                break;
            case NationsStat::SweptArea:
                valueData << it.value().at(nation).mSweptArea;
                break;
            case NationsStat::RevenuePerSweptArea:
                valueData << it.value().at(nation).mRevenuePerSweptArea;
                break;
            case NationsStat::GVA:
                valueData << it.value().at(nation).GVA;
                break;
            case NationsStat::GVAPerRevenue:
                valueData << it.value().at(nation).GVAPerRevenue;
                break;
            case NationsStat::LabourSurplus:
                valueData << it.value().at(nation).LabourSurplus;
                break;
            case NationsStat::GrossProfit:
                valueData << it.value().at(nation).GrossProfit;
                break;
            case NationsStat::NetProfit:
                valueData << it.value().at(nation).NetProfit;
                break;
            case NationsStat::NetProfitMargin:
                valueData << it.value().at(nation).NetProfitMargin;
                break;
            case NationsStat::GVAPerFTE:
                valueData << it.value().at(nation).GVAPerFTE;
                break;
            case NationsStat::RoFTA:
                valueData << it.value().at(nation).RoFTA;
                break;
            case NationsStat::BER:
                valueData << it.value().at(nation).BER;
                break;
            case NationsStat::CRBER:
                valueData << it.value().at(nation).CRBER;
                break;
            case NationsStat::NetPresentValue:
                valueData << it.value().at(nation).NetPresentValue;
                break;
            case NationsStat::numTrips:
                valueData << it.value().at(nation).numTrips;
                break;
            }
        }
        ++it;
    }
#endif

    double rc = 0;
    // make running sum
    for (int i = 0; i < vd.size(); ++i) {
        rc += vd[i];
        vd[i] = rc;
    }

    return std::make_tuple(kd, vd);
}
