#include "options.h"
using namespace Options;

#define O(MAP,NAME) MAP.insert(std::make_pair(#NAME,NAME))
#define M(MAP,NAME,ENUM) MAP.insert(std::make_pair(NAME,ENUM))

DynAllocOptions::DynAllocOptions()
{
    M(mMap,"baseline", alloc_baseline);
    O(mMap,focus_on_high_profit_grounds);
    O(mMap,TACs);
    O(mMap,LTMP);
    O(mMap,FMSY);
    O(mMap,area_closure);
    O(mMap,fuelprice_plus20percent);
    O(mMap,reduced_speed_10percent);
    O(mMap,reduced_speed_20percent);
    O(mMap,reduced_speed_30percent);
    O(mMap,closer_grounds);
    O(mMap,closer_port);
    O(mMap,area_closure_fuelprice_plus20percent);
    O(mMap,increased_catch_power_5percent);
    O(mMap,focus_on_high_profit_grounds_area_closure);
    O(mMap,focus_on_high_previous_cpue);
    O(mMap,shared_harbour_knowledge);
    O(mMap,twoFoldTACconstr);

}


PopSceOptions::PopSceOptions()
{
    M(mMap,"baseline", pop_baseline);
    O(mMap,use_SMS);
    O(mMap,use_SMS_single);
    O(mMap,with_monthly_redistribution);
    O(mMap,white_noise_on_avai);
    O(mMap,with_stochast_oth_land);
    O(mMap,stop_mig_35065);
}
