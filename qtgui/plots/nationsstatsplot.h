#ifndef NATIONSSTATSPLOT_H
#define NATIONSSTATSPLOT_H

#include <plots/plottypes.h>
#include <palettemanager.h>

#include <QPen>
#include <QColor>

#include <graphinteractioncontroller.h>

class DisplaceModel;
class QCustomPlot;
class QCPItemLine;

class NationsStatsPlot
{
    QCustomPlot *plotNations;
    QCPItemLine *timeline;
    double timelineMax = 1e20;
    double timelineMin = -1e20;
    Palette mPalette;
public:
    NationsStatsPlot(QCustomPlot *plotNations, QCPItemLine *timeLine);

    void update(DisplaceModel *model, displace::plot::NationsStat stat);
    void createPopup (GraphInteractionController::PopupMenuLocation location, QMenu *menu);
private:
    void saveTo();
};

#endif // NATIONSSTATSPLOT_H
