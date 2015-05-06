#ifndef DTREEVARIABLES_H
#define DTREEVARIABLES_H

#include <string>
#include <vector>

namespace dtree {

enum Variable {
    VarUndefined = -1,
    arrived_on_the_ground,
    bycatch_risk,
    catch_volume,
    distance_to_port,
    feeling_for_higher_catches_elsewhere,
    fish_detection_with_echosounder,
    fish_price,
    fish_price_is,
    high_potential_catch,
    last_trip_on_the_ground,
    lastTripProfitIs,
    lastTripRevenueIs,
    remaining_quota_is,
    suitable_bottom_detection,
    weather_is,

    VarLeaf,     // this is a leaf node
    // keep this last
    VarLast
};

class VariableNames {
public:
    static const char *variableName(Variable);
    static Variable variableCode (const std::string &name);

#if ((__cplusplus >= 201103L))
    static int variableBinCount(Variable var) { return bins[var].size(); }
    static const char *variableBin(Variable var, int ndx) { return bins[var][ndx].c_str(); }
#endif

private:
    static const char *const names[];

#if ((__cplusplus >= 201103L))
    static const std::vector<std::string> bins[];
#endif
};

}

#endif // DTREEVARIABLES_H
