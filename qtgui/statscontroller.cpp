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
    if (mPlotPopulations) {
        QVector<double> keyData;
        QVector<double> valueData;

        qDebug() << "Updating stats: " << model->getPopulationsCount();
        for (int i = 0; i < model->getPopulationsCount(); ++i) {
            std::shared_ptr<PopulationData> pop = model->getPopulations(i);

            keyData << i;
            valueData << pop->getAggregate();
        }

        qDebug() << valueData;
        mPlotPopulationsBar->setData(keyData, valueData);
        mPlotPopulations->rescaleAxes();
        mPlotPopulations->replot();
    }
}

void StatsController::initPlots()
{
    if (mPlotPopulations) {

    }
}

