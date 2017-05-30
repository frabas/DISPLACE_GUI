#ifndef FISHFARMSSTATSPLOT_H
#define FISHFARMSSTATSPLOT_H

#include <plots/plottypes.h>
#include <palettemanager.h>

#include <QPen>
#include <QColor>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class FishfarmsStatsPlot
{
    QCustomPlot *mPlot;
    QCPItemLine *mTimeline;
    QPen pen;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

public:
    FishfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::FishfarmsStat stat);

private:
    double getStatValue(DisplaceModel *model, int tstep, int benthos, int funcgroup, displace::plot::FishfarmsStat stattype);
};


#endif // FISHFARMSSTATSPLOT_H
