#ifndef TIMESERIESEVALUATOR_H
#define TIMESERIESEVALUATOR_H

#include <simulation.h>
#include <Vessel.h>
#include <dtree/stateevaluator.h>
#include <tseries/timeseries.h>
#include <tseries/timeseriesmanager.h>

namespace displace {
namespace dtree {

template <displace::simulation::TimeSeriesManager::Variables Var>
class TimeSeriesEvaluator : public ::dtree::StateEvaluator
{
public:
    TimeSeriesEvaluator() {}

    double evaluate(int tstep, Vessel *v) const {
        // NOTE: tstep must be converted to day!

        displace::simulation::TimeSeries *ts = displace::simulation::Simulation::instance()->getTimeSeries(Var, 0, v->get_loc()->get_code_area());
        if (!ts) {
            std::cerr << "** ERROR: Time Series not found for: " << Var << " " << 0 << " " << v->get_loc()->get_code_area();
            exit (1);
        }
       // return ts->evaluateThresholds(tstep % 365);
         return ts->evaluateThresholds(tstep );
    }
};

}
}

#endif // TIMESERIESEVALUATOR_H
