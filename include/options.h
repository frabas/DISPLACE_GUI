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
    LTMP,
    FMSY,
    area_closure,
    area_closure_netters,
    area_closure_trawlers,
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
    white_noise_on_avai,
    with_stochast_oth_land,
    stop_mig_35065,

    Pop_Sce_last
};

enum Closure_Opt {
    banned_metiers,

    Closure_Opt_last
};

} // namespace

class DynAllocOptions : public Option<Options::Dyn_Alloc,  Options::Dyn_Alloc_last, bool>
{
public:
    DynAllocOptions();
};

class PopSceOptions : public Option<Options::Pop_Sce, Options::Pop_Sce_last, bool>
{
public:
    PopSceOptions();
};

class ClosureOptions : public Option<Options::Closure_Opt, Options::Closure_Opt_last, std::vector<int>> {
};

#endif // OPTIONS_H
