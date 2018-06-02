//
// Created by Federico Fuga on 02/06/18.
//

#include "BaseStatsPlotImpl.h"

void BaseStatsPlotImpl::doUpdate()
{
    update(nullptr);
}

void BaseStatsPlotImpl::checkUpdate(QCustomPlot *plot, BaseStatsPlotImpl::HasChangedFunction hasChangedFunction,
                                    BaseStatsPlotImpl::UpdateChangedStateFunction updateChangedStateFunction)
{
    if (plot != nullptr) {
        // do not cache
        update(plot);
    } else {
        if (hasChangedFunction()) {
            updateChangedStateFunction();
            // need to properly update
            invalidate();
        }
        if (isVisible())
            update(nullptr);
    }
}
