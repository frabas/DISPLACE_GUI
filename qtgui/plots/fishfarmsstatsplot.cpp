#include "fishfarmsstatsplot.h"


#include <displacemodel.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

FishfarmsStatsPlot::FishfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeline)
    : mPlot(plot),
      mTimeline(timeline),
      pen(QColor(0,0,255,200))
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void FishfarmsStatsPlot::update(DisplaceModel *model, displace::plot::FishfarmsStat stat)
{
    mPlot->clearGraphs();
    double val;

    QList<int>  interFishfarmsIDsList= model->getInterestingFishfarms();

    auto farmsTypeGroups = model->getFishfarmsTypesList();
    QList<int> interFishfarmsTypesList = farmsTypeGroups->list();

    QList<int> graphList;
    bool showtotal = farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    /* If no farm type is selected, but aggregate is selected, select all farms */
    if (interFishfarmsTypesList.size() != 0) {
        for(int i = 0; i < model->getNumFishfarmsTypes(); ++i) {
            if (!farmsTypeGroups->has(i))
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
        interFishfarmsTypesList.push_back(999);
        if (showmax)
            graphList.push_front(4999);
        if (showmin)
            graphList.push_front(3999);
        if (showavg)
            graphList.push_front(2999);
        if (showtotal)
            graphList.push_front(1999);
    }

    /* If no fishfarms is selected, select all fishfarms type */
    if (interFishfarmsIDsList.size() == 0) {
        for (int i = 0; i < model->getFishfarmsCount(); ++i) {
            interFishfarmsIDsList.push_back(i+1);
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
            graph->setName(QString(QObject::tr("farm id %1 Max")).arg(group));
            break;
        case 3:
            graph->setName(QString(QObject::tr("farm id %1 Min")).arg(group));
            break;
        case 2:
            graph->setName(QString(QObject::tr("farm id %1 Avg")).arg(group));
            break;
        case 1:
            graph->setName(QString(QObject::tr("farm id %1 Total")).arg(group));
            break;
        }

        graphs.push_back(graph);
        keyData.push_back(QVector<double>());
        valueData.push_back(QVector<double>());
    }

    int nsteps = model->getFishfarmsStatistics().getUniqueValuesCount();

    //qDebug() << "**** Plotting " << nsteps << interFishfarmsTypesList.size();
    auto it = model->getFishfarmsStatistics().getFirst();
    for (int istep = 0; istep <nsteps; ++istep) {
        int nInterFishfarmsIDs = interFishfarmsIDsList.size();

        //qDebug() << "Step: " <<istep << it.key();
        for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
            //int gidx = iInterFishfarmsIDs * graphNum + iGraph;
            int gidx = iGraph;

            auto group = graphList[iGraph] % 1000;

            //qDebug() << "Graph:" << iGraph << group;
             // calculate transversal values...
            double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0, nsam = 0;
            for (int iInterFishfarmTypes = 0; iInterFishfarmTypes < interFishfarmsTypesList.size(); ++iInterFishfarmTypes) {
                for (int iInterFishfarmsIDs = 0; iInterFishfarmsIDs < nInterFishfarmsIDs; ++iInterFishfarmsIDs) {

                    auto fmtype = model->getFishfarmList()[iInterFishfarmsIDs]->mFishfarm->get_farmtype();
                    if (group != 999 && iInterFishfarmTypes != fmtype)
                        continue;

                    val = getStatValue(model, it.key(), interFishfarmsIDsList[iInterFishfarmsIDs], interFishfarmsTypesList[iInterFishfarmTypes], stat);

                    //qDebug() << iInterFishfarmsIDs << iInterFishfarmTypes << val;

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


            strm << "interFishfarmsIDsList: ";
            for (auto x : interFishfarmsIDsList)
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

            it = model->getFishfarmsStatistics().getFirst();
            for (int istep = 0; istep <nsteps; ++istep) {
                int nInterFishfarmsIDs = interFishfarmsIDsList.size();

                //qDebug() << "Step: " <<istep << it.key();
                for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
                    //int gidx = iInterFishfarmsIDs * graphNum + iGraph;
                    int gidx = iGraph;

                    auto group = graphList[iGraph] % 1000;

                    //qDebug() << "Graph:" << iGraph << group;
                     // calculate transversal values...
                    double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0, nsam = 0;
                    for (int iInterFishfarmTypes = 0; iInterFishfarmTypes < interFishfarmsTypesList.size(); ++iInterFishfarmTypes) {
                        for (int iInterFishfarmsIDs = 0; iInterFishfarmsIDs < nInterFishfarmsIDs; ++iInterFishfarmsIDs) {

                            auto fmtype = model->getFishfarmList()[iInterFishfarmsIDs]->mFishfarm->get_farmtype();
                            if (group != 999 && iInterFishfarmTypes != fmtype)
                                continue;

                            val = getStatValue(model, it.key(), interFishfarmsIDsList[iInterFishfarmsIDs], interFishfarmsTypesList[iInterFishfarmTypes], stat);

                            strm << it.key() << ", " << iInterFishfarmsIDs << ", " << iInterFishfarmTypes << ", " << group
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
    case FishfarmsStat::FF_FishMeanWeight:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Fish mean weight (kg)"));
        break;
    case FishfarmsStat::FF_FishHarvestedKg:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Fish Harvested (kg)"));
        break;
    case FishfarmsStat::FF_EggsHarvestedKg:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Eggs Harvested (kg)"));
        break;
    case FishfarmsStat::FF_AnnualProfit:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Annual Profit (euro)"));
        break;
    case FishfarmsStat::FF_NetDischargeN:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Net discharge N (kg)"));
        break;
    case FishfarmsStat::FF_NetDischargeP:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Net discharge P (kg)"));
        break;
    case FishfarmsStat::FF_CumulNetDischargeN:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Acc. Net discharge N (kg)"));
        break;
    case FishfarmsStat::FF_CumulNetDischargeP:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Acc. Net discharge P (kg)"));
        break;
    }

    mPlot->rescaleAxes();
    mPlot->replot();
}

void FishfarmsStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&FishfarmsStatsPlot::saveTo, this));
    }
}

void FishfarmsStatsPlot::saveTo()
{
    mSaveFilename = "fishfarms.txt";
}

double FishfarmsStatsPlot::getStatValue(DisplaceModel *model, int tstep, int farmid, int farmtype, displace::plot::FishfarmsStat stattype)
{
    switch (stattype) {
    case FishfarmsStat::FF_FishMeanWeight:
        return model->getFishfarmsStatistics().getValue(tstep).meanwForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_FishHarvestedKg:
        return model->getFishfarmsStatistics().getValue(tstep).fishharvestedkgForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_EggsHarvestedKg:
        return model->getFishfarmsStatistics().getValue(tstep).eggsharvestedkgForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_AnnualProfit:
        return model->getFishfarmsStatistics().getValue(tstep).annualprofitForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_NetDischargeN:
        return model->getFishfarmsStatistics().getValue(tstep).netdischargeNForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_NetDischargeP:
        return model->getFishfarmsStatistics().getValue(tstep).netdischargePForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_CumulNetDischargeN:
        return model->getFishfarmsStatistics().getValue(tstep).cumulnetdischargeNForFishfarmAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_CumulNetDischargeP:
        return model->getFishfarmsStatistics().getValue(tstep).cumulnetdischargePForFishfarmAndFarmGroup(farmid, farmtype);
    }

    return 0;
}
