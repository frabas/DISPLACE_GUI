#ifndef NATIONSSTATSPLOT_H
#define NATIONSSTATSPLOT_H

#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class NationsStatsPlot
{
    QCustomPlot *plotNations;
    QCPItemLine *timeline;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::NationsStat lastStat;
public:
    NationsStatsPlot(QCustomPlot *plotNations, QCPItemLine *timeLine);

    void update(DisplaceModel *model, displace::plot::NationsStat stat, QCustomPlot *theplot = nullptr);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);
private:
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::NationsStat stat, int nation);
};

#endif // NATIONSSTATSPLOT_H
