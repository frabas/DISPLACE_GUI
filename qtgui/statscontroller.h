#ifndef STATSCONTROLLER_H
#define STATSCONTROLLER_H

#include <QObject>
#include <qcustomplot.h>

class DisplaceModel;

class StatsController : public QObject
{
    Q_OBJECT

    /* Populations stuff */
    QCustomPlot *mPlotPopulations;
    QCPBars *mPlotPopulationsBar;
    QCPBars *mPlotPopulationsBar2;

    DisplaceModel *mLastModel;
public:
    explicit StatsController(QObject *parent = 0);

    void setPopulationPlot(QCustomPlot *plot);

    void updateStats(DisplaceModel *model);

    /* == */

    void initPlots();
};

#endif // STATSCONTROLLER_H
