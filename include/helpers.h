// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2026 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef HELPERS_H
#define HELPERS_H

#include <commons_global.h>

/* Hint: in Qt Creator you can fix the dout << "blablah"; with dout(cout<<"blablah"); using find/replace regexp
 * find: cout(.*);  replace: dout(cout \1);
 * */

extern int COMMONSSHARED_EXPORT verbosity;

#define outv(l,x) { if (verbosity >= l) { x; } }

#define dout(x) outv(3,x)
#define outc(x) outv(2,x)
#define tout(x) outv(1,x)

#define UNUSED(expr) do { (void)(expr); } while (0)


#include <map>
#include <string>
#include <stdexcept>
#include <iostream>


namespace util {

    /**
     * @brief Return the value associated with `key` or `fallback` if the key
     *        does not exist (or the map is empty).
     *
     * The function is deliberately *inline* and *constexpr*?friendly so the
     * compiler can optimise it away when the map is known to be non?empty.
     *
     * @tparam MapT   map type (must support `find` and `operator[]`/`at`).
     * @param  m      the map to query.
     * @param  key    the lookup key.
     * @param  fallback value to return when the key is absent.
     * @param  fatal  if true, throw std::runtime_error when the key is missing.
     * @return the mapped value or `fallback`.
     */
    template<class MapT, class KeyT, class ValueT>
    inline ValueT safe_lookup(const MapT& m,
        const KeyT& key,
        const ValueT& fallback = ValueT{},
        bool fatal = false)
    {
        if (m.empty()) {
            if (fatal) {
                std::ostringstream oss;
                oss << "safe_lookup: map is empty (requested key = '" << key << "')";
                throw std::runtime_error(oss.str());
            }
            // Log once per run (optional)
            static bool warned = false;
            if (!warned) {
                std::cerr << "[WARN] safe_lookup: map is empty (key = '" << key << "')\n";
                warned = true;
            }
            return fallback;
        }

        auto it = m.find(key);
        if (it != m.end())
            return it->second;

        if (fatal) {
            std::ostringstream oss;
            oss << "safe_lookup: key '" << key << "' not found in map (size = "
                << m.size() << ")";
            throw std::runtime_error(oss.str());
        }

        return fallback;
    }
} // namespace util





#endif // HELPERS_H
