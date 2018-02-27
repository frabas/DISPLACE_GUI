#ifndef METIERSSTATSPLOT_H
#define METIERSSTATSPLOT_H

#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class MetiersStatsPlot
{
    QCustomPlot *plot;
    QCPItemLine *timeline;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::MetiersStat metStat;
public:
    MetiersStatsPlot(QCustomPlot *plot_);

    void update(DisplaceModel *model);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);

    void setCurrentTimeStep(double t);
    void setStat(displace::plot::MetiersStat stat) { metStat = stat; }
private:
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::MetiersStat stat, int metier);
};

#endif // METIERSSTATSPLOT_H
