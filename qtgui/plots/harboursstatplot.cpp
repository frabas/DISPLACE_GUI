#include "harboursstatplot.h"

#include <displacemodel.h>
#include <storage/sqliteoutputstorage.h>
#include <storage/tables/vesselslogliketable.h>
#include <sqlitestatement.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

HarboursStatPlot::HarboursStatPlot(QCustomPlot *plot_)
    : plot(plot_)
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);

    timeline = new QCPItemLine(plot);
    plot->legend->setVisible(true);
}

void HarboursStatPlot::update(DisplaceModel *model)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return;

    lastModel = model;

    static const QPen pen(QColor(0,0,255,200));
    plot->clearGraphs();

    auto ipl = model->getInterestingHarbours();

    double t = model->getCurrentStep();
    if (timeline != nullptr ) {
        timeline->start->setCoords(t, timelineMin);
        timeline->end->setCoords(t, timelineMax);
    }

    int cnt = 0;
    Palette::Iterator col_it = mPalette.begin();
    foreach (auto ip, ipl) {
        if (col_it == mPalette.end())
            col_it = mPalette.begin();

        QCPGraph *graph = plot->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(QString::fromStdString(model->getHarbourData(ip.toIndex()).mHarbour->get_name()));

        int nodeid = model->getHarbourData(ip.toIndex()).mHarbour->get_idx_node().toIndex();

        auto v = getData(model, lastStat, nodeid);
        graph->setData(std::get<0>(v), std::get<1>(v));

        ++col_it;
    }

    switch (lastStat) {
    case HarboursStat::H_Catches:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case HarboursStat::H_Discards:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Discards (kg)"));
        break;
    case HarboursStat::H_Earnings:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case HarboursStat::H_Gav:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GAV (Euro)"));
        break;
    case HarboursStat::H_Vpuf:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
        break;
    case HarboursStat::H_SweptArea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
        break;
    case HarboursStat::H_RevenuePerSweptArea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue per Swept Area (Euro/km^2)"));
        break;
    case HarboursStat::H_GVA:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_GVAPerRevenue:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
        break;
    case HarboursStat::H_LabourSurplus:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_GrossProfit:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_NetProfit:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_NetProfitMargin:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("%"));
        break;
    case HarboursStat::H_GVAPerFTE:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_RoFTA:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("%"));
        break;
    case HarboursStat::H_BER:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_CRBER:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_NetPresentValue:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_numTrips:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("#"));
        break;
    }

    plot->rescaleAxes();
    plot->replot();
}

void HarboursStatPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&HarboursStatPlot::saveTo, this));
    }
}

void HarboursStatPlot::saveTo()
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

void HarboursStatPlot::setCurrentTimeStep(double t)
{
    timeline->start->setCoords(t, timelineMin);
    timeline->end->setCoords(t, timelineMax);
}

std::tuple<QVector<double>, QVector<double> > HarboursStatPlot::getData(DisplaceModel *model, displace::plot::HarboursStat stat, int harbour)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        throw std::runtime_error("null db");

    auto dt = db->getVesselLoglikeDataByHarbour(stat, harbour);

    QVector<double> kd = QVector<double>::fromStdVector(dt.t), vd = QVector<double>::fromStdVector(dt.v);

    double rc = 0;
    // make running sum
    for (int i = 0; i < vd.size(); ++i) {
        rc += vd[i];
        vd[i] = rc;
    }

    return std::make_tuple(kd, vd);
}
