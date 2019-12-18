// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef SCENARIO_H
#define SCENARIO_H

#include <tseries/timeseriesmanager.h>
#include <string>

#include <boost/lexical_cast.hpp>
#include <memory>

namespace displace {
namespace simulation {

/** \brief a class to manage the simulation scenario
 *
 * It contains all data common to the whole simulation, including, for example, Time Series and global status.
 * \note Since there's no a "simulation" class to contain all the data structures, the object is managed as a
 * monostate/singleton. This avoids the use of other globals, as it is planned to remove all global variables and
 * manage all the data using a simulation Object.
 * So the interface is subject to change (at least the accessing functions).
 */
class  Simulation
{
private:
    Simulation();
    ~Simulation(); // Avoids destroying. Not implemented.

    static Simulation *mInstance;
public:
    static Simulation *instance();

    bool loadTimeSeries(std::string path, std::string tsscenario);
    TimeSeries *getTimeSeries (TimeSeriesManager::Variables var, int zone, int adim);

private:
    std::shared_ptr<TimeSeriesManager> mTsManager;

    bool readTsFile(std::string filename);
};

} } // ns displace::simulation


#endif // SCENARIO_H
