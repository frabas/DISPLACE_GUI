#include "timeseriesmanager.h"

using namespace displace::simulation;

const int TimeSeriesManager::ALL_ZONES = -1;

TimeSeriesManager::TimeSeriesManager()
{
    for (int i = 0; i < VariablesLast; ++i)
        mData.push_back(new ZoneContainer());
}

void TimeSeriesManager::addTimeSerie(TimeSeriesManager::Variables var, int zone, int adim, boost::shared_ptr<TimeSeries> ts)
{
    ZoneContainer *zonec = mData.at(var);

    ADimContainer *adimc;
    ZoneContainer::iterator it = zonec->find(zone);
    if (it == zonec->end()) {
        adimc = new ADimContainer();
        zonec->insert(std::make_pair(zone, adimc));
    } else {
        adimc = it->second;
    }

    ADimContainer::iterator adcit = adimc->find(adim);
    if (adcit != adimc->end()) {
        adimc->erase(adcit);
    }

    adimc->insert(std::make_pair(adim, ts));
}

