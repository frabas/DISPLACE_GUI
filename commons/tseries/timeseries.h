#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <vector>
#include <cmath>

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

    int evaluateThresholds (int idx) {
        int i;
        for (i = 0; i < mThresholds.size(); ++i) {
            if (std::isnan(mThresholds[idx]) || mData.at(idx) < mThresholds[idx])
                return i;
        }
        return i;
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
