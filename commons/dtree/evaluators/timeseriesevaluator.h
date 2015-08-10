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

        int adim =0;

        // if no ts informed for v->get_loc()->get_code_area() then take code_area at 0


        // find out the targetted stocks and take the more priced
       //if(Var=="fishPriceTargetStockIs"){
        multimap<int, int>  trgts =v->get_metier()->get_metier_target_stocks();
        vector <double> values;
        for (std::multimap<int,int>::iterator it=trgts.begin(); it!=trgts.end(); ++it)
           {
            adim=(*it).second;
            displace::simulation::TimeSeries *a_ts = displace::simulation::Simulation::instance()->getTimeSeries(Var,  v->get_loc()->get_code_area(), adim);
            values.push_back (a_ts->evaluateThresholds((tstep / 24) % 365));
           }
        return *(max_element(values.begin(), values.end()));
       //}


        displace::simulation::TimeSeries *ts = displace::simulation::Simulation::instance()->getTimeSeries(Var,  v->get_loc()->get_code_area(), adim);
        if (!ts) {
            std::cerr << "** ERROR: Time Series not found for: " << Var << " " << 0 << " " << v->get_loc()->get_code_area();
            exit (1);
        }
    cout << "ts evaluated at " << ts->evaluateThresholds((tstep / 24) % 365) << endl << " for this tstep";

        return ts->evaluateThresholds((tstep / 24) % 365);
    }
};

}
}

#endif // TIMESERIESEVALUATOR_H
