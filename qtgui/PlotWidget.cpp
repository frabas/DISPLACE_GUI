//
// Created by Federico Fuga on 29/05/18.
//

#include "PlotWidget.h"

#include "plots/StatsPlot.h"

void PlotWidget::showEvent(QShowEvent *event)
{
    if (plot)
        plot->update();

    QWidget::showEvent(event);
}
