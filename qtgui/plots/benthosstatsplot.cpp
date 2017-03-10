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

    QList<int> interBenthosList = model->getInterestingBenthos();

    auto funcGroups = model->getFunctionalGroupsList();
    QList<int> interFuncGroupsList = funcGroups->list();

    QList<int> graphList;
    bool showtotal = funcGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  funcGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  funcGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  funcGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    if (showmax)
        graphList.push_front(-4);
    if (showmin)
        graphList.push_front(-3);
    if (showavg)
        graphList.push_front(-2);
    if (showtotal)
        graphList.push_front(-1);

    if (graphList.size() == 0)
        graphList.append(interFuncGroupsList);

    /* If no size is selected, but aggregate is selected, select all sizes */
    if (interFuncGroupsList.size() == 0 && graphList.size() != 0) {
        for(int i = 0; i < model->getNumFuncGroups(); ++i)
            interFuncGroupsList.push_back(i);
    }

    int szNum = interFuncGroupsList.size();
    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;
    QList<QVector<double> >keyData;
    QList<QVector<double> >valueData;

    double t = model->getCurrentStep();
    if (mTimeline != nullptr) {
        mTimeline->start->setCoords(t, timelineMin);
        mTimeline->end->setCoords(t, timelineMax);
    }

    foreach (int funcgrp, interBenthosList) {
        for (int igraph = 0; igraph < graphNum; ++igraph) {
            // Creates graph. Index in list are: ip * nsz + isz
            QCPGraph *graph = mPlot->addGraph();
            QColor col = mPalette.colorByIndex(funcgrp);

            graph->setLineStyle(QCPGraph::lsLine);
            graph->setPen(QPen(QBrush(col),2));

            col.setAlpha(128);
            graph->setBrush(QBrush(col));

            switch (graphList[igraph]) {
            case -4:
                graph->setName(QString(QObject::tr("FuncGrp %1 Max")).arg(funcgrp));
                break;
            case -3:
                graph->setName(QString(QObject::tr("FuncGrp %1 Min")).arg(funcgrp));
                break;
            case -2:
                graph->setName(QString(QObject::tr("FuncGrp %1 Avg")).arg(funcgrp));
                break;
            case -1:
                graph->setName(QString(QObject::tr("FuncGrp %1 Total")).arg(funcgrp));
                break;
            default:
                graph->setName(QString(QObject::tr("Benthos %1 FuncGrp %2")).arg(funcgrp).arg(graphList[igraph]+1));
            }

            graphs.push_back(graph);
            keyData.push_back(QVector<double>());
            valueData.push_back(QVector<double>());
        }
    }

    int nsteps = model->getBenthosStatistics().getUniqueValuesCount();

    qDebug() << "Plotting benthos graph: " << interBenthosList << interFuncGroupsList;

    auto it = model->getBenthosStatistics().getFirst();
    for (int istep = 0; istep <nsteps; ++istep) {
        int nInterBenthos = interBenthosList.size();
        for (int iInterBenthos = 0; iInterBenthos < nInterBenthos; ++iInterBenthos) {

            // calculate transversal values...
            double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0;
            for (int iInterFuncGroup = 0; iInterFuncGroup < interFuncGroupsList.size(); ++iInterFuncGroup) {
                val = getStatValue(model, it.key(), interBenthosList[iInterBenthos], interFuncGroupsList[iInterFuncGroup], stat);
                if (iInterFuncGroup == 0) {
                    mMin = val;
                    mMax = val;
                } else {
                    if (mMin > val)
                        mMin = val;
                    if (mMax < val)
                        mMax = val;
                }
                mAvg += val;
                mTot += val;
            }
            if (szNum > 0)
                mAvg /= szNum;

            for (int iGraph = 0; iGraph < graphNum; ++iGraph) {
                int gidx = iInterBenthos * graphNum + iGraph;

                keyData[gidx] << it.key();
                switch (graphList[iGraph]) {
                case -4:
                    val = mMax;
                    break;
                case -3:
                    val = mMin;
                    break;
                case -2:
                    val = mAvg;
                    break;
                case -1:
                    val = mTot;
                    break;
                default:
                    val = getStatValue(model, it.key(), interBenthosList[iInterBenthos], graphList[iGraph], stat);
                    break;
                }

                valueData[gidx] << val;
            }
        }
        ++it;
    }

    for (int i = 0; i < graphs.size(); ++i) {
        graphs[i]->setData(keyData.at(i), valueData.at(i));
    }


    switch (stat) {
    case BenthosStat::B_TotBiomass:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("Numbers"));
        break;
    case BenthosStat::B_Number:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("F"));
        break;
    case BenthosStat::B_MeanWeight:
        mPlot->xAxis->setLabel(QObject::tr("Time (h)"));
        mPlot->yAxis->setLabel(QObject::tr("SSB (kg)"));
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
    }

    return 0;
}
