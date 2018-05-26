#ifndef FISHFARMSSTATSPLOT_H
#define FISHFARMSSTATSPLOT_H

#include "plottypes.h"
#include <palettemanager.h>
#include <graphinteractioncontroller.h>

#include <QPen>
#include <QColor>

#include <vector>

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

    QString mSaveFilename;
public:
    FishfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::FishfarmsStat stat);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);
private:
    std::tuple<QVector<double>, QVector<double> > getData(DisplaceModel *model,
                                                                              displace::plot::FishfarmsStat stattype,
                                                                              displace::plot::AggregationType aggtype,
                                                                              int popid, std::vector<int> szid);
    double getStatValue(DisplaceModel *model, int tstep, int farmid, int farmtype, displace::plot::FishfarmsStat stattype);
    void saveTo();
};


#endif // FISHFARMSSTATSPLOT_H
