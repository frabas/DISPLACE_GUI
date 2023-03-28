//
// Created by fuga on 01/06/2020.
//



#include "DatabaseModelLoader.h"
#include "Impl.h"
#include "SimModel.h"

#include "Population.h"

#include "PopulationsLoader.h"

#include <iostream>

#define NBSZGROUP 14
#define NBAGE 11                 // nb age classes max
#define SEL_NBSZGROUP 5             // DEPRECATED - according to the R glm on cpue (see R code)

bool DatabaseModelLoader::loadPopulations(int period)
{
    if (period == 1) {
        int nbmets = 100; // CAUTION: for now we don't care as a large value will be resized. but best to ultimately include it to config()
        for (unsigned int i = 0; i < model().nodes().size(); i++) {
            model().nodes().at(i)->init_Ns_pops_at_szgroup(model().config().nbpops, NBSZGROUP, nbmets);
            model().nodes().at(i)->init_avai_pops_at_selected_szgroup(model().config().nbpops, SEL_NBSZGROUP);
        }
    }

    PopulationsLoader loader(*p->db);
    auto pops = loader.loadPopulationBaseData(period);

    auto populations = vector<Population *>();
    std::transform(pops.begin(), pops.end(), std::back_inserter(populations),
                   [this](PopulationsLoader::PopulationData const &d) {
                       std::cout << "Create: " << d.a_name << " " << d.a_pop_name << "\n";
                       auto pop = new Population(
                               d.a_name,
                               d.a_pop_name,
                               d.avai0_beta, d.avai2_beta, d.avai3_beta, d.avai5_beta, d.avai7_beta,

                               std::move(d.selected_szgroups),
                               std::move(d.init_tot_N_at_szgroup),
                               std::move(d.init_prop_migrants_in_tot_N_at_szgroup),
                               std::move(d.init_fecundity_at_szgroup),
                               std::move(d.init_weight_at_szgroup),
                               std::move(d.init_comcat_at_szgroup),
                               std::move(d.init_maturity_at_szgroup),
                               std::move(d.init_M_at_szgroup),
                               std::move(d.init_proprecru_at_szgroup),

                               std::move(d.param_sr),
                               std::move(d.selectivity_per_stock_ogives_for_oth_land),
                           
                               std::move(d.full_spatial_availability),
                               std::move(d.field_of_coeff_diffusion_this_pop),
                               std::move(d.oth_land),
                               std::move(d.oth_land_map_per_met),
                               std::move(d.overall_migration_fluxes),
                               std::move(d.relative_stability_key),
                               std::move(d.percent_tac_per_vessel_length_class),
                               std::move(d.percent_tac_cumul_over_months_key),

                               std::move(d.percent_szgroup_per_age_matrix),
                               std::move(d.percent_age_per_szgroup_matrix),
                               std::move(d.growth_transition_matrix),

                               model().nodes(),
                               std::move(d.fbar_ages_min_max),
                               std::move(d.init_tac),

                               d.tac_percent_simulated,
                               d.hyperstability_param,
                               d.a_calib_cpue_multiplier,
                               d.a_calib_weight_at_szgroup
                       );

                       return pop;
                   });

    model().setPopulations(std::move(populations));
  
    std::cout << "Nodes: " << model().nodes().size() << "\n";
    return true;
}

