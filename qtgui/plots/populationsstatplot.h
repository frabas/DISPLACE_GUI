#ifndef POPULATIONSSTATPLOT_H
#define POPULATIONSSTATPLOT_H

#include "StatsPlot.h"
#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;


class PopulationsStatPlot : public StatsPlot
{
    QCustomPlot *plot;
    QCPItemLine *timeline;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::PopulationStat lastStat;
public:
    explicit PopulationsStatPlot(QCustomPlot *plot);

    void doUpdate() override;
    void update(DisplaceModel *model, displace::plot::PopulationStat stat, QCustomPlot *plot = nullptr);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);

    void setCurrentTimeStep(double t);
private:
    void update (QCustomPlot *plot, displace::plot::PopulationStat stat);
    void update (QCustomPlot *plot);
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::PopulationStat stattype,
                                                        displace::plot::AggregationType aggtype, int popid,
                                                        std::vector<int> szid);
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::PopulationStat stattype,
                                                        displace::plot::NationsStat stattype2,
                                                        displace::plot::AggregationType aggtype, int popid,
                                                        std::vector<int> szid);
};

#endif // POPULATIONSSTATPLOT_H
