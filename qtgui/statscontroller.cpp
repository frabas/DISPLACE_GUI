#include "statscontroller.h"
#include <displacemodel.h>
#include <QVector>

StatsController::StatsController(QObject *parent)
    : QObject(parent),
      mPlotPopulations(0), mPlotPopulationsBar(0),
      mLastModel(0)
{
}

void StatsController::setPopulationPlot(QCustomPlot *plot)
{
    mPlotPopulations = plot;
    mPlotPopulationsBar = new QCPBars(mPlotPopulations->xAxis, mPlotPopulations->yAxis);

    mPlotPopulations->addPlottable(mPlotPopulationsBar);
}

void StatsController::updateStats(DisplaceModel *model)
{
    if (!model)
        return;

    if (mPlotPopulations) {
        QVector<double> keyData;
        QVector<double> valueData;
        QVector<QString> labels;

        int cntr = 0;
        for (int i = 0; i < model->getPopulationsCount(); ++i) {
            if (model->isInterestingPop(i)) {
                std::shared_ptr<PopulationData> pop = model->getPopulations(i);

                keyData << cntr++;
                labels << QString::number(i);

                switch (mSelectedPopStat) {
                case Aggregate:
                    valueData << pop->getAggregate();
                    break;
                case Mortality:
                    valueData << pop->getMortality();
                    break;
                }
            }
        }

        mPlotPopulationsBar->setData(keyData, valueData);
        mPlotPopulations->xAxis->setAutoTicks(false);
        mPlotPopulations->xAxis->setAutoTickLabels(false);
        mPlotPopulations->xAxis->setTickVector(keyData);
        mPlotPopulations->xAxis->setTickVectorLabels(labels);
        mPlotPopulations->rescaleAxes();
        mPlotPopulations->replot();
    }

    mLastModel = model;
}

void StatsController::setPopulationStat(StatsController::PopulationStat stat)
{
    mSelectedPopStat = stat;
    updateStats(mLastModel);
}

void StatsController::initPlots()
{
    if (mPlotPopulations) {

    }
}

