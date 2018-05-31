#ifndef VESSELSSTATSPLOT_H
#define VESSELSSTATSPLOT_H

#include "StatsPlot.h"
#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class VesselsStatsPlot : public StatsPlot
{
    QCustomPlot *plotVessels;
    QCPItemLine *timeline;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::VesselsStat lastStat;
public:
    VesselsStatsPlot(QCustomPlot *plotVessels, QCPItemLine *timeLine);

    void update(DisplaceModel *model, displace::plot::VesselsStat stat, QCustomPlot *theplot = nullptr);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);
private:
    void update (QCustomPlot *);
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::VesselsStat stat, int vessel);

protected:
    void doUpdate() override;
};

#endif // VESSELSSTATSPLOT_H
