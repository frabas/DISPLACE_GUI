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
    mPalette = PaletteManager::instance()->palette(PopulationRole);
}

void BenthosStatsPlot::update(DisplaceModel *model, displace::plot::BenthosStat stat)
{
    mPlot->clearGraphs();
    double val;

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

    int szNum = interSizeList.size();
    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;
    QList<QVector<double> >keyData;
    QList<QVector<double> >valueData;

    double t = model->getCurrentStep();
    if (mTimeline != nullptr) {
        mTimeline->start->setCoords(t, timelineMin);
        mTimeline->end->setCoords(t, timelineMax);
    }

    foreach (int ipop, interPopList) {
        for (int igraph = 0; igraph < graphNum; ++igraph) {
            // Creates graph. Index in list are: ip * nsz + isz
            QCPGraph *graph = mPlot->addGraph();
            QColor col = mPalette.colorByIndex(ipop);

            graph->setLineStyle(QCPGraph::lsLine);
            graph->setPen(QPen(QBrush(col),2));

            col.setAlpha(128);
            graph->setBrush(QBrush(col));

            switch (graphList[igraph]) {
            case -4:
                graph->setName(QString(QObject::tr("Pop %1 Max")).arg(ipop));
                break;
            case -3:
                graph->setName(QString(QObject::tr("Pop %1 Min")).arg(ipop));
                break;
            case -2:
                graph->setName(QString(QObject::tr("Pop %1 Avg")).arg(ipop));
                break;
            case -1:
                graph->setName(QString(QObject::tr("Pop %1 Total")).arg(ipop));
                break;
            default:
                graph->setName(QString(QObject::tr("Pop %1 Group %2")).arg(ipop).arg(graphList[igraph]+1));
            }

            graphs.push_back(graph);
            keyData.push_back(QVector<double>());
            valueData.push_back(QVector<double>());
        }
    }

    int nsteps = model->getPopulationsValuesCount();

    DisplaceModel::PopulationStatContainer::Container::const_iterator it = model->getPopulationsFirstValue();
    for (int istep = 0; istep <nsteps; ++istep) {
        int ninterPop = interPopList.size();
        for (int iinterpPop = 0; iinterpPop < ninterPop; ++iinterpPop) {

            // calculate transversal values...
            double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0;
            for (int iInterSize = 0; iInterSize < interSizeList.size(); ++iInterSize) {
                val = getStatValue(model, it.key(), interPopList[iinterpPop], interSizeList[iInterSize], stat);
                if (iInterSize == 0) {
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

            for (int isz = 0; isz < graphNum; ++isz) {
                int gidx = iinterpPop * graphNum + isz;

                keyData[gidx] << it.key();
                switch (graphList[isz]) {
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
                    val = getStatValue(model, it.key(), interPopList[iinterpPop], graphList[isz], stat);
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

double BenthosStatsPlot::getStatValue(DisplaceModel *model, int tstep, int popid, int szid, displace::plot::BenthosStat stattype)
{
    switch (stattype) {
    case BenthosStat::B_TotBiomass:
        return model->getPopulationsAtStep(tstep, popid).getAggregateAt(szid);
    case BenthosStat::B_Number:
        return model->getPopulationsAtStep(tstep, popid).getMortalityAt(szid);
    case BenthosStat::B_MeanWeight: {
        auto &x = model->getPopulationsAtStep(tstep, popid).getSSB();
        if (szid < x.size())
            return x.at(szid);
        return 0;
        }
    }

    return 0;
}
