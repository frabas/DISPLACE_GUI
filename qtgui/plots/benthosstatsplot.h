#ifndef BENTHOSSTATSPLOT_H
#define BENTHOSSTATSPLOT_H

#include "plottypes.h"
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
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::BenthosStat stattype, displace::plot::AggregationType aggtype, int grpid, const std::vector<int> &btype);
};

#endif // BENTHOSSTATSPLOT_H
