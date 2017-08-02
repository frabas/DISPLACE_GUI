#include "benthosstatsplot.h"

#include <displacemodel.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

BenthosStatsPlot::BenthosStatsPlot(QCustomPlot *plot, QCPItemLine *timeline)
    : mPlot(plot),
      mTimeline(timeline),
      pen(QColor(0,0,255,200))
{
    mPalette = PaletteManager::instance()->palette(BenthosRole);
}

void BenthosStatsPlot::update(DisplaceModel *model, displace::plot::BenthosStat stat)
{
    mPlot->clearGraphs();
    double val;

    QList<int>  interBenthosIDsList= model->getInterestingBenthos();

    auto benthosTypeGroups = model->getFunctionalGroupsList();
    QList<int> interBenthosTypesList = benthosTypeGroups->list();

    QList<int> graphList;
    bool showtotal = benthosTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  benthosTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  benthosTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  benthosTypeGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    /* If no farm type is selected, but aggregate is selected, select all farms */
    if (interBenthosTypesList.size() != 0) {
        for(int i = 0; i < model->getBenthosCount(); ++i) {
            if (!benthosTypeGroups->has(i))
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
        interBenthosTypesList.push_back(999);
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
    if (interBenthosIDsList.size() == 0) {
        for (int i = 0; i < model->getBenthosCount(); ++i) {
            interBenthosIDsList.push_back(i+1);
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

    int nsteps = model->getBenthosStatistics().getUniqueValuesCount();

    //qDebug() << "**** Plotting " << nsteps << interFishfarmsTypesList.size();
    auto it = model->getBenthosStatistics().getFirst();
    for (int istep = 0; istep <nsteps; ++istep) {
        int nInterBenthosIDs = interBenthosIDsList.size();

        //qDebug() << "Step: " <<istep << it.key();
        for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
            //int gidx = iInterFishfarmsIDs * graphNum + iGraph;
            int gidx = iGraph;

            auto group = graphList[iGraph] % 1000;

            //qDebug() << "Graph:" << iGraph << group;
             // calculate transversal values...
            double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0, nsam = 0;
            for (int iInterBenthosTypes = 0; iInterBenthosTypes < interBenthosTypesList.size(); ++iInterBenthosTypes) {
                for (int iInterBenthosIDs = 0; iInterBenthosIDs < nInterBenthosIDs; ++iInterBenthosIDs) {

                    /* TODO: filter on functional groups
                    auto fmtype = model->getBenthosList()[iInterBenthosIDs];
                    if (group != 999 && iInterBenthosTypes != fmtype)
                        continue;
                        */

                    val = getStatValue(model, it.key(), interBenthosIDsList[iInterBenthosIDs], interBenthosTypesList[iInterBenthosTypes], stat);

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

#if 0
    if (!mSaveFilename.isEmpty()) {
        QFile f(mSaveFilename);
        if (f.open(QIODevice::WriteOnly)) {
            QTextStream strm(&f);


            strm << "interFishfarmsIDsList: ";
            for (auto x : interBenthosIDsList)
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

            it = model->getBenthosStatistics().getFirst();
            for (int istep = 0; istep <nsteps; ++istep) {
                int nInterBenthosIDs = interBenthosIDsList.size();

                //qDebug() << "Step: " <<istep << it.key();
                for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
                    //int gidx = iInterFishfarmsIDs * graphNum + iGraph;
                    int gidx = iGraph;

                    auto group = graphList[iGraph] % 1000;

                    //qDebug() << "Graph:" << iGraph << group;
                     // calculate transversal values...
                    double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0, nsam = 0;
                    for (int iInterBenthosTypes = 0; iInterBenthosTypes < interBenthosTypesList.size(); ++iInterBenthosTypes) {
                        for (int iInterBenthosIDs = 0; iInterBenthosIDs < nInterBenthosIDs; ++iInterBenthosIDs) {

                            // TODO Filter out.. as in main code.
                            /*
                            auto fmtype = model->getFishfarmList()[iInterBenthosIDs]->mFishfarm->get_farmtype();
                            if (group != 999 && iInterBenthosTypes != fmtype)
                                continue;
                                */

                            val = getStatValue(model, it.key(), interBenthosIDsList[iInterBenthosIDs], interBenthosTypesList[iInterBenthosTypes], stat);

                            strm << it.key() << ", " << iInterBenthosIDs << ", " << iInterBenthosTypes << ", " << group
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
#endif

    for (int i = 0; i < graphs.size(); ++i) {
        graphs[i]->setData(keyData.at(i), valueData.at(i));
    }

    switch (stat) { // stat is the index in the selecting combo box
    case BenthosStat::B_TotBiomass:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Biomass"));
        break;
    case BenthosStat::B_Number:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Number"));
        break;
    case BenthosStat::B_MeanWeight:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Mean Weight (gram/sq_m)"));
        break;
    case BenthosStat::B_TotBiomassOverK:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("B/K"));
        break;
    case BenthosStat::B_NumberOverK:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("N/K"));
        break;
    }

    mPlot->rescaleAxes();
    mPlot->replot();
}

double BenthosStatsPlot::getStatValue(DisplaceModel *model, int tstep, int benthos, int funcgroup, displace::plot::BenthosStat stattype)
{
    switch (stattype) {
    case BenthosStat::B_TotBiomass:
        return model->getBenthosStatistics().getValue(tstep).biomassForBenthosAndFuncGroup(funcgroup, benthos);
    case BenthosStat::B_Number:
        return model->getBenthosStatistics().getValue(tstep).numberForBenthosAndFuncGroup(funcgroup, benthos);
    case BenthosStat::B_MeanWeight: {
        /*
        auto &x = model->getPopulationsAtStep(tstep, popid).getSSB();
        if (szid < x.size())
            return x.at(szid);*/
        return 0;
        }
    case BenthosStat::B_TotBiomassOverK:
        return model->getBenthosStatistics().getValue(tstep).biomassOverKForBenthosAndFuncGroup(funcgroup, benthos);
    case BenthosStat::B_NumberOverK:
        return model->getBenthosStatistics().getValue(tstep).numberOverKForBenthosAndFuncGroup(funcgroup, benthos);



    }

    return 0;
}
