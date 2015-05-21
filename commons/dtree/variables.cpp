#include <dtree/variables.h>

using namespace dtree ;

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

#if ((__cplusplus >= 201103L))
const std::vector<std::string> VariableNames::bins[] = {
    { "0", "1","2","3","4","5","6","7","8","9",
      "10", "11","12","13","14","15","16","17","18","19",
      "20", "21","22","23","24","25","26","27","28","29"  },
    { "u15m", "15-18m", "18-24m", "24-40m", "o40m"},    // given by length_class in Vessel::Vessel()    or { "small", "medium", "large"}
    { "below av.", "above av." },   //lastTripProfitIs
    {  "below av.", "above av."},      //lastTripRevenueIs
    { "low", "medium", "large" }, // or { "<5knots", "5-10knots", ">10knots" },
    { "low", "high" },
    { "low", "high" },  // fishPriceTargetStockIs a difficult one which requires an extra info to link metier names to targetted stock(s)
    { "low", "high" },
    { "ongoing", "ending" },


    { "not expected", "expected" },
    { "bad", "good"},
    { "bad", "good"},
    { "low", "high" },

    { "false", "true" },
    { "false", "true" },
    { "false", "true" },
//    { "low", "high" },
    { "no", "yes" },

    { "after 3 h", "after 6 h" },
    { "no", "yes" },

    { "still ok", "fulfilled" },
    { "still ok", "low" },
    { "true", "false" },
    { ">5", ">14" },  //or  {"short", "long" },
    { "bad", "good" },

    { "<10nm", "<100nm", ">100nm" },  // or { "near", "not that far", "far" },
    { "low", "high" },
    { "low", "high" },
    { "not mine", "mine" },

    { },  // probability
    // keep this last
};
#endif

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
