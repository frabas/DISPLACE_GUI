#ifndef TIMESERIESMANAGER_H
#define TIMESERIESMANAGER_H

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace displace {
namespace simulation {

class TimeSeries;

class TimeSeriesManager
{
public:
    enum Variables { Fishprice = 0, Fuelprice, Weather,
                     VariablesLast }; // keep this at the end

    TimeSeriesManager();

    void addTimeSerie (Variables var, int zone, int adim, boost::shared_ptr<TimeSeries> ts);

    static const int ALL_ZONES;

private:
    typedef std::map<int, boost::shared_ptr<TimeSeries> > ADimContainer;
    typedef std::map<int, ADimContainer *> ZoneContainer;
    typedef std::vector<ZoneContainer *> TimeSeriesContainer;

    TimeSeriesContainer mData;
};

} } // ns displace::simulation

#endif // TIMESERIESMANAGER_H
