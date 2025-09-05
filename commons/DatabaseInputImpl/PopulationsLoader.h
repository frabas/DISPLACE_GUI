
#ifndef DISPLACE_POPULATIONSLOADER_H
#define DISPLACE_POPULATIONSLOADER_H


#include "myutils.h"
#include "utils/spimpl.h"
#include "Node.h"

namespace msqlitecpp { namespace v2 { class Storage; } }

class PopulationsLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    explicit PopulationsLoader(msqlitecpp::v2::Storage &db);

    std::vector<std::string> getListOfAllPopulationsName();

    struct PopulationData {

        int a_name;
        string a_pop_name;
        double avai0_beta;
        double avai2_beta;
        double avai3_beta;
        double avai5_beta;
        double avai7_beta;
        std::vector<int> selected_szgroups;
        std::vector<double> init_tot_N_at_szgroup;
        std::vector<double> init_prop_migrants_in_tot_N_at_szgroup;
        std::vector<double> init_fecundity_at_szgroup;
        std::vector<double> init_weight_at_szgroup;
        std::vector<int> init_comcat_at_szgroup;
        std::vector<double> init_maturity_at_szgroup;
        std::vector<double> init_M_at_szgroup;
        std::vector<double> init_proprecru_at_szgroup;
        std::vector<double> param_sr;
        std::vector<double> selectivity_per_stock_ogives_for_oth_land;
        std::multimap<types::NodeId, double> full_spatial_availability;
        std::multimap<types::NodeId, double> field_of_coeff_diffusion_this_pop;
        std::map<int, int> nbhours_for_distance_internodes_this_pop;
        std::map<types::NodeId, double> oth_land;
        vector<map<types::NodeId, double> > oth_land_map_per_met;
        std::multimap<int, double> overall_migration_fluxes;
        std::map<string, double> relative_stability_key;
        std::map<int, double> percent_tac_per_vessel_length_class;    
        std::map<int, double> percent_tac_cumul_over_months_key;
        std::vector<std::vector<double> > percent_szgroup_per_age_matrix;
        std::vector<std::vector<double> > percent_age_per_szgroup_matrix;
        std::vector<std::vector<double> > growth_transition_matrix;
        std::vector<Node *> nodes;

        // F min age, F max age, LTMP ftarget, Fpercent, TACpercent, Btrigger,
        // F-MSY value, F-MSY-low, F-MSY-up
        std::vector<double> fbar_ages_min_max;
        std::vector<double> init_tac;
        double tac_percent_simulated;
        double hyperstability_param;
        double a_calib_cpue_multiplier=1;
        double a_calib_weight_at_szgroup=1;
    };

    std::vector<PopulationData> loadPopulationBaseData(int period);

    std::shared_ptr<PopulationData> getPopulationData(std::string populationname, int period);

};


#endif //DISPLACE_POPULATIONSLOADER_H
