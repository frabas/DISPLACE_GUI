//
// Created by fuga on 01/06/2020.
//

#include "DatabaseModelLoader.h"
#include "Impl.h"
#include "SimModel.h"

#include "Vessel.h"
#include "VesselsLoader.h"

#include <msqlitecpp/v2/selectstatement.h>
#include <msqlitecpp/v2/fields.h>

namespace db = msqlitecpp::v2;


void DatabaseModelLoader::loadVessels(int year, int month, int quarter, int semester)
{
    VesselsLoader loader(*p->db);

    auto allVessels = loader.getListOfAllVessels();

    std::vector<Vessel *> vesselsToAdd;
    for (auto &vessel: allVessels) {
        // TODO: in the DB we just have opt1 as "period". We select month here. FIXME.
        auto vesselData = loader.getVesselData(vessel, month);

        // TODO: There's no a_location in the db. FIXME.
        auto v = new Vessel(
                nullptr,        // a_location. What is it?
                0,              // idx_vessel, where to find it?
                vessel,         // name
                0, 0,           // nb pops, nbszgrous,
                vesselData->harbours,
                vesselData->fground,
                std::vector<types::NodeId>(),   // FIXME ?
                //vesselData->fgroundFreq,
                vesselData->freq_harbours,
                vesselData->freq_fgrounds,
                vesselData->freq_fgrounds_init,
                vesselData->vessel_betas_per_pop,
                vesselData->percent_tac_per_pop,
                vesselData->possible_metiers,
                vesselData->freq_possible_metiers,
                vesselData->gshape_cpue_per_stk_on_nodes,
                vesselData->gscale_cpue_per_stk_on_nodes,
                vesselData->vid_is_active,
                vesselData->vid_is_part_of_ref_fleet,
                vesselData->speed,
                vesselData->fuelcons,
                vesselData->length,
                vesselData->KW,
                vesselData->carrycapacity,
                vesselData->tankcapacity,
                vesselData->nbfpingspertrip,
                vesselData->resttime_par1,
                vesselData->resttime_par2,
                vesselData->av_trip_duration,
                vesselData->mult_fuelcons_when_steaming,
                vesselData->mult_fuelcons_when_fishing,
                vesselData->mult_fuelcons_when_returning,
                vesselData->mult_fuelcons_when_inactive,
                vesselData->_firm_id,
                vesselData->cd,
                vesselData->this_vessel_nb_crew,
                vesselData->annual_other_income,
                vesselData->landing_costs_percent,
                vesselData->crewshare_and_unpaid_labour_costs_percent,
                vesselData->other_variable_costs_per_unit_effort,
                vesselData->annual_insurance_costs_per_crew,
                vesselData->standard_labour_hour_opportunity_costs,
                vesselData->standard_annual_full_time_employement_hours,
                vesselData->other_annual_fixed_costs,
                vesselData->vessel_value,
                vesselData->annual_depreciation_rate,
                vesselData->opportunity_interest_rate,
                vesselData->annual_discount_rate);

        vesselsToAdd.push_back(v);
    }

    model().setVessels(vesselsToAdd);
}

void DatabaseModelLoader::loadShips()
{

}

