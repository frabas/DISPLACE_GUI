//
// Created by Federico Fuga on 29/05/18.
//

#include "PlotWidget.h"

#include "StatsPlot.h"

void PlotWidget::showEvent(QShowEvent *event)
{
    if (plot) {
        plot->setVisible(true);
        plot->update();
    }

    QWidget::showEvent(event);
}

void PlotWidget::hideEvent(QHideEvent *event)
{
    if (plot) {
        plot->setVisible(false);
    }
    QWidget::hideEvent(event);
}
