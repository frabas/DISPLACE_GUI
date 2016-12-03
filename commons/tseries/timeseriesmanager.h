#ifndef TIMESERIESMANAGER_H
#define TIMESERIESMANAGER_H

#include <commons_global.h>
#include <vector>
#include <map>
#include <memory>

namespace displace {
namespace simulation {

class TimeSeries;

class COMMONSSHARED_EXPORT TimeSeriesManager
{
public:
    enum Variables { Fishprice = 0, Fuelprice, WSpeed,
                     VariablesLast }; // keep this at the end

    TimeSeriesManager();

    void addTimeSerie (Variables var, int zone, int adim, std::shared_ptr<TimeSeries> ts);
    std::shared_ptr<TimeSeries> getTimeSeries (Variables var, int zone, int adim);

    static const int ALL_ZONES;

private:
    typedef std::map<int, std::shared_ptr<TimeSeries> > ADimContainer;
    typedef std::map<int, ADimContainer *> ZoneContainer;
    typedef std::vector<ZoneContainer *> TimeSeriesContainer;

    TimeSeriesContainer mData;
};

} } // ns displace::simulation

#endif // TIMESERIESMANAGER_H
