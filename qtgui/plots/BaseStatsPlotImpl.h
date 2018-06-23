//
// Created by Federico Fuga on 02/06/18.
//

#ifndef DISPLACE_BASESTATSPLOTIMPL_H
#define DISPLACE_BASESTATSPLOTIMPL_H

#include "StatsPlot.h"

#include <functional>


class BaseStatsPlotImpl : public StatsPlot
{
public:
    BaseStatsPlotImpl() : StatsPlot() {}

protected:
    using HasChangedFunction = std::function<bool()>;
    using UpdateChangedStateFunction = std::function<void()>;

    void checkUpdate(QCustomPlot *plot, HasChangedFunction hasChangedFunction, UpdateChangedStateFunction updateChangedStateFunction);
    void doUpdate() override;
    virtual void update (QCustomPlot *) = 0;
};


#endif //DISPLACE_BASESTATSPLOTIMPL_H
