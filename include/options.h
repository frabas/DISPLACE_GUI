/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014, 2015 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <string>
#include <vector>

#include <commons_global.h>

template <typename OPT, int N, typename TYPE>
class Option
{
public:
    Option() {}

    void setOption (OPT option, TYPE value) {
        mOptions[option] = value;
    }

    void setOption (std::string option, TYPE value) {
        Iterator it = mMap.find(option);
        if (it != mMap.end())
            setOption(it->second, value);
    }

    constexpr int n() const { return N; }

    const TYPE &option(OPT option) const {
        return mOptions[option];
    }

    template <typename _CTYPE = TYPE>
    typename std::enable_if<std::is_same<_CTYPE, bool>::value, std::string>::type
    toString() const {
        std::string ret;
        ConstIterator it = mMap.begin();
        while (it != mMap.end()) {
            if (option(it->second)) {
                ret += it->first;
                ret += " ";
            }
            ++it;
        }

        return ret;
    }

    friend bool operator == (const Option<OPT,N,TYPE> &o1, const Option<OPT,N,TYPE> &o2) {
        for (int i = 0; i < N; ++i) {
            if (o1.mOptions[i] != o2.mOptions[i])
                return false;
        }
        return true;
    }

protected:
    TYPE mOptions[N];
    typedef std::map<std::string, OPT> Map;
    typedef typename Map::iterator Iterator;
    typedef typename Map::const_iterator ConstIterator;
    Map mMap;
};

namespace Options {

enum Dyn_Alloc {
    alloc_baseline,
    focus_on_high_profit_grounds,
    TACs,
    discard_ban,
    groupedTACs,
    stopGoingFishingOnFirstChokedStock,
    LTMP,
    FMSY,
    FMSYrange,
    //area_closure, // [DEPRECATED]
    area_monthly_closure,
    closed_to_others_as_well,
    fuelprice_plus20percent,
    reduced_speed_10percent,
    reduced_speed_20percent,
    reduced_speed_30percent,
    gear_fuel_efficiency_gain_30percent,
    closer_grounds,
    closer_port,
    focus_on_high_previous_cpue,
    shared_harbour_knowledge,
    twoFoldTACconstr,
    experiencedCPUEsPerMet,
    experiencedCPUEsPerYearQuarter,
    fishing_credits,
    promote_high_tariffs,
    TechCreeping3Per,
    ExitVessels10Per,
    halfShippingDensity,
    DEBUG_TAC,
    EffortMinControl,
    EffortMaxControl,
    averageTariffsPerRectangle,
    averageCPUEsPerRectangle,
    otherLandPerMetPerPop,
    otherLandAsMultiplierOnSp,
    doNotExportDiscardsInLogbooks,
    PickUpTheMostFrequentGroundEachTime,

    Dyn_Alloc_last
};

enum Pop_Sce {
    pop_baseline,
    use_SMS,
    use_SMS_single,
    lgnbCoupling,
    biolsceOnStaticAvai,
    diffusePopN,
    avai_shuffler_on,
    avai_updater_on,
    white_noise_on_avai_deprecated,
    with_stochast_oth_land,
    stop_mig_35065,
    includeForcingLayers,
    diffuseBenthos,
    modelBenthosInN,
    modelBenthosInLongevity,
    modeldirectKillingOnBenthos,
    modelResuspensionEffectOnBenthos,
    modelShippingOnBenthos,
    sizeSpectra,
    stochasticGrowth,
    massiveMortalityEvent20perY1,
    massiveMortalityEvent20perY5,
    diffuseNutrients,
    noHyperstability,
  
    Pop_Sce_last
};

enum Closure_Opt {
    banned_metiers,

    Closure_Opt_last
};

} // namespace

class  DynAllocOptions : public Option<Options::Dyn_Alloc,  Options::Dyn_Alloc_last, bool>
{
public:
    DynAllocOptions();
};

class  PopSceOptions : public Option<Options::Pop_Sce, Options::Pop_Sce_last, bool>
{
public:
    PopSceOptions();
};

class  ClosureOptions : public Option<Options::Closure_Opt, Options::Closure_Opt_last, std::vector<int>> {
    std::vector<bool> mBannedMetiers;
public:
    void update();

    bool isMetierBanned(int metier) const {
        if (metier < (int)mBannedMetiers.size())
            return mBannedMetiers[metier];
        return false;
    }
};

#endif // OPTIONS_H
