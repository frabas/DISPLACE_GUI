#ifndef DTREEVARIABLES_H
#define DTREEVARIABLES_H

#include <string>
#include <vector>

namespace dtree {

enum Variable {
    VarUndefined = -1,
    vesselMetierIs,      // goFishing
    vesselSizeIs,             // goFishing
    lastTripProfitIs,          // goFishing
    lastTripRevenueIs,         // goFishing
    windSpeedIs,                // goFishing
    fishPriceTargetStockIs,      // goFishing
    fuelPriceIs,                  // goFishing
    todayIs,                      // goFishing

    smartCatch,                  // ChooseGround
    highPotentialCatch,          // ChooseGround
    notThatFar,                  // ChooseGround
    knowledgeOfThisGround,       // ChooseGround
    lastTripThisGroundWas,       // ChooseGround
    riskOfBycatchIs,             // ChooseGround
    saveFuel,                    // ChooseGround
    isInAreaClosure,          // ChooseGround


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
//    windSpeedIs, // goFishing and StopFishing

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

    static int variableBinCount(Variable var);
    static const char *variableBin(Variable var, int ndx) { return bins[var][ndx]; }

private:
    static const char *const names[];

    static const char *const bins[][40];
};

}

#endif // DTREEVARIABLES_H
