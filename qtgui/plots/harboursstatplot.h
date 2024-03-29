#ifndef HARBOURSSTATPLOT_H
#define HARBOURSSTATPLOT_H

#include "StatsPlot.h"

#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class HarboursStatPlot : public StatsPlot
{
    QCustomPlot *plot;
    QCPItemLine *timeline;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::HarboursStat lastStat;
public:
    HarboursStatPlot(QCustomPlot *plotNations);

    void update(DisplaceModel *model, QCustomPlot *theplot = nullptr);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);

    void setCurrentTimeStep(double t);
    void setStat(displace::plot::HarboursStat stat) { lastStat = stat; invalidate(); }

    void update() override;

protected:
    void update (QCustomPlot *);
    void doUpdate() override;

private:
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::HarboursStat stat, int harbour);
};

#endif // HARBOURSSTATPLOT_H
