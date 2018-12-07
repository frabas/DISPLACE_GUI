#include "populationsstatplot.h"

#include "stats/statsutils.h"

#include <displacemodel.h>
#include <storage/sqliteoutputstorage.h>
#include <storage/tables/vesselslogliketable.h>
#include <sqlitestatement.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

PopulationsStatPlot::PopulationsStatPlot(QCustomPlot *plt)
{
    plot = plt;
    timeline = new QCPItemLine(plot);

    mPalette = PaletteManager::instance()->palette(PopulationRole);
}

void PopulationsStatPlot::update(DisplaceModel *model, displace::plot::PopulationStat stat, QCustomPlot *theplot)
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

void PopulationsStatPlot::doUpdate()
{
    update(nullptr);
}

void PopulationsStatPlot::update(QCustomPlot *theplot)
{
    qDebug() << "PopulationsStatPlot UPDATE";

    if (!theplot) {
        theplot = plot;
    }
    theplot->clearGraphs();

    auto model = lastModel;
    auto stat = lastStat;

    QList<int> interPopList = model->getInterestingPops();
    QList<int> interSizeList = model->getInterestingSizes();
    QList<int> graphList;
    bool showtotal = model->isInterestingSizeTotal();
    bool showavg =  model->isInterestingSizeAvg();
    bool showmin =  model->isInterestingSizeMin();
    bool showmax =  model->isInterestingSizeMax();

    if (showmax)
        graphList.push_front(-4);
    if (showmin)
        graphList.push_front(-3);
    if (showavg)
        graphList.push_front(-2);
    if (showtotal)
        graphList.push_front(-1);

    if (graphList.size() == 0)
        graphList.append(interSizeList);

    /* If no size is selected, but aggregate is selected, select all sizes */
    if (interSizeList.size() == 0 && graphList.size() != 0) {
        for(int i = 0; i < model->getSzGrupsCount(); ++i)
            interSizeList.push_back(i);
    }

    std::vector<int> stype;
    for (auto i : interSizeList)
        stype.push_back(i);

    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;

    double t = model->getCurrentStep();
    if (timeline != nullptr) {
        timeline->start->setCoords(t, timelineMin);
        timeline->end->setCoords(t, timelineMax);
    }

    foreach (int ipop, interPopList) {
        for (int igraph = 0; igraph < graphNum; ++igraph) {
            // Creates graph. Index in list are: ip * nsz + isz
            QCPGraph *graph = theplot->addGraph();
            QColor col = mPalette.colorByIndex(ipop);

            graph->setLineStyle(QCPGraph::lsLine);
            graph->setPen(QPen(QBrush(col),2));

            col.setAlpha(128);
            graph->setBrush(QBrush(col));

            //QString popName = model->getPop(ipop).getName(); //NOT IMPLEMENTED, BUT WOULD BE USEFUL TO INJECT NAMES INTO GRAPH LEGEND
            AggregationType aggtype = AggregationType::None;

            switch (graphList[igraph]) {
            case -4:
                graph->setName(QString(QObject::tr("Pop %1 Max")).arg(ipop));
                aggtype = AggregationType::Max;
                break;
            case -3:
                graph->setName(QString(QObject::tr("Pop %1 Min")).arg(ipop));
                aggtype = AggregationType::Min;
                break;
            case -2:
                graph->setName(QString(QObject::tr("Pop %1 Avg")).arg(ipop));
                aggtype = AggregationType::Avg;
                break;
            case -1:
                graph->setName(QString(QObject::tr("Pop %1 Total")).arg(ipop));
                aggtype = AggregationType::Sum;
                break;
            default:
                graph->setName(QString(QObject::tr("Pop %1 Group %2")).arg(ipop).arg(graphList[igraph]+1));
                aggtype = AggregationType::None;
                break;
            }

            auto v = getData(model, stat, aggtype, ipop, stype);
            if(stat==PopulationStat::FvsEffort)
               {
                v = getData(model, (displace::plot::PopulationStat) 2, (displace::plot::NationsStat) 5, aggtype, ipop, stype); // see 1 is F and 4 is "TimeAtSea" in plottype.h
               }

            graph->setData(std::get<0>(v), std::get<1>(v));
            graphs.push_back(graph);
        }
    }

    switch (stat) {
    case PopulationStat::Aggregate:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Numbers ('000)"));
        break;
    case PopulationStat::Catches:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("kg"));
        break;
    case PopulationStat::Mortality:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("F"));
        break;
    case PopulationStat::NatMortality:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("M"));
        break;
    case PopulationStat::SSB:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("SSB (kg)"));
        break;
    case PopulationStat::QuotasUptake:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Quota Uptake"));
        break;
    case PopulationStat::Quotas:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Quota (tons)"));
        break;
    case PopulationStat::FFmsy:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("F/Fmsy"));
        break;
    case PopulationStat::PropMature:
        theplot->xAxis->setLabel(QObject::tr("Time (h)"));
        theplot->yAxis->setLabel(QObject::tr("Proportion mature fish"));
        break;
    case PopulationStat::FvsEffort:
        theplot->yAxis->setLabel(QObject::tr("F"));
        theplot->xAxis->setLabel(QObject::tr("Effort (h)"));
        break;
    }


    theplot->rescaleAxes();
    theplot->replot();
}

void PopulationsStatPlot::setCurrentTimeStep(double t)
{
    timeline->start->setCoords(t, timelineMin);
    timeline->end->setCoords(t, timelineMax);
}

std::tuple<QVector<double>, QVector<double> > PopulationsStatPlot::getData(DisplaceModel *model,
                                                                           displace::plot::PopulationStat stattype,
                                                                           displace::plot::AggregationType aggtype,
                                                                           int popid, vector<int> szid)
{
    if(stattype==PopulationStat::FvsEffort) return  std::make_tuple(QVector<double > (0),  QVector<double > (0));


    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    auto dt = db->getPopulationStatData(stattype, aggtype, popid, szid);

    QVector<double> kd = QVector<double>::fromStdVector(dt.t), vd = QVector<double>::fromStdVector(dt.v);
    return std::make_tuple(kd, vd);
}

std::tuple<QVector<double>, QVector<double> > PopulationsStatPlot::getData(DisplaceModel *model,
                                                                           displace::plot::PopulationStat stattype,
                                                                           displace::plot::NationsStat stattype2,
                                                                           displace::plot::AggregationType aggtype,
                                                                           int popid, vector<int> szid)
{
   // a getData() version to cross to origin of data i.e. PopulationStat and NationsStat

    auto db = model->getOutputStorage();
    if (db == nullptr)
        return std::tuple<QVector<double>, QVector<double>>();

    // x
    int nation =0; // TO DO..
    auto dt1 = db->getVesselLoglikeDataByNation(stattype2, model->getNation(nation).getName().toStdString(),
                                                          SQLiteOutputStorage::Operation::Sum);
    stats::runningSum(dt1.v);   // TO DO.. PROBLEM HERE...WHAT IS PLOTTED on X IS NOT CLEAR....

    // y
    auto dt2 = db->getPopulationStatData(stattype, aggtype, popid, szid);

    QVector<double> vd1 = QVector<double>::fromStdVector(dt1.v), vd2 = QVector<double>::fromStdVector(dt2.v);
    return std::make_tuple(vd1, vd2);
}


void PopulationsStatPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        // TODO enable this when saveTo() will be implemented
        menu->addAction(QObject::tr("Save Data"), std::bind(&PopulationsStatPlot::saveTo, this))->setEnabled(false);
    }
}

void PopulationsStatPlot::saveTo()
{
    // TODO enable saveTo(), disabled because query has changed
#if 0
    if (!lastModel)
        return;

    auto db = lastModel->getOutputStorage();
    if (db == nullptr)
        return;

    QString fn = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save plot data"), QString(), QObject::tr("Csv file (*.csv)"));
    if (!fn.isEmpty()) {
        QFile file(fn);
        if (!file.open(QIODevice::ReadWrite)) {
            QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Cannot save to %1: %2").arg(fn).arg(file.errorString()));
            return;
        }

        QTextStream strm(&file);


        QList<int> interPopList = lastModel->getInterestingPops();
        QList<int> interSizeList = lastModel->getInterestingSizes();

        for (auto ip : interPopList) {
            for (auto is : interSizeList) {
                AggregationType aggtype = AggregationType::None;

                strm << "Pop," << ip << ",Size," << is << "\n";

                auto v = getData(lastModel, lastStat, aggtype, ip, is);
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
#endif
}
