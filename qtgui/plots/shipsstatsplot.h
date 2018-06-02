#ifndef SHIPSSTATSPLOT_H
#define SHIPSSTATSPLOT_H

#include "BaseStatsPlotImpl.h"
#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class ShipsStatsPlot : public BaseStatsPlotImpl
{
    QCustomPlot *mPlot;
    QCPItemLine *mTimeline;
    QPen pen;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::ShipsStat lastStat;

    QString mSaveFilename;
public:
    ShipsStatsPlot(QCustomPlot *plot, QCPItemLine *timeLine);

    void setTimelineLimits(double min, double max) {
        timelineMin = min;
        timelineMax = max;
    }

    void update(DisplaceModel *model, displace::plot::ShipsStat stat, QCustomPlot *plot);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);
private:
//    double getStatValue(DisplaceModel *model, displace::plot::ShipsStat stattype, displace::plot::AggregationType aggtype, int shipid, std::vector<int> shiptypeid);
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model,
                                                        displace::plot::ShipsStat stattype,
                                                        displace::plot::AggregationType aggtype,
                                                        int shipid,
                                                        std::vector<int> shiptypeid);

protected:
    void update(QCustomPlot *plot) override;
};



#endif // SHIPSSTATSPLOT_H
