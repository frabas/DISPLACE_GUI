#include "options.h"
using namespace Options;

#define O(MAP,NAME) MAP.insert(std::make_pair(#NAME,NAME))

DynAllocOptions::DynAllocOptions()
{
    O(mMap,baseline);
    O(mMap,focus_on_high_profit_grounds);
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
    O(mMap,loser_grounds);
}
