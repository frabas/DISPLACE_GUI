#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <vector>
#include <cmath>
#include <iostream>

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
        for (i = 0; i < (int)mThresholds.size(); ++i) {
         //std::cout << "in evaluateThresholds(), current data is " <<  mData.at(idx) << ", compared to threshold " << i << ": " << mThresholds[i] << std::endl;
            if (std::isnan(mThresholds[i]) || mData.at(idx) < mThresholds[i])
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
