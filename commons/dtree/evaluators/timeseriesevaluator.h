#ifndef TIMESERIESEVALUATOR_H
#define TIMESERIESEVALUATOR_H

#include <simulation.h>
#include <dtree/stateevaluator.h>
#include <tseries/timeseries.h>
#include <tseries/timeseriesmanager.h>

namespace displace {
namespace dtree {

template <displace::simulation::TimeSeriesManager::Variables Var>
class TimeSeriesEvaluator : public ::dtree::StateEvaluator
{
    int zone;
    int adim;
public:
    TimeSeriesEvaluator();

    double evaluate() const {
        displace::simulation::Simulation::instance()->getTimeSeries(Var, zone, adim);
    }
};

}
}

#endif // TIMESERIESEVALUATOR_H
