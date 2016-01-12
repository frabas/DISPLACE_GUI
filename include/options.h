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

template <typename OPT, int N>
class Option
{
public:
    Option() {
        for (int i = 0; i < N; ++i)
            mOptions[i] = false;
    }

    void setOption (OPT option) {
        mOptions[option] = true;
    }

    void setOption (std::string option) {
        Iterator it = mMap.find(option);
        if (it != mMap.end())
            setOption(it->second);
    }

    bool option(OPT option) const {
        return mOptions[option];
    }

    std::string toString() const {
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

protected:
    bool mOptions[N];
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
    LTMP,
    FMSY,
    area_closure,
    fuelprice_plus20percent,
    reduced_speed_10percent,
    reduced_speed_20percent,
    reduced_speed_30percent,
    closer_grounds,
    closer_port,
    area_closure_fuelprice_plus20percent,
    increased_catch_power_5percent,
    focus_on_high_profit_grounds_area_closure,
    focus_on_high_previous_cpue,
    shared_harbour_knowledge,
    twoFoldTACconstr,
    fishing_credits,

    Dyn_Alloc_last
};

enum Pop_Sce {
    pop_baseline,
    use_SMS,
    use_SMS_single,
    with_monthly_redistribution,
    white_noise_on_avai,
    with_stochast_oth_land,
    stop_mig_35065,

    Pop_Sce_last
};

} // namespace

class DynAllocOptions : public Option<Options::Dyn_Alloc,  Options::Dyn_Alloc_last>
{
public:
    DynAllocOptions();
};

class PopSceOptions : public Option<Options::Pop_Sce, Options::Pop_Sce_last>
{
public:
    PopSceOptions();
};

#endif // OPTIONS_H
