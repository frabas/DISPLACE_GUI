#include "windfarmsstatsplot.h"


#include <displacemodel.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

WindfarmsStatsPlot::WindfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeline)
    : mPlot(plot),
      mTimeline(timeline),
      pen(QColor(0,0,255,200))
{
    mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void WindfarmsStatsPlot::update(DisplaceModel *model, displace::plot::WindfarmsStat stat)
{
    mPlot->clearGraphs();
    double val;

    QList<int>  interWindfarmsIDsList= model->getInterestingWindfarms();

    auto farmsTypeGroups = model->getWindfarmsTypesList();
    QList<int> interWindfarmsTypesList = farmsTypeGroups->list();

    QList<int> graphList;
    bool showtotal = farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  farmsTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    /* If no farm type is selected, but aggregate is selected, select all farms */
    if (interWindfarmsTypesList.size() != 0) {
        for(int i = 0; i < model->getNumWindfarmsTypes(); ++i) {
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
        interWindfarmsTypesList.push_back(999);
        if (showmax)
            graphList.push_front(4999);
        if (showmin)
            graphList.push_front(3999);
        if (showavg)
            graphList.push_front(2999);
        if (showtotal)
            graphList.push_front(1999);
    }

    /* If no Windfarms is selected, select all Windfarms type */
    if (interWindfarmsIDsList.size() == 0) {
        for (int i = 0; i < model->getWindmillCount(); ++i) {
            interWindfarmsIDsList.push_back(i+1);
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

    int nsteps = model->getWindfarmsStatistics().getUniqueValuesCount();

    //qDebug() << "**** Plotting " << nsteps << interWindfarmsTypesList.size();
    auto it = model->getWindfarmsStatistics().getFirst();
    for (int istep = 0; istep <nsteps; ++istep) {
        int nInterWindfarmsIDs = interWindfarmsIDsList.size();

        //qDebug() << "Step: " <<istep << it.key();
        for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
            //int gidx = iInterWindfarmsIDs * graphNum + iGraph;
            int gidx = iGraph;

            auto group = graphList[iGraph] % 1000;

            //qDebug() << "Graph:" << iGraph << group;
             // calculate transversal values...
            double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0, nsam = 0;
            for (int iInterWindfarmTypes = 0; iInterWindfarmTypes < interWindfarmsTypesList.size(); ++iInterWindfarmTypes) {
                for (int iInterWindfarmsIDs = 0; iInterWindfarmsIDs < nInterWindfarmsIDs; ++iInterWindfarmsIDs) {

                    auto fmtype = model->getWindmillList()[iInterWindfarmsIDs]->mWindmill->get_type();
                    if (group != 999 && iInterWindfarmTypes != fmtype)
                        continue;

                    val = getStatValue(model, it.key(), interWindfarmsIDsList[iInterWindfarmsIDs], interWindfarmsTypesList[iInterWindfarmTypes], stat);

                    //qDebug() << iInterWindfarmsIDs << iInterWindfarmTypes << val;

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


            strm << "interWindfarmsIDsList: ";
            for (auto x : interWindfarmsIDsList)
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

            it = model->getWindfarmsStatistics().getFirst();
            for (int istep = 0; istep <nsteps; ++istep) {
                int nInterWindfarmsIDs = interWindfarmsIDsList.size();

                //qDebug() << "Step: " <<istep << it.key();
                for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
                    //int gidx = iInterWindfarmsIDs * graphNum + iGraph;
                    int gidx = iGraph;

                    auto group = graphList[iGraph] % 1000;

                    //qDebug() << "Graph:" << iGraph << group;
                     // calculate transversal values...
                    double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0, nsam = 0;
                    for (int iInterWindfarmTypes = 0; iInterWindfarmTypes < interWindfarmsTypesList.size(); ++iInterWindfarmTypes) {
                        for (int iInterWindfarmsIDs = 0; iInterWindfarmsIDs < nInterWindfarmsIDs; ++iInterWindfarmsIDs) {

                            auto fmtype = model->getWindmillList()[iInterWindfarmsIDs]->mWindmill->get_type();
                            if (group != 999 && iInterWindfarmTypes != fmtype)
                                continue;

                            val = getStatValue(model, it.key(), interWindfarmsIDsList[iInterWindfarmsIDs], interWindfarmsTypesList[iInterWindfarmTypes], stat);

                            strm << it.key() << ", " << iInterWindfarmsIDs << ", " << iInterWindfarmTypes << ", " << group
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
    case WindfarmsStat::WF_kWh:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("kWh"));
        break;
    case WindfarmsStat::WF_kWProduction:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("kW Production"));
        break;
    }

    mPlot->rescaleAxes();
    mPlot->replot();
}

void WindfarmsStatsPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&WindfarmsStatsPlot::saveTo, this));
    }
}

void WindfarmsStatsPlot::saveTo()
{
    mSaveFilename = "Windfarms.txt";
}

double WindfarmsStatsPlot::getStatValue(DisplaceModel *model, int tstep, int farmid, int farmtype, displace::plot::WindfarmsStat stattype)
{
    Q_UNUSED(farmtype);
    switch (stattype) {
    case WindfarmsStat::WF_kWh:
        return model->getWindfarmsStatistics().getValue(tstep).kWhForWindfarmAndWindfarmGroup(farmid, 1);
    case WindfarmsStat::WF_kWProduction:
        return model->getWindfarmsStatistics().getValue(tstep).kWProductionForWindfarmAndWindfarmGroup(farmid, 1);
    }

    return 0;
}
