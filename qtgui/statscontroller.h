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

    enum NationsStat { Catches, Earnings, TimeAtSea, Gav, Vpuf };
    void setNationsStat(NationsStat stat);
    NationsStat getNationsStat() const { return mSelectedNationsStat; }

    enum HarboursStat { H_Catches, H_Earnings, H_Gav, H_Vpuf };
    void setHarbourStat(HarboursStat stat);
    HarboursStat getHarboursStat() const { return mSelectedHarboursStat; }

    /* == */

    void initPlots();

    void setCurrentTimeStep(double t);

protected:
    void updatePopulationStats(DisplaceModel *model);
    double getPopStatValue (DisplaceModel *model, int tstep, int popid, int szid, PopulationStat stattype);

    void updateNationStats(DisplaceModel *model);
    void updateHarboursStats (DisplaceModel *model);

private:
    Palette mPalette;

    /* Populations stuff */
    QCustomPlot *mPlotPopulations;
    PopulationStat mSelectedPopStat;
    QCPItemLine *mPopTimeLine;

    /* Harbour stuff */
    QCustomPlot *mPlotHarbours;
    HarboursStat mSelectedHarboursStat;
    QCPItemLine *mHarbTimeLine;

    /* Nations */
    QCustomPlot *mPlotNations;
    NationsStat mSelectedNationsStat;
    QCPItemLine *mNatTimeLine;

    DisplaceModel *mLastModel;

    static double timelineMin, timelineMax;
};

#endif // STATSCONTROLLER_H
