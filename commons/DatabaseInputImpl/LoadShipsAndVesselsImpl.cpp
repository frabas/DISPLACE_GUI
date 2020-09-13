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
    double idx_vessel = -1;

    for (auto &vessel: allVessels) {
        // TODO: in the DB we just have opt1 as "period". We select month here. FIXME.
        auto vesselData = loader.getVesselData(vessel, month);
        auto &spe_harbours = vesselData->harbours;
        auto &spe_freq_harbours = vesselData->freq_harbours;

        // TODO: There's no a_location in the db. FIXME.
        types::NodeId start_harbour{0};
        if (!spe_harbours.empty()) {
            // need to convert in array, see myRutils.cpp
            auto one_harbour = do_sample(1, spe_harbours.size(), spe_harbours, spe_freq_harbours);
            start_harbour = one_harbour[0];
        } else {
            // if missing info for a given vessel for this quarter
            outc(cout << "no specified harbour in this quarter for this vessel..." << endl);
            // CAUTION: LIKE A MAGIC NUMBER HERE!!!
            // TODO I don't know what this does.
//            start_harbour = find_entries(loadedDataVessels.mmapsnparam1, loadedDataVessels.vectsparam1.at(0))[0];
//            spe_harbours.push_back(start_harbour);
//            spe_freq_harbours.push_back(1);
            outc(cout << "then take node: " << start_harbour << endl);
            std::ostringstream ss;
            ss << "This part was not implemented, please fix it: " << __FILE__ << " line " << __LINE__;
            throw std::logic_error(ss.str());
        }


        auto a_location = model().nodes().at(start_harbour.toIndex());
        
        auto& vessel_beta = vesselData->vessel_betas_per_pop;
        idx_vessel += idx_vessel;
        double nbpops = vessel_beta.size();
        double nbszgroups = 14; // caution: magic number


        auto v = new Vessel(
                a_location,        // a_location i.e start harbour node
                idx_vessel,
                vessel,         // name
                nbpops,
                nbszgroups,           
                vesselData->harbours,
                vesselData->fground,
                vesselData->fground_init,
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

