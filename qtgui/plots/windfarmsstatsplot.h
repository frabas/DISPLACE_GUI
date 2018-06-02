#ifndef WINDFARMSSTATSPLOT_H
#define WINDFARMSSTATSPLOT_H

#include "BaseStatsPlotImpl.h"
#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class WindfarmsStatsPlot : public BaseStatsPlotImpl
{
    QCustomPlot *mPlot;
    QCPItemLine *mTimeline;
    QPen pen;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::WindfarmsStat lastStat;

    QString mSaveFilename;
public:
    WindfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::WindfarmsStat stat, QCustomPlot *plot);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);
private:
    void saveTo();

protected:
    void update(QCustomPlot *plot) override;
};



#endif // WINDFARMSSTATSPLOT_H
