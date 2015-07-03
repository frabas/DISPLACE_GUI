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

boost::shared_ptr<TimeSeries> TimeSeriesManager::getTimeSeries(TimeSeriesManager::Variables var, int zone, int adim)
{
    ZoneContainer *zonec = mData.at(var);

    ZoneContainer::iterator it = zonec->find(zone);
    if (it == zonec->end()) {
        // try with "all area"
        it = zonec->find(ALL_ZONES);
        if (it == zonec->end())
            return boost::shared_ptr<TimeSeries>();
    }

    ADimContainer::iterator adcit = (it->second)->find(adim);
    if (adcit == (it->second)->end()) {
        // not sure if that should work
        adcit = (it->second)->find(0);
        if (adcit == (it->second)->end())
        // -- //
            return boost::shared_ptr<TimeSeries>();
    }
    return adcit->second;
}

