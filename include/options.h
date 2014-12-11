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
    baseline,
    focus_on_high_profit_grounds,
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
    loser_grounds,

    Dyn_Alloc_last
};

} // namespace

class DynAllocOptions : public Option<Options::Dyn_Alloc,  Options::Dyn_Alloc_last>
{
public:
    DynAllocOptions();
};

#endif // OPTIONS_H
