#ifndef SHIPSSTATSPLOT_H
#define SHIPSSTATSPLOT_H

#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class ShipsStatsPlot
{
    QCustomPlot *mPlot;
    QCPItemLine *mTimeline;
    QPen pen;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    QString mSaveFilename;
public:
    ShipsStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::ShipsStat stat);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);
private:
    double getStatValue(DisplaceModel *model, int tstep, int shipid, int shiptypeid, displace::plot::ShipsStat stattype);
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::ShipsStat stat);
};



#endif // SHIPSSTATSPLOT_H
