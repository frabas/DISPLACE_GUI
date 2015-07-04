#include "timeseries.h"

using namespace displace::simulation;

TimeSeries::TimeSeries()
{

}

void TimeSeries::setThreshold(int idx, double value)
{
    while ((int)mThresholds.size() <= idx)
        mThresholds.push_back(0);

    mThresholds.at(idx) = value;
}

