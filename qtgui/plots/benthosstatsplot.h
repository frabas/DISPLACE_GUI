#ifndef BENTHOSSTATSPLOT_H
#define BENTHOSSTATSPLOT_H

#include "StatsPlot.h"
#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class BenthosStatsPlot : public StatsPlot
{
    QCustomPlot *mPlot;
    QCPItemLine *mTimeline;
    QPen pen;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    QString mSaveFilename;

    DisplaceModel *lastModel;
    displace::plot::BenthosStat lastStat;

public:
    BenthosStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::BenthosStat stat, QCustomPlot *theplot);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);

private:
    void update (QCustomPlot *);
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::BenthosStat stattype, displace::plot::AggregationType aggtype, int grpid, const std::vector<int> &btype);

protected:
    void doUpdate() override;
};

#endif // BENTHOSSTATSPLOT_H
