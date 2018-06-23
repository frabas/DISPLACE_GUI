#ifndef METIERSSTATSPLOT_H
#define METIERSSTATSPLOT_H

#include "StatsPlot.h"
#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class MetiersStatsPlot : public StatsPlot
{
    QCustomPlot *plot;
    QCPItemLine *timeline;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::MetiersStat metStat;
public:
    explicit MetiersStatsPlot(QCustomPlot *plot_);

    void update(DisplaceModel *model, QCustomPlot *theplot = nullptr);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);

    void setCurrentTimeStep(double t);
    void setStat(displace::plot::MetiersStat stat) { metStat = stat; invalidate(); }
private:
    void update(QCustomPlot *);
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::MetiersStat stat, int metier);

protected:
    void doUpdate() override;
};

#endif // METIERSSTATSPLOT_H
