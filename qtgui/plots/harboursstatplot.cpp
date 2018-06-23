#include "harboursstatplot.h"

#include <displacemodel.h>
#include "stats/statsutils.h"
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

void HarboursStatPlot::update()
{
    update(plot);
}

void HarboursStatPlot::doUpdate()
{
    update((QCustomPlot *)nullptr);
}

void HarboursStatPlot::update(DisplaceModel *model, QCustomPlot *theplot)
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
            doUpdate();
    }
}

void HarboursStatPlot::update(QCustomPlot *theplot)
{
    auto model = lastModel;
    auto stat = lastStat;

    qDebug() << "HarboursStatPlot UPDATE";

    auto db = model->getOutputStorage();
    if (db == nullptr)
        return;

    if (theplot == nullptr)
        theplot = plot;
    
    lastModel = model;

    static const QPen pen(QColor(0,0,255,200));
    theplot->clearGraphs();

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

        QCPGraph *graph = theplot->addGraph();
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
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case HarboursStat::H_Discards:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Discards (kg)"));
        break;
    case HarboursStat::H_Earnings:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case HarboursStat::H_Gav:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("GAV (Euro)"));
        break;
    case HarboursStat::H_Vpuf:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
        break;
    case HarboursStat::H_SweptArea:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Swept Area (km^2)"));
        break;
    case HarboursStat::H_RevenuePerSweptArea:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Revenue per Swept Area (Euro/km^2)"));
        break;
    case HarboursStat::H_GVA:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_GVAPerRevenue:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
        break;
    case HarboursStat::H_LabourSurplus:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_GrossProfit:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_NetProfit:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_NetProfitMargin:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("%"));
        break;
    case HarboursStat::H_GVAPerFTE:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_RoFTA:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("%"));
        break;
    case HarboursStat::H_BER:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_CRBER:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_NetPresentValue:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("euro"));
        break;
    case HarboursStat::H_numTrips:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("#"));
        break;
    }

    theplot->rescaleAxes();
    theplot->replot();
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
        return std::tuple<QVector<double>, QVector<double>>();

    TimelineData dt;

    using HS = displace::plot::HarboursStat;
    switch (stat) {
    case HS::H_Catches:
    case HS::H_Discards:
    case HS::H_Earnings:
    case HS::H_Gav:
    case HS::H_SweptArea:
    case HS::H_GVA:
    case HS::H_LabourSurplus:
    case HS::H_GrossProfit:
    case HS::H_NetProfit:
    case HS::H_NetPresentValue:
        dt = db->getVesselLoglikeDataByHarbour(stat, harbour,
                                               SQLiteOutputStorage::Operation::Sum);
        stats::runningSum(dt.v);
        break;
    case HS::H_numTrips:
        dt = db->getVesselLoglikeDataByHarbour(stat, harbour,
                                               SQLiteOutputStorage::Operation::Count);
        stats::runningSum(dt.v);
        break;
    case HS::H_Vpuf:
    case HS::H_RevenuePerSweptArea:
    case HS::H_GVAPerRevenue:
    case HS::H_NetProfitMargin:
    case HS::H_GVAPerFTE:
    case HS::H_RoFTA:
    case HS::H_BER:
    case HS::H_CRBER:
        dt = db->getVesselLoglikeDataByHarbour(stat, harbour,
                                               SQLiteOutputStorage::Operation::Average);
        stats::runningAvg(dt.v);
        break;
    }

    QVector<double> kd = QVector<double>::fromStdVector(dt.t), vd = QVector<double>::fromStdVector(dt.v);
    return std::make_tuple(kd, vd);
}
