// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "options.h"
using namespace Options;

#define O(MAP,NAME) MAP.insert(std::make_pair(#NAME,NAME))
#define M(MAP,NAME,ENUM) MAP.insert(std::make_pair(NAME,ENUM))

DynAllocOptions::DynAllocOptions()
{
    M(mMap,"baseline", alloc_baseline);
    O(mMap,focus_on_high_profit_grounds);
    O(mMap,TACs);
    O(mMap,groupedTACs);
    O(mMap,stopGoingFishingOnFirstChokedStock);
    O(mMap,discard_ban);
    O(mMap,LTMP);
    O(mMap,FMSY);
    O(mMap,FMSYrange);
    //M(mMap,"area_closure", area_closure); // quartely based [deprecated]
    M(mMap,"area_monthly_closure", area_monthly_closure);
    M(mMap,"closed_to_others_as_well", closed_to_others_as_well);
    O(mMap,fuelprice_plus20percent);
    O(mMap,fuelprice_plus50percent);
    O(mMap,fuelprice_plus100percent);
    O(mMap,fishprice_plus100percent);
    O(mMap,fishprice_plus700percent);
    O(mMap, fishprice_plus700percent);
    O(mMap, fishprice_plus800percent);
    O(mMap, fishprice_plus900percent);
    O(mMap, fishprice_plus1000percent);
    O(mMap, fishprice_plus1100percent);
    O(mMap, fishprice_plus1200percent);
    O(mMap, fishprice_plus1300percent);
    O(mMap, fishprice_plus1400percent);
    O(mMap, fishprice_plus1500percent);
    O(mMap,reduced_speed_10percent);
    O(mMap,reduced_speed_20percent);
    O(mMap,reduced_speed_30percent);
    O(mMap,multiOnFuelcons50percent);
    O(mMap,multiOnFuelcons100percent);
    O(mMap,gear_fuel_efficiency_gain_30percent);
    O(mMap,multiOnVesselValue50percent);
    O(mMap,multiOnVesselValue100percent);
    O(mMap,closer_grounds);
    O(mMap,closer_port);
    O(mMap,focus_on_high_previous_cpue);
    O(mMap,shared_harbour_knowledge);
    O(mMap,twoFoldTACconstr);
    O(mMap, multiOnF01);
    O(mMap, multiOnF02);
    O(mMap, multiOnF03);
    O(mMap, multiOnF04);
    O(mMap, multiOnF05);
    O(mMap, multiOnF06);
    O(mMap, multiOnF07);
    O(mMap, multiOnF08);
    O(mMap, multiOnF09);
    O(mMap, experiencedCPUEsPerMet);
    O(mMap, experiencedCPUEsPerYearQuarter);
    O(mMap,fishing_credits);
    O(mMap,promote_high_tariffs);
    O(mMap,TechCreeping3Per);
    O(mMap,ExitVessels10Per);
    O(mMap,halfShippingDensity);
    O(mMap,DEBUG_TAC);
    O(mMap,EffortMinControl);
    O(mMap,EffortMaxControl);
    O(mMap, averageTariffsPerRectangle);
    O(mMap, averageCPUEsPerRectangle);
    O(mMap, otherLandPerMetPerPop);
    O(mMap, otherLandAsMultiplierOnSp);
    O(mMap, doNotExportDiscardsInLogbooks);
    O(mMap, PickUpTheMostFrequentGroundEachTime);





    for (int i = 0; i < n(); ++i)
        mOptions[i] = false;
}


PopSceOptions::PopSceOptions()
{
    M(mMap,"baseline", pop_baseline);
    O(mMap,use_SMS);
    O(mMap,use_SMS_single);
    O(mMap,lgnbCoupling);
    O(mMap,biolsceOnStaticAvai);
    O(mMap,diffusePopN);
    O(mMap,avai_shuffler_on);
    O(mMap,avai_updater_on);
    O(mMap,with_stochast_oth_land);
    O(mMap,stop_mig_35065);
    O(mMap,includeForcingLayers);
    O(mMap,diffuseBenthos);
    O(mMap,modelBenthosInN);
    O(mMap,modelBenthosInLongevity);
    O(mMap,modeldirectKillingOnBenthos);
    O(mMap,modelShippingOnBenthos);
    O(mMap,modelResuspensionEffectOnBenthos);
    O(mMap,sizeSpectra);
    O(mMap, stochasticGrowth);
    O(mMap,massiveMortalityEvent20perY1);
    O(mMap, massiveMortalityEvent20perY5);
    O(mMap,diffuseNutrients);
    O(mMap,noHyperstability);
    O(mMap, static_avai_2);
    O(mMap, static_avai_3);
    O(mMap, static_avai_4);


    for (int i = 0; i < n(); ++i)
        mOptions[i] = false;
}

void ClosureOptions::update()
{
    auto &l = option(banned_metiers);
    for (auto &n : l) {
        while (mBannedMetiers.size() <= (size_t)n)
            mBannedMetiers.push_back(false);
        mBannedMetiers[n] = true;
    }
}
