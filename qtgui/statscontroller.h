#ifndef STATSCONTROLLER_H
#define STATSCONTROLLER_H

#include <QObject>
#include <qcustomplot.h>
#include <palettemanager.h>

class DisplaceModel;

class StatsController : public QObject
{
    Q_OBJECT
public:
    explicit StatsController(QObject *parent = 0);

    void setPopulationPlot(QCustomPlot *plot);
    void setHarboursPlot(QCustomPlot *plot);
    void setNationsPlot(QCustomPlot *plot);

    void updateStats(DisplaceModel *model);

    enum PopulationStat { Aggregate, Mortality };
    void setPopulationStat(PopulationStat stat);
    PopulationStat getPopulationStat() const { return mSelectedPopStat; }

    enum NationsStat { Catches, Earnings, TimeAtSea };
    void setNationsStat(NationsStat stat);
    NationsStat getNationsStat() const { return mSelectedNationsStat; }

    enum HarboursStat { H_Catches, H_Earnings };
    void setHarbourStat(HarboursStat stat);
    HarboursStat getHarboursStat() const { return mSelectedHarboursStat; }

    /* == */

    void initPlots();

protected:
    void updatePopulationStats(DisplaceModel *model);
    void updateNationStats(DisplaceModel *model);
    void updateHarboursStats (DisplaceModel *model);

private:
    Palette mPalette;

    /* Populations stuff */
    QCustomPlot *mPlotPopulations;
    PopulationStat mSelectedPopStat;

    /* Harbour stuff */
    QCustomPlot *mPlotHarbours;
    HarboursStat mSelectedHarboursStat;

    /* Nations */
    QCustomPlot *mPlotNations;
    NationsStat mSelectedNationsStat;

    DisplaceModel *mLastModel;
};

#endif // STATSCONTROLLER_H
