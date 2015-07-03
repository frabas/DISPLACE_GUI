#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <vector>

namespace displace {
namespace simulation {

class TimeSeries
{
public:
    TimeSeries();

    double getThreshold(int idx) const {
        return mThresholds.at(idx);
    }

    double getValue (int idx) const {
        return mData.at(idx);
    }

    void setThreshold (int idx, double value);
    void appendValue (double value) {
        mData.push_back(value);
    }

private:
    std::vector<double> mThresholds;
    std::vector<double> mData;
};


} }


#endif // TIMESERIES_H
