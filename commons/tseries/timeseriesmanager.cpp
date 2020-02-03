// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "timeseriesmanager.h"

using namespace displace::simulation;

const int TimeSeriesManager::ALL_ZONES = 0;

TimeSeriesManager::TimeSeriesManager()
{
    for (int i = 0; i < VariablesLast; ++i)
        mData.push_back(new ZoneContainer());
}

void TimeSeriesManager::addTimeSerie(TimeSeriesManager::Variables var, int zone, int adim, std::shared_ptr<TimeSeries> ts)
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

std::shared_ptr<TimeSeries> TimeSeriesManager::getTimeSeries(TimeSeriesManager::Variables var, int zone, int adim)
{
    ZoneContainer *zonec = mData.at(var);

    ZoneContainer::iterator it = zonec->find(zone);
    if (it == zonec->end()) {
        // "all area" is supposed to be the 0 in code_area
        it = zonec->find(ALL_ZONES);
        if (it == zonec->end())
            return std::shared_ptr<TimeSeries>();
    }

    ADimContainer::iterator adcit = (it->second)->find(adim);
    if (adcit == (it->second)->end()) {
        // not sure if that should work
        adcit = (it->second)->find(0);
        if (adcit == (it->second)->end())
        // -- //
            return std::shared_ptr<TimeSeries>();
    }
    return adcit->second;
}

