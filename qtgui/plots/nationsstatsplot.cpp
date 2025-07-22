#include "nationsstatsplot.h"

#include <displacemodel.h>
#include "stats/statsutils.h"
#include <storage/sqliteoutputstorage.h>
#include <storage/tables/vesselslogliketable.h>
#include <msqlitecpp/v1/sqlitestatement.h>

#include <qcustomplot.h>


#include <QPen>

using namespace displace::plot;

NationsStatsPlot::NationsStatsPlot(QCustomPlot *plot_, QCPItemLine *timeline_)
    : plotNations(plot_), timeline(timeline_)
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void NationsStatsPlot::update(DisplaceModel *model, displace::plot::NationsStat stat, QCustomPlot *theplot)
{
    if (theplot != nullptr) {
        // do not cache
        update(theplot, stat);
    } else {
        if (model != lastModel || stat != lastStat) {
            // need to properly update
            lastModel = model;
            lastStat = stat;
            invalidate();
        }
        if (isVisible())
            update(plotNations, stat);
    }
}

void NationsStatsPlot::update(QCustomPlot *plot, displace::plot::NationsStat stat)
{
    auto model = lastModel;

    qDebug() << "NationsStatPlot UPDATE";

    auto db = model->getOutputStorage();
    if (db == nullptr)
        return;

    if (plot == nullptr)
        plot = plotNations;

    static const QPen pen(QColor(0,0,255,200));
    plot->clearGraphs();

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

        QCPGraph *graph = plot->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QString nationName = model->getNation(ip).getName();
        // BE,DE,DK,ES,FR,GB,IE,NL,NO,PL,PT etc.
        if(nationName.toStdString()=="ITA")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/italy-flag-round-icon-16.png")));
        if(nationName.toStdString()=="HRV")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/croatia-flag-round-icon-16.png")));
        if(nationName.toStdString()=="BE" || nationName.toStdString()=="BEL")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/belgium-flag-round-icon-16.png")));
        if(nationName.toStdString()=="DE" || nationName.toStdString()=="DEU")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/germany-flag-round-icon-16.png")));
        if(nationName.toStdString()=="ES" || nationName.toStdString()=="ESP" || nationName.toStdString()=="SPN")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/spain-flag-round-icon-16.png")));
        if(nationName.toStdString()=="FR" || nationName.toStdString()=="FRA")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/france-flag-round-icon-16.png")));
        if(nationName.toStdString()=="GB" || nationName.toStdString()=="GBR" || nationName.toStdString()=="UK")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/united-kingdom-flag-round-icon-16.png")));
        if(nationName.toStdString()=="IE" || nationName.toStdString()=="IRL" ||  nationName.toStdString()=="IRE")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/ireland-flag-round-icon-16.png")));
        if(nationName.toStdString()=="ND" || nationName.toStdString()=="NLD")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/netherlands-flag-round-icon-16.png")));
        if(nationName.toStdString()=="NO" || nationName.toStdString()=="NOR")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/norway-flag-round-icon-16.png")));
        if(nationName.toStdString()=="PL" || nationName.toStdString()=="POL")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/poland-flag-round-icon-16.png")));
        if(nationName.toStdString()=="PT" || nationName.toStdString()=="PRT" || nationName.toStdString()=="POR")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/portugal-flag-round-icon-16.png")));
        if(nationName.toStdString()=="RO" || nationName.toStdString()=="ROM")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/romania-flag-round-icon-16.png")));
        if(nationName.toStdString()=="FI" || nationName.toStdString()=="FIN")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/finland-flag-round-icon-16.png")));
        if(nationName.toStdString()=="EE" || nationName.toStdString()=="EST")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/estonia-flag-round-icon-16.png")));
        if(nationName.toStdString()=="LTU")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/lithuania-flag-round-icon-16.png")));
        if(nationName.toStdString()=="LVA")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/latvia-flag-round-icon-16.png")));
        if(nationName.toStdString()=="DNK" || nationName.toStdString()=="DEN")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/denmark-flag-round-icon-16.png")));
        if(nationName.toStdString()=="SWE")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/sweden-flag-round-icon-16.png")));
        if(nationName.toStdString()=="BGR")
             graph->setScatterStyle(QCPScatterStyle(QPixmap(":/icons/bulgaria-flag-round-icon-16.png")));

        QColor col = col_it != mPalette.end() ? *col_it : QColor();

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(nationName);

        auto v = getData(model, stat, ip);
        graph->setData(std::get<0>(v), std::get<1>(v));

        ++col_it;
    }

    switch (stat) {
    case NationsStat::Catches:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Landings (kg)"));
        break;
    case NationsStat::Discards:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Discards (kg)"));
        break;
    case NationsStat::Earnings:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case NationsStat::ExEarnings:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue (Euro)"));
        break;
    case NationsStat::TimeAtSea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Time at sea (h)"));
        break;
    case NationsStat::Gav:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GVA (Euro)"));
        break;
    case NationsStat::Vpuf:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("VPUF (Euro per litre)"));
        break;
    case NationsStat::SweptArea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Swept Area (m^2)"));
        break;
    case NationsStat::RevenuePerSweptArea:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Revenue Per Swept Area (Euro/km^2)"));
        break;
    case NationsStat::GVA:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::GVAPerRevenue:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("GVA to Revenue Ratio"));
        break;
    case NationsStat::LabourSurplus:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::GrossProfit:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::NetProfit:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::NetProfitMargin:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("%"));
        break;
    case NationsStat::GVAPerFTE:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::RoFTA:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("%"));
        break;
    case NationsStat::BER:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::CRBER:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Ratio"));
        break;
    case NationsStat::NetPresentValue:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Euro"));
        break;
    case NationsStat::numTrips:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("#"));
        break;
    }
    plot->rescaleAxes();
    plot->replot();
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
    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    TimelineData dt;

    using NS = displace::plot::NationsStat;
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
        dt = db->getVesselLoglikeDataByNation(stat, model->getNation(nation).getName().toStdString(),
                                              SQLiteOutputStorage::Operation::Sum);
        stats::runningSum(dt.v);
        break;
    case NS::numTrips:
        dt = db->getVesselLoglikeDataByNation(stat, model->getNation(nation).getName().toStdString(),
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
        dt = db->getVesselLoglikeDataByNation(stat, model->getNation(nation).getName().toStdString(),
                                              SQLiteOutputStorage::Operation::Average);
        stats::runningAvg(dt.v);
        break;
    }

    QVector<double> kd(dt.t.begin(), dt.t.end()), vd(dt.v.begin(), dt.v.end());
    return std::make_tuple(kd, vd);
}

void NationsStatsPlot::doUpdate()
{
    update(plotNations, lastStat);
}
