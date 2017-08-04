#ifndef BENTHOSSTATSPLOT_H
#define BENTHOSSTATSPLOT_H

#include <plots/plottypes.h>
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class BenthosStatsPlot
{
    QCustomPlot *mPlot;
    QCPItemLine *mTimeline;
    QPen pen;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    QString mSaveFilename;

public:
    BenthosStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::BenthosStat stat);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);

private:
    double getStatValue(DisplaceModel *model, int tstep, int benthos, int funcgroup, displace::plot::BenthosStat stattype);
    void saveTo();

};

#endif // BENTHOSSTATSPLOT_H
