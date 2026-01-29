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

#include "VesselDebugHelper.h"
#include "Vessel.h"          // full definition needed for the bodies
#include <iomanip>           // for pretty printing in report_diff


namespace VesselDebug
{
    
   

    // -----------------------------------------------------------------
    // 1️⃣  Capture the current state of a vessel
    // -----------------------------------------------------------------
     VesselState capture_state(const Vessel& v)
    {
        VesselState s;
        // scalars
        s.cumcatches = v.get_cumcatches();   // you may need a simple getter
        s.cumdiscards = v.get_cumdiscards();
        s.cumeffort = v.get_timeatsea();    // add getter if you track effort

        // per‑ground vectors
        s.cumcatch_fgrounds = v.get_cumcatch_fgrounds();
        s.cumdiscard_fgrounds = v.get_cumdiscard_fgrounds();

        // per‑population matrices
        s.catch_pop_at_szgroup = v.get_catch_pop_at_szgroup();
        s.discards_pop_at_szgroup = v.get_discards_pop_at_szgroup();
        s.ping_catch_pop_at_szgroup = v.get_ping_catch_pop_at_szgroup();


        return s;
    }

    // -----------------------------------------------------------------
    // 2️⃣  Restore a previously saved state into a vessel
    // -----------------------------------------------------------------
     void restore_state(Vessel& v, const VesselState& s)
    {
        // scalars
        v.set_cumcatches(s.cumcatches);      // you may need simple setters
        v.set_cumdiscards(s.cumdiscards);
        v.set_timeatsea(s.cumeffort);

        // per‑ground vectors
        v.set_cumcatch_fgrounds(s.cumcatch_fgrounds);
        v.set_cumdiscard_fgrounds(s.cumdiscard_fgrounds);

        // per‑population matrices
        std::vector<std::vector<double>> catchPop = s.catch_pop_at_szgroup; // copy
        v.set_catch_pop_at_szgroup(catchPop);
        std::vector<std::vector<double>> discardPop = s.discards_pop_at_szgroup; // copy
        v.set_discards_pop_at_szgroup(discardPop);
        std::vector<std::vector<double>> PingCatchPop = s.ping_catch_pop_at_szgroup; // copy
        v.set_ping_catch_pop_at_szgroup(PingCatchPop);

    }

    // -----------------------------------------------------------------
    // 3️⃣  Very simple diff printer (writes to a file or stdout)
    // -----------------------------------------------------------------
     void report_diff(const VesselState& a,
        const VesselState& b,
        std::ostream& out,
        double absTol,
        double relTol)
    {
        auto cmp = [&](const char* name, double av, double bv)
        {
            double diff = std::abs(av - bv);
            double rel = (std::abs(bv) > 1e-12) ? diff / std::abs(bv) : diff;
            if (diff > absTol && rel > relTol)
                out << "  ✗ " << name << ": " << av << " vs " << bv
                << " (Δ=" << diff << ", rel=" << rel << ")\n";
        };

        // scalars
        cmp("cumcatches", a.cumcatches, b.cumcatches);
        cmp("cumdiscards", a.cumdiscards, b.cumdiscards);
        cmp("cumeffort", a.cumeffort, b.cumeffort);

        // vectors – compare element‑wise (stop after first few mismatches)
        auto vec_cmp = [&](const char* name,
            const std::vector<double>& va,
            const std::vector<double>& vb)
        {
            if (va.size() != vb.size())
            {
                out << "  ✗ " << name << ": size mismatch (" << va.size()
                    << " vs " << vb.size() << ")\n";
                return;
            }
            for (size_t i = 0; i < va.size(); ++i)
            {
                double diff = std::abs(va[i] - vb[i]);
                double rel = (std::abs(vb[i]) > 1e-12) ? diff / std::abs(vb[i]) : diff;
                if (diff > absTol && rel > relTol && i < 10)   // show first few
                    out << "  ✗ " << name << "[" << i << "] = "
                    << va[i] << " vs " << vb[i]
                    << " (Δ=" << diff << ", rel=" << rel << ")\n";
            }
        };

        vec_cmp("cumcatch_fgrounds", a.cumcatch_fgrounds, b.cumcatch_fgrounds);
        vec_cmp("cumdiscard_fgrounds", a.cumdiscard_fgrounds, b.cumdiscard_fgrounds);

        // matrices – flatten and reuse vec_cmp
        auto flatten = [](const std::vector<std::vector<double>>& mat)
        {
            std::vector<double> flat;
            for (const auto& row : mat) flat.insert(flat.end(), row.begin(), row.end());
            return flat;
        };
        vec_cmp("catch_pop_at_szgroup",
            flatten(a.catch_pop_at_szgroup),
            flatten(b.catch_pop_at_szgroup));
        vec_cmp("discards_pop_at_szgroup",
            flatten(a.discards_pop_at_szgroup),
            flatten(b.discards_pop_at_szgroup));
        vec_cmp("ping_catch_pop_at_szgroup",
            flatten(a.ping_catch_pop_at_szgroup),
            flatten(b.ping_catch_pop_at_szgroup));


    }

}