#ifndef SCENARIO_H
#define SCENARIO_H

#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

namespace displace {
namespace simulation {

class TimeSeriesManager;

/** \brief a class to manage the simulation scenario
 *
 * It contains all data common to the whole simulation, including, for example, Time Series and global status.
 * \note Since there's no a "simulation" class to contain all the data structures, the object is managed as a
 * monostate/singleton. This avoids the use of other globals, as it is planned to remove all global variables and
 * manage all the data using a simulation Object.
 * So the interface is subject to change (at least the accessing functions).
 */
class Simulation
{
private:
    Simulation();
    ~Simulation(); // Avoids destroying. Not implemented.

    static Simulation *mInstance;
public:
    static Simulation *instance();

    bool loadTimeSeries(std::string path, std::string tsscenario);

private:
    boost::shared_ptr<TimeSeriesManager> mTsManager;

    bool readTsFile(std::string filename) throw (boost::bad_lexical_cast);
};

} } // ns displace::simulation


#endif // SCENARIO_H
