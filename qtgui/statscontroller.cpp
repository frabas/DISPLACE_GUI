#include "statscontroller.h"
#include <displacemodel.h>
#include <QVector>
#include <QtAlgorithms>

StatsController::StatsController(QObject *parent)
    : QObject(parent),
      mPalette(),
      mPlotPopulations(0),
      mSelectedPopStat(Aggregate),
      mPlotHarbours(0),
      mPlotNations(0),
      mSelectedNationsStat(Catches),
      mLastModel(0)
{
    QFile file (":/palettes/iso1996_2.p2c");
    mPalette.loadFromFile(&file);
}

void StatsController::setPopulationPlot(QCustomPlot *plot)
{
    mPlotPopulations = plot;
    mPlotPopulations->legend->setVisible(true);
}

void StatsController::setHarboursPlot(QCustomPlot *plot)
{
    mPlotHarbours = plot;
    mPlotHarbours->legend->setVisible(true);
}

void StatsController::setNationsPlot(QCustomPlot *plot)
{
    mPlotNations = plot;
    mPlotNations->legend->setVisible(true);
}

void StatsController::updateStats(DisplaceModel *model)
{
    if (!model)
        return;

    if (mPlotPopulations) {
        updatePopulationStats(model);
    }
    if (mPlotNations) {
        updateNationStats(model);
    }
    if (mPlotHarbours) {
        updateHarboursStats(model);
    }

    mLastModel = model;
}

void StatsController::setPopulationStat(StatsController::PopulationStat stat)
{
    mSelectedPopStat = stat;
    updateStats(mLastModel);
}

void StatsController::setNationsStat(StatsController::NationsStat stat)
{
    mSelectedNationsStat = stat;
    updateStats(mLastModel);
}

void StatsController::setHarbourStat(StatsController::HarboursStat stat)
{
    mSelectedHarboursStat = stat;
    updateStats(mLastModel);
}

void StatsController::initPlots()
{
    if (mPlotPopulations) {

    }
}

void StatsController::updatePopulationStats(DisplaceModel *model)
{
    static const QPen pen(QColor(0,0,255,200));
    mPlotPopulations->clearGraphs();

    QList<int> ipl = model->getInterestingPops();
    QList<int> szpl = model->getInterestingSizes();
    bool showtotal = model->isInterestingSizeTotal();
    int nsz = szpl.size() + (showtotal ? 1 : 0);

    int cnt;
    int palcnt = 0;
    foreach (int ip, ipl) {

        for (int isz = 0; isz < nsz; ++isz) {
            QVector<double> keyData;
            QVector<double> valueData;

            QCPGraph *graph = mPlotPopulations->addGraph();
            graph->setPen(pen);
            graph->setLineStyle(QCPGraph::lsLine);
            QColor col = mPalette.colorForIndexMod(palcnt % mPalette.colorCount());

            col.setAlpha(128);
            graph->setBrush(QBrush(col));
            ++cnt;

            if (isz < szpl.size()) {        // is a siz group
                graph->setName(QString(QObject::tr("Pop %1 SzGrp %2")).arg(ip).arg(szpl[isz]+1));
            } else {
                graph->setName(QString(QObject::tr("Pop %1 Total")).arg(ip));
            }

            int n = model->getPopulationsValuesCount();
            DisplaceModel::PopulationStatContainer::Container::const_iterator it = model->getPopulationsFirstValue();
            for (int i = 0; i <n; ++i) {
                keyData << it.key();

                switch (mSelectedPopStat) {
                case Aggregate:
                    if (isz < szpl.size())
                        valueData << it.value().at(ip).getAggregate()[szpl[isz]];
                    else
                        valueData << it.value().at(ip).getAggregateTot();
                    break;
                case Mortality:
                    if (isz < szpl.size())
                        valueData << it.value().at(ip).getMortality()[szpl[isz]];
                    else
                        valueData << it.value().at(ip).getMortalityTot();
                    break;
                }

                ++it;
            }

            graph->setData(keyData, valueData);

            ++palcnt;
        }
    }

    mPlotPopulations->rescaleAxes();
    mPlotPopulations->replot();
}

void StatsController::updateNationStats(DisplaceModel *model)
{
    static const QPen pen(QColor(0,0,255,200));
    mPlotNations->clearGraphs();

    QList<int> ipl = model->getInterestingNations();

    int cnt;
    int palcnt = 0;
    foreach (int ip, ipl) {
        QVector<double> keyData;
        QVector<double> valueData;

        QCPGraph *graph = mPlotNations->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = mPalette.colorForIndexMod(palcnt % mPalette.colorCount());

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(QString(model->getNation(ip).getName()));

        int n = model->getNationsStatsCount();
        DisplaceModel::NationsStatsContainer::Container::const_iterator it = model->getNationsStatsFirstValue();
        for (int i = 0; i <n; ++i) {
            keyData << it.key();

            switch (mSelectedNationsStat) {
            case Catches:
                valueData << it.value().at(ip).mTotCatches;
                break;
            case Earnings:
                valueData << it.value().at(ip).mRevenues;
                break;
            case TimeAtSea:
                valueData << it.value().at(ip).mTimeAtSea;
                break;
            }

            ++it;
        }

        graph->setData(keyData, valueData);

        ++palcnt;
    }

    mPlotNations->rescaleAxes();
    mPlotNations->replot();    
}

void StatsController::updateHarboursStats(DisplaceModel *model)
{
    static const QPen pen(QColor(0,0,255,200));
    mPlotHarbours->clearGraphs();

    QList<int> ipl = model->getInterestingHarbours();

    int cnt;
    int palcnt = 0;
    foreach (int ip, ipl) {
        QVector<double> keyData;
        QVector<double> valueData;

        QCPGraph *graph = mPlotHarbours->addGraph();
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsLine);
        QColor col = mPalette.colorForIndexMod(palcnt % mPalette.colorCount());

        col.setAlpha(128);
        graph->setBrush(QBrush(col));
        ++cnt;

        graph->setName(QString::fromStdString(model->getHarbourData(ip).mHarbour->get_name()));

        int n = model->getHarboursStatsCount();
        DisplaceModel::HarboursStatsContainer::Container::const_iterator it = model->getHarboursStatsFirstValue();
        for (int i = 0; i <n; ++i) {
            keyData << it.key();

            switch (mSelectedHarboursStat) {
            case H_Catches:
                valueData << it.value().at(ip).mCumCatches;
                break;
            case H_Earnings:
                valueData << it.value().at(ip).mCumProfit;
                break;
            }

            ++it;
        }

        graph->setData(keyData, valueData);

        ++palcnt;
    }

    mPlotHarbours->rescaleAxes();
    mPlotHarbours->replot();

}

