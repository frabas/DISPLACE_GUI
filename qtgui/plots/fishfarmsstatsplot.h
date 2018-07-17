#ifndef FISHFARMSSTATSPLOT_H
#define FISHFARMSSTATSPLOT_H

#include "BaseStatsPlotImpl.h"
#include "plottypes.h"
#include <palettemanager.h>
#include <graphinteractioncontroller.h>

#include <QPen>
#include <QColor>

#include <vector>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class FishfarmsStatsPlot : public BaseStatsPlotImpl
{
    QCustomPlot *mPlot = nullptr;
    QCPItemLine *mTimeline = nullptr;
    QCPItemText *textLabel = nullptr;
    QPen pen;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel = nullptr;
    displace::plot::FishfarmsStat lastStat;
public:
    FishfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::FishfarmsStat stat, QCustomPlot *plot);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);

protected:
    void update(QCustomPlot *plot) override;

private:
    void displayPlot(DisplaceModel *model, displace::plot::FishfarmsStat stat);
    std::tuple<QVector<double>, QVector<double> > getData(DisplaceModel *model,
                                                                              displace::plot::FishfarmsStat stattype,
                                                                              displace::plot::AggregationType aggtype,
                                                                              int popid, std::vector<int> szid);
    void saveTo();
};


#endif // FISHFARMSSTATSPLOT_H
