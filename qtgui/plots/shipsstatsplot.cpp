#include "shipsstatsplot.h"


#include <displacemodel.h>

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

void ShipsStatsPlot::update(DisplaceModel *model, displace::plot::ShipsStat stat)
{
    mPlot->clearGraphs();
    double val;

    QList<int>  interShipsIDsList= model->getInterestingShips();

    auto shipsTypeGroups = model->getShipsTypesList();
    QList<int> interShipsTypesList = shipsTypeGroups->list();

    QList<int> graphList;
    bool showtotal = shipsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  shipsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  shipsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  shipsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    /* If no farm type is selected, but aggregate is selected, select all farms */
    if (interShipsTypesList.size() != 0) {
        for(int i = 0; i < model->getNumShipsTypes(); ++i) {
            if (!shipsTypeGroups->has(i))
                continue;

            if (showmax)
                graphList.push_front(4000 + i);
            if (showmin)
                graphList.push_front(3000 + i);
            if (showavg)
                graphList.push_front(2000 + i);
            if (showtotal)
                graphList.push_front(1000 + i);
        }
    } else {
        interShipsTypesList.push_back(999);
        if (showmax)
            graphList.push_front(4999);
        if (showmin)
            graphList.push_front(3999);
        if (showavg)
            graphList.push_front(2999);
        if (showtotal)
            graphList.push_front(1999);
    }

    /* If no Ships is selected, select all Ships type */
    if (interShipsIDsList.size() == 0) {
        for (int i = 0; i < model->getShipsCount(); ++i) {
            interShipsIDsList.push_back(i+1);
        }
    }

    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;
    QList<QVector<double> >keyData;
    QList<QVector<double> >valueData;

    double t = model->getCurrentStep();
    if (mTimeline != nullptr) {
        mTimeline->start->setCoords(t, timelineMin);
        mTimeline->end->setCoords(t, timelineMax);
    }

    for (int igraph = 0; igraph < graphNum; ++igraph) {
        // Creates graph. Index in list are: ip * nsz + isz
        QCPGraph *graph = mPlot->addGraph();
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

        switch (graphList[igraph] / 1000) {
        case 4:
            graph->setName(QString(QObject::tr("ship id %1 Max")).arg(group));
            break;
        case 3:
            graph->setName(QString(QObject::tr("ship id %1 Min")).arg(group));
            break;
        case 2:
            graph->setName(QString(QObject::tr("ship id %1 Avg")).arg(group));
            break;
        case 1:
            graph->setName(QString(QObject::tr("ship id %1 Total")).arg(group));
            break;
        }

        graphs.push_back(graph);
        keyData.push_back(QVector<double>());
        valueData.push_back(QVector<double>());
    }

    int nsteps = model->getShipsStatistics().getUniqueValuesCount();

    //qDebug() << "**** Plotting " << nsteps << interShipsTypesList.size();
    auto it = model->getShipsStatistics().getFirst();
    for (int istep = 0; istep <nsteps; ++istep) {
        int nInterShipsIDs = interShipsIDsList.size();

        //qDebug() << "Step: " <<istep << it.key();
        for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
            //int gidx = iInterShipsIDs * graphNum + iGraph;
            int gidx = iGraph;

            auto group = graphList[iGraph] % 1000;

            //qDebug() << "Graph:" << iGraph << group;
             // calculate transversal values...
            double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0, nsam = 0;
            for (int iInterShipTypes = 0; iInterShipTypes < interShipsTypesList.size(); ++iInterShipTypes) {
                for (int iInterShipsIDs = 0; iInterShipsIDs < nInterShipsIDs; ++iInterShipsIDs) {

                    auto shtype = model->getShipList()[iInterShipsIDs]->mShip->get_typecode();
                    if (group != 999 && iInterShipTypes != shtype)
                        continue;

                    val = getStatValue(model, it.key(), interShipsIDsList[iInterShipsIDs], interShipsTypesList[iInterShipTypes], stat);

                    //qDebug() << iInterShipsIDs << iInterShipTypes << val;

                    if (nsam == 0) {
                        mMin = val;
                        mMax = val;
                        mAvg = val;
                        mTot = val;
                    } else {
                        mMin = std::min (mMin, val);
                        mMax = std::max (mMax, val);
                        mAvg += val;
                        mTot += val;
                    }
                    ++nsam;
                }
            }
            if (nsam > 0)
                mAvg /= nsam;

            //qDebug() << "Res: " << mMin << mMax << mAvg << mTot;

            keyData[gidx] << it.key();
            switch (graphList[iGraph] / 1000) {
            case 4:
                val = mMax;
                break;
            case 3:
                val = mMin;
                break;
            case 2:
                val = mAvg;
                break;
            case 1:
                val = mTot;
                break;
            }

            valueData[gidx] << val;
        }
        ++it;
    }

    if (!mSaveFilename.isEmpty()) {
        QFile f(mSaveFilename);
        if (f.open(QIODevice::WriteOnly)) {
            QTextStream strm(&f);


            strm << "interShipsIDsList: ";
            for (auto x : interShipsIDsList)
                strm << x << " ";
            strm << "\n\n";

            for (int i = 0; i < graphs.size(); ++i) {
                strm << "Dt " << i << " (" <<graphs.at(i)->name() << ") ";

                auto const& k = keyData.at(i);
                auto const& v = valueData.at(i);
                for (int j = 0; j < k.size(); ++j) {
                    strm << "," << k.at(j) << "," << v.at(j);
                }
                strm << "\n";
            }

            strm << "\n------\n\n";

            it = model->getShipsStatistics().getFirst();
            for (int istep = 0; istep <nsteps; ++istep) {
                int nInterShipsIDs = interShipsIDsList.size();

                //qDebug() << "Step: " <<istep << it.key();
                for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
                    //int gidx = iInterShipsIDs * graphNum + iGraph;
                    int gidx = iGraph;

                    auto group = graphList[iGraph] % 1000;

                    //qDebug() << "Graph:" << iGraph << group;
                     // calculate transversal values...
                    double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0, nsam = 0;
                    for (int iInterShipTypes = 0; iInterShipTypes < interShipsTypesList.size(); ++iInterShipTypes) {
                        for (int iInterShipsIDs = 0; iInterShipsIDs < nInterShipsIDs; ++iInterShipsIDs) {

                            auto fmtype = model->getShipList()[iInterShipsIDs]->mShip->get_typecode();
                            if (group != 999 && iInterShipTypes != fmtype)
                                continue;

                            val = getStatValue(model, it.key(), interShipsIDsList[iInterShipsIDs], interShipsTypesList[iInterShipTypes], stat);

                            strm << it.key() << ", " << iInterShipsIDs << ", " << iInterShipTypes << ", " << group
                                 << ", " << static_cast<int>(stat) << " ==> " << val << "\n";
                        }
                    }
                }
                ++it;
            }
            f.close();
        }

        mSaveFilename.clear();
    }

    for (int i = 0; i < graphs.size(); ++i) {
        graphs[i]->setData(keyData.at(i), valueData.at(i));
    }

    switch (stat) {
    case ShipsStat::SH_NbTransportedUnits:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Nb. Transported Units"));
        break;
    case ShipsStat::SH_FuelPerHour:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Nb. Transported Units"));
        break;
    case ShipsStat::SH_NOxEmission_gperkW:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("NOx Emission Factor (g per kW)"));
        break;
    case ShipsStat::SH_SOxEmission_PercentPerFuelMass:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("SOx Emission Factor (g per kW)"));
        break;
    case ShipsStat::SH_GHGEmission_gperkW:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("GHG Emission Factor (g per kW)"));
        break;
    case ShipsStat::SH_PMEEmission_gperkW:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("PME Emission Factor (g per kW)"));
        break;
    case ShipsStat::SH_FuelUseLitre:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Fule Use (litre)"));
        break;
    case ShipsStat::SH_NOxEmission:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("NOx Emission (g)"));
        break;
    case ShipsStat::SH_SOxEmission:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("SOx Emission (g)"));
        break;
    case ShipsStat::SH_GHGEmission:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("GHG Emission (g)"));
        break;
    case ShipsStat::SH_PMEEmission:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("PME Emission (g)"));
        break;
    }

    mPlot->rescaleAxes();
    mPlot->replot();
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

double ShipsStatsPlot::getStatValue(DisplaceModel *model, int tstep, int farmid, int farmtype, displace::plot::ShipsStat stattype)
{
    Q_UNUSED(farmtype);
    switch (stattype) {
    case ShipsStat::SH_NbTransportedUnits:
        return model->getShipsStatistics().getValue(tstep).NbTransportedUnitsForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_FuelPerHour:
        return model->getShipsStatistics().getValue(tstep).FuelPerHourForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_NOxEmission_gperkW:
        return model->getShipsStatistics().getValue(tstep).NOxEmissionFactorForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_SOxEmission_PercentPerFuelMass:
        return model->getShipsStatistics().getValue(tstep).SOxEmissionFactorForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_GHGEmission_gperkW:
        return model->getShipsStatistics().getValue(tstep).GHGEmissionFactorForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_PMEEmission_gperkW:
        return model->getShipsStatistics().getValue(tstep).PMEEmissionFactorForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_FuelUseLitre:
        return model->getShipsStatistics().getValue(tstep).FuelUseLitreForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_NOxEmission:
        return model->getShipsStatistics().getValue(tstep).NOxEmissionForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_SOxEmission:
        return model->getShipsStatistics().getValue(tstep).SOxEmissionForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_GHGEmission:
        return model->getShipsStatistics().getValue(tstep).GHGEmissionForShipAndShipGroup(farmid, 0);
    case ShipsStat::SH_PMEEmission:
        return model->getShipsStatistics().getValue(tstep).PMEEmissionForShipAndShipGroup(farmid, 0);
    }

    return 0;
}
