#include "statscontroller.h"
#include <displacemodel.h>
#include <QVector>

StatsController::StatsController(QObject *parent)
    : QObject(parent),
      mPlotPopulations(0), mPlotPopulationsBar(0), mPlotPopulationsBar2(0),
      mLastModel(0)
{
}

void StatsController::setPopulationPlot(QCustomPlot *plot)
{
    mPlotPopulations = plot;
    mPlotPopulationsBar = new QCPBars(mPlotPopulations->xAxis, mPlotPopulations->yAxis);
    mPlotPopulationsBar2 = new QCPBars(mPlotPopulations->xAxis, mPlotPopulations->yAxis2);

    mPlotPopulations->addPlottable(mPlotPopulationsBar);
    mPlotPopulations->addPlottable(mPlotPopulationsBar2);

    mPlotPopulations->yAxis2->setVisible(true);
}

void StatsController::updateStats(DisplaceModel *model)
{
    if (mPlotPopulations) {
        QVector<double> keyData;
        QVector<double> valueData;
        QVector<double> fData;

        for (int i = 0; i < model->getPopulationsCount(); ++i) {
            std::shared_ptr<PopulationData> pop = model->getPopulations(i);

            keyData << i;
            valueData << pop->getAggregate();
            fData << pop->getMortality();
        }

        mPlotPopulationsBar->setData(keyData, valueData);
        mPlotPopulationsBar2->setData(keyData, fData);
        mPlotPopulations->rescaleAxes();
        mPlotPopulations->replot();
    }
}

void StatsController::initPlots()
{
    if (mPlotPopulations) {

    }
}

