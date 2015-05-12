#ifndef DTREEVARIABLES_H
#define DTREEVARIABLES_H

#include <string>
#include <vector>

namespace dtree {

enum Variable {
    VarUndefined = -1,
//    vesselMetierIs,      // goFishing
    vesselSizeIs,             // goFishing
    lastTripProfitIs,          // goFishing
    lastTripRevenueIs,         // goFishing
    windSpeedIs,                // goFishing
    fishPriceTargetStockIs,      // goFishing
    fuelPriceIs,                  // goFishing
    remainingQuotaTargetStockIs,    // goFishing
    todayIs,                      // goFishing

    highPotentialCatch,          // ChooseGround
    knowledgeOfThisGround,          // ChooseGround
    lastTripThisGroundWas,          // ChooseGround
    riskOfBycatchIs,          // ChooseGround


    arrivedOnGroundIs,    // StartFishing
    detectedWithEchosounderIs,    // StartFishing
    suitableBottomTypeIs,   // StartFishing
//    riskOfBycatchIs,          // ChooseGround and StartFishing
    otherVesselFishingHere,          // StartFishing

    FeelingForCatchingElsewhere,  // ChangeGround
    otherVesselFishingElsewhere,  // ChangeGround

    catchVolumeIs, // StopFishing
    fuelTankIs, // StopFishing
    endOfTheDayIs, // StopFishing
    nbOfDaysAtSeaSoFarIs, // StopFishing
    weatherIs, // StopFishing

    distanceToPortIs,   // ChoosePort
    fuelPriceInPortIs,     // ChoosePort
    fishPriceThisStockInPortIs,   // ChoosePort
    portNationalityIs,   // ChoosePort

    VarLeaf,     // this is a leaf node
    // keep this last
    VarLast
};

class VariableNames {
public:
    static const int VERSION;

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
