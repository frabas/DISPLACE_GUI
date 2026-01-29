#ifndef VESSELDEBUGHELPER_H
#define VESSELDEBUGHELPER_H

#include "Vessel.h"          // <-- must be *before* any use of Vessel

#include <vector>
#include <iostream>
#include <cmath>

// Forward‑declare the classes you need to capture state from.
class Vessel;
class Population;
class Node;
class Benthos;

namespace VesselDebug
{
    struct VesselState
    {
        // ---- scalar aggregates -------------------------------------------------
        double cumcatches = 0.0;
        double cumdiscards = 0.0;
        double cumeffort = 0.0;          // if you track effort

        // ---- per‑ground vectors -----------------------------------------------
        std::vector<double> cumcatch_fgrounds;
        std::vector<double> cumdiscard_fgrounds;
        std::vector<double> experienced_bycatch_prop_on_fgrounds;
        std::vector<double> experienced_avoided_stks_bycatch_prop_on_fgrounds;

        // ---- per‑population matrices ------------------------------------------
        std::vector<std::vector<double>> catch_pop_at_szgroup;
        std::vector<std::vector<double>> discards_pop_at_szgroup;
        std::vector<std::vector<double>> ping_catch_pop_at_szgroup;


    };

    VesselState capture_state(const Vessel& v);
    void        restore_state(Vessel& v, const VesselState& s);
    void        report_diff(const VesselState& a,
        const VesselState& b,
        std::ostream&,
        double absTol,
        double relTol);


} // namespace VesselDebug

#endif // VESSELDEBUGHELPER_H


