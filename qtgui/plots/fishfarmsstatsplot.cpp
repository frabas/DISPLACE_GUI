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
   // mPalette = PaletteManager::instance()->palette(FishfarmRole);
}

void FishfarmsStatsPlot::update(DisplaceModel *model, displace::plot::FishfarmsStat stat)
{
    mPlot->clearGraphs();
    double val;

    QList<int>  interFishfarmsTypesList= model->getInterestingFarmTypes();


    auto farmsIdsGroups = model->getFishfarmsIDsGroupsList();
    QList<int> interFishfarmsIdsList = farmsIdsGroups->list();

    QList<int> graphList;
    bool showtotal = farmsIdsGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Total);
    bool showavg =  farmsIdsGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Average);
    bool showmin =  farmsIdsGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Min);
    bool showmax =  farmsIdsGroups->isSpecialValueSelected(DisplaceModel::SpecialGroups::Max);

    if (showmax)
        graphList.push_front(-4);
    if (showmin)
        graphList.push_front(-3);
    if (showavg)
        graphList.push_front(-2);
    if (showtotal)
        graphList.push_front(-1);

    if (graphList.size() == 0)
        graphList.append(interFishfarmsIdsList);

    /* If no farm is selected, but aggregate is selected, select all farms */
    if (interFishfarmsIdsList.size() == 0 && graphList.size() != 0) {
        for(int i = 0; i < model->getNumFishfarmIDs(); ++i)
            interFishfarmsIdsList.push_back(i);
    }

    /* If no fishfarms type is selected, select all fishfarms type */
    if (interFishfarmsTypesList.size() == 0) {
        for (int i = 0; i < model->getFishfarmsTypeCount(); ++i) {
            interFishfarmsTypesList.push_back(i);
        }
    }

    int szNum = interFishfarmsIdsList.size();
    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;
    QList<QVector<double> >keyData;
    QList<QVector<double> >valueData;

    double t = model->getCurrentStep();
    if (mTimeline != nullptr) {
        mTimeline->start->setCoords(t, timelineMin);
        mTimeline->end->setCoords(t, timelineMax);
    }

    foreach (int farmid, interFishfarmsIdsList) {
        for (int igraph = 0; igraph < graphNum; ++igraph) {
            // Creates graph. Index in list are: ip * nsz + isz
            QCPGraph *graph = mPlot->addGraph();
            QColor col = mPalette.colorByIndex(farmid);

            graph->setLineStyle(QCPGraph::lsLine);
            graph->setPen(QPen(QBrush(col),2));

            col.setAlpha(128);
            graph->setBrush(QBrush(col));

            switch (graphList[igraph]) {
            case -4:
                graph->setName(QString(QObject::tr("farmid %1 Max")).arg(farmid));
                break;
            case -3:
                graph->setName(QString(QObject::tr("farmid %1 Min")).arg(farmid));
                break;
            case -2:
                graph->setName(QString(QObject::tr("farmid %1 Avg")).arg(farmid));
                break;
            case -1:
                graph->setName(QString(QObject::tr("farmid %1 Total")).arg(farmid));
                break;
            default:
                graph->setName(QString(QObject::tr("farm type %1 farmid %2")).arg(farmid).arg(graphList[igraph]+1));
            }

            graphs.push_back(graph);
            keyData.push_back(QVector<double>());
            valueData.push_back(QVector<double>());
        }
    }

    int nsteps = model->getFishfarmsStatistics().getUniqueValuesCount();

    auto it = model->getFishfarmsStatistics().getFirst();
    for (int istep = 0; istep <nsteps; ++istep) {
        int nInterFishfarmsTypes = interFishfarmsTypesList.size();
        for (int iInterFishfarmsTypes = 0; iInterFishfarmsTypes < nInterFishfarmsTypes; ++iInterFishfarmsTypes) {

            // calculate transversal values...
            double mMin = 0.0,mMax = 0.0,mAvg = 0.0,mTot = 0.0;
            for (int iInterFishfarmIds = 0; iInterFishfarmIds < interFishfarmsIdsList.size(); ++iInterFishfarmIds) {
                val = getStatValue(model, it.key(), interFishfarmsTypesList[iInterFishfarmsTypes], interFishfarmsIdsList[iInterFishfarmIds], stat);
                if (iInterFishfarmIds == 0) {
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
                int gidx = iInterFishfarmsTypes * graphNum + iGraph;

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
                    val = getStatValue(model, it.key(), interFishfarmsTypesList[iInterFishfarmsTypes], graphList[iGraph], stat);
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
    }

    mPlot->rescaleAxes();
    mPlot->replot();
}

double FishfarmsStatsPlot::getStatValue(DisplaceModel *model, int tstep, int farmtype, int farmid, displace::plot::FishfarmsStat stattype)
{
    switch (stattype) {
    case FishfarmsStat::FF_FishMeanWeight:
        return model->getFishfarmsStatistics().getValue(tstep).meanwForFarmTypeAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_FishHarvestedKg:
        return model->getFishfarmsStatistics().getValue(tstep).fishharvestedkgForFarmTypeAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_EggsHarvestedKg:
        return model->getFishfarmsStatistics().getValue(tstep).eggsharvestedkgForFarmTypeAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_AnnualProfit:
        return model->getFishfarmsStatistics().getValue(tstep).annualprofitForFarmTypeAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_NetDischargeN:
        return model->getFishfarmsStatistics().getValue(tstep).netdischargeNForFarmTypeAndFarmGroup(farmid, farmtype);
    case FishfarmsStat::FF_NetDischargeP:
        return model->getFishfarmsStatistics().getValue(tstep).netdischargePForFarmTypeAndFarmGroup(farmid, farmtype);
    }

    return 0;
}
