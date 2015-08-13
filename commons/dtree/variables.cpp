#include <dtree/variables.h>

using namespace dtree;

const int VariableNames::VERSION = 6;

/** \brief The names if the variables, as used in the decision tree files.
 * When changing this array, changes must account relative changes to VariableNames::bins and Variable .
 * \note The name must not contain spaces or other character used as separator by the parsers.
 * */
const char *const VariableNames::names[] = {
    "vesselMetierIs",      // goFishing
    "vesselSizeIs",             // goFishing
    "lastTripProfitIs",          // goFishing
    "lastTripRevenueIs",         // goFishing
    "windSpeedIs",                // goFishing
    "fishPriceTargetStockIs",      // goFishing
    "fuelPriceIs",                  // goFishing
    "remainingQuotaTargetStockIs",     // goFishing
    "todayIs",                      // goFishing

    "highPotentialCatch",          // ChooseGround
    "knowledgeOfThisGround",          // ChooseGround
    "lastTripThisGroundWas",          // ChooseGround
    "riskOfBycatchIs",          // ChooseGround


    "arrivedOnGroundIs",    // StartFishing
    "detectedWithEchosounderIs",    // StartFishing
    "suitableBottomTypeIs",   // StartFishing
//    "riskOfBycatchIs",          // ChooseGround and StartFishing
    "otherVesselFishingHere",          // StartFishing

    "FeelingForCatchingElsewhere",  // ChangeGround
    "otherVesselFishingElsewhere",  // ChangeGround

    "catchVolumeIs", // StopFishing
    "fuelTankIs", // StopFishing
    "endOfTheDayIs", // StopFishing
    "nbOfDaysAtSeaSoFarIs", // StopFishing
    "weatherIs", // StopFishing

    "distanceToPortIs",   // ChoosePort
    "fuelPriceInPortIs",     // ChoosePort
    "fishPriceThisStockInPortIs",   // ChoosePort
    "portNationalityIs",   // ChoosePort

    "probability",
    // keep this last
    0
};

const char *const VariableNames::bins[][40] = {
    { "0", "1","2","3","4","5","6","7","8","9",
      "10", "11","12","13","14","15","16","17","18","19",
      "20", "21","22","23","24", 0  },
    { "u15m", "15-18m", "18-24m", "24-40m", "o40m", 0},    // given by length_class in Vessel::Vessel()
    { "below av.", "above av.", 0 },   //lastTripProfitIs
    {  "below av.", "above av.",0 },      //lastTripRevenueIs
    { "low", "medium", "large",0 }, // or { "<5knots", "5-10knots", ">10knots" },
    { "low","medium", "high",0 },
    { "low", "medium", "high",0 },  // fishPriceTargetStockIs a difficult one which requires an extra info to link metier names to targetted stock(s)
    { "low", "high",0 },
    { "ongoing", "ending",0 },


    { "not expected", "expected",0 },
    { "bad", "good",0 },
    { "bad", "good",0 },
    { "low", "high",0 },

    { "false", "true",0 },
    { "false", "true",0 },
    { "false", "true",0 },
//    { "low", "high",0 },
    { "no", "yes",0 },

    { "after 3 h", "after 6 h",0 },
    { "no", "yes",0 },

    { "still ok", "fulfilled",0 },
    { "still ok", "low",0 },
    { "true", "false",0 },
    { ">5", ">14",0 },  //or  {"short", "long" },
    { "bad", "good",0 },

    { "<10nm", "<100nm", ">100nm",0 },  // or { "near", "not that far", "far" },
    { "low", "high",0 },
    { "low", "high",0 },
    { "not mine", "mine",0 },

    { 0 },  // probability
    // keep this last
};

const char *VariableNames::variableName(Variable id)
{
    if ( id < 0 || id >= VarLast)
        return 0;

    return names[id];
}

Variable VariableNames::variableCode(const std::string &name)
{
    int i = 0;
    while (names[i] != 0) {
        if (names[i] == name)
            return static_cast<Variable>(i);
        ++i;
    }

    return VarUndefined;
}

int VariableNames::variableBinCount(Variable var)
{
    int i = 0;
    for (i = 0; bins[var][i] != 0; ++i);
    return i;
}
