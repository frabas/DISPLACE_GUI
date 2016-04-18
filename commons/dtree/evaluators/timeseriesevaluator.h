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

        displace::simulation::TimeSeries *ts = displace::simulation::Simulation::instance()->getTimeSeries(Var, v->get_loc()->get_code_area(),0 );
        if (!ts) {
            std::cerr << "** ERROR: Time Series not found for: " << Var << " " << 0 << " " << v->get_loc()->get_code_area();
            exit (1);
        }
        return ts->evaluateThresholds((tstep / 24) % 365);
    }
};

template <>
class TimeSeriesEvaluator<displace::simulation::TimeSeriesManager::Fishprice> : public ::dtree::StateEvaluator
{
public:
    TimeSeriesEvaluator() {}

    double evaluate(int tstep, Vessel *v) const {
        // NOTE: tstep must be converted to day!

        int adim =0;

        // find out the bunch of targeted stocks and decide from the currently more priced one among them
        vector<int>  trgts =v->get_metier()->get_metier_target_stocks();
        vector <double> values;

        for (size_t i=0; i<trgts.size(); i++)
        {
            adim = trgts.at(i);
            cout << v->get_name() << " look at the price of the target -> stock " << adim << endl;
            displace::simulation::TimeSeries *a_ts = displace::simulation::Simulation::instance()->getTimeSeries(displace::simulation::TimeSeriesManager::Fishprice,  v->get_loc()->get_code_area(), adim);
            values.push_back (a_ts->evaluateThresholds((tstep / 24) % 365));
        }
        return *(max_element(values.begin(), values.end()));
    }
};



}
}

#endif // TIMESERIESEVALUATOR_H
