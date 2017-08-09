#ifndef WINDFARMSSTATSPLOT_H
#define WINDFARMSSTATSPLOT_H


#include <plots/plottypes.h>
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class WindfarmsStatsPlot
{
    QCustomPlot *mPlot;
    QCPItemLine *mTimeline;
    QPen pen;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    QString mSaveFilename;
public:
    WindfarmsStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::WindfarmsStat stat);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);
private:
    double getStatValue(DisplaceModel *model, int tstep, int farmtype, int farmid, displace::plot::WindfarmsStat stattype);
    void saveTo();
};



#endif // WINDFARMSSTATSPLOT_H
