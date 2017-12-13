// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <dtree/variables.h>

using namespace dtree;

const int VariableNames::VERSION = 7;

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
    "todayIs",                      // goFishing
    "monthIs",                      // goFishing
    "stockTargetIs",                      // goFishing
    "riskOfBycatchAvoidedStksNowIs", // goFishing
    "quotaLeftOnAvoidedStksNowIs", // goFishing



    "smartCatch",                  // ChooseGround
    "highPotentialCatch",          // ChooseGround
    "notThatFar",                  // ChooseGround
    "knowledgeOfThisGround",       // ChooseGround
    "riskOfBycatchAvoidedStksThisGroundIs",  // ChooseGround related to experienced_avoided_stks_bycatch_prop_on_fgrounds
    "riskOfBycatchAllStksThisGroundIs",     // ChooseGround related to experienced_bycatch_prop_on_fgrounds
    "individualQuotaLeftOnAvoidedStksIs",    // ChooseGround
    "globalQuotaLeftOnAvoidedStksIs",    // ChooseGround
    "saveFuel",                    // ChooseGround
    "isInAreaClosure",             // ChooseGround


    "arrivedOnGroundIs",    // StartFishing
    "detectedWithEchosounderIs",    // StartFishing
    "suitableBottomTypeIs",   // StartFishing
    "riskOfBycatchAvoidedStksHereIs", // StartFishing
    "quotaLeftOnAvoidedStksHereIs",   // StartFishing
    "otherVesselFishingHere",          // StartFishing

    "FeelingForCatchingElsewhere",  // ChangeGround
    "otherVesselFishingElsewhere",  // ChangeGround

    "catchVolumeIs", // StopFishing
    "fuelTankIs", // StopFishing
    "endOfTheDayIs", // StopFishing
    "nbOfDaysAtSeaSoFarIs", // StopFishing
//  "windSpeedIs", // goFishing and StopFishing

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
    { "low", "medium", "high",0 },  // fishPriceTargetStockIs a difficult one which requires an extra info to link metier names to targeted stock(s)
    { "week start", "week ending",0 },
    { "Jan", "Feb","Mar", "Apr","May", "Jun","Jul", "Aug","Sep", "Oct","Nov", "Dec", 0 }, // current_month
    { "0", "1","2","3","4","5","6","7","8","9",
      "10", "11","12","13","14","15","16","17","18","19",
      "20", "21","22","23","24", "25","26", "27","28","29","30", "31","32", "33","34", "35","36", 0  },
    { "<20% bycatch", ">20%",0 },
    { ">10% quota left", "<10%",0 },


    { "no", "yes",0 },
    { "no", "yes",0 },
    { "no", "yes",0 },
    { "no", "yes",0 },
    { "<20% bycatch", ">20%",0 },
    { "<20% bycatch", ">20%",0 },
    { ">10% quota left", "<10%",0 },
    { ">10% quota left", "<10%",0 },
    { "no", "yes",0 },
    { "no", "yes",0 },

    { "false", "true",0 },
    { "false", "true",0 },
    { "false", "true",0 },
    { "<20% bycatch", ">20%",0 },
    { ">10% quota left", "<10%",0 },
    { "no", "yes",0 },

    { "after 3 h", "after 6 h",0 },
    { "no", "yes",0 },

    { "still ok", "fulfilled",0 },
    { "still ok", "low",0 },
    { "true", "false",0 },
    { "<5days", ">5days",0 },  //or  {"short", "long" },
//    { "low", "medium", "large",0 }, // or { "<5knots", "5-10knots", ">10knots" },


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
