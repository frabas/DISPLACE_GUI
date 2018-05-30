//
// Created by Federico Fuga on 29/05/18.
//

#ifndef DISPLACE_PLOTWIDGET_H
#define DISPLACE_PLOTWIDGET_H

#include "qcustomplot.h"
#include <qtcommons_global.h>

class StatsPlot;

class QTCOMMONSSHARED_EXPORT PlotWidget : public QCustomPlot {
    StatsPlot *plot = nullptr;
public:
    explicit PlotWidget (QWidget *widget = nullptr)
            : QCustomPlot(widget) {

    }

    void setStatsPlot (StatsPlot *plt) { plot = plt; }

protected:
    void showEvent(QShowEvent *event) override;

    void hideEvent(QHideEvent *event) override;
};


#endif //DISPLACE_PLOTWIDGET_H
