#ifndef POPULATIONSSTATPLOT_H
#define POPULATIONSSTATPLOT_H

#include "plottypes.h"
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;


class PopulationsStatPlot
{
    QCustomPlot *plot;
    QCPItemLine *timeline;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;

    DisplaceModel *lastModel;
    displace::plot::PopulationStat lastStat;

public:
    PopulationsStatPlot(QCustomPlot *plot);

    void update(DisplaceModel *model, displace::plot::PopulationStat stat);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);

    void setCurrentTimeStep(double t);
private:
    void saveTo();
    std::tuple<QVector<double>,QVector<double>> getData(DisplaceModel *model, displace::plot::NationsStat stat, int nation);

    double getPopStatValue(DisplaceModel *model, int tstep, int popid, int szid, displace::plot::PopulationStat stattype);
};

#endif // POPULATIONSSTATPLOT_H
