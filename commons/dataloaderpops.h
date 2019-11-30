#ifndef DATALOADERPOPS_H
#define DATALOADERPOPS_H

#include<dataloader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <set>

#include "Node.h"

#include "helpers.h"
#include "options.h"

#include "msqlitecpp/v2/storage.h"


namespace sql = msqlitecpp::v2;
using namespace std;


class Node;

class COMMONSSHARED_EXPORT Dataloaderpops : public virtual Dataloader
{
public:
    Dataloaderpops();
    int gettype (void);
    int features(std::shared_ptr<msqlitecpp::v2::Storage> indb,
                 const string& folder_name_parameterization,
                  const string& inputfolder,
                  PopSceOptions& dyn_pop_sce,
                  DynAllocOptions& dyn_alloc_sce,
                  string &biolsce,
                  string &fleetsce,
                  ParamsForLoad &paramsForLoad,
                  LoadedData & loadedData);

};


void COMMONSSHARED_EXPORT  read_pop_names_in_string(map<int, string>& pop_names, string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_avai_betas(string a_semester, string szgroup, string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_init_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_prop_migrants_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_maturity_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_fecundity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_weight_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, int> COMMONSSHARED_EXPORT read_init_comcat_per_szgroup(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_init_M_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_init_proprecru_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce);
multimap<int, types::NodeId> COMMONSSHARED_EXPORT read_lst_idx_nodes_per_pop(string a_semester, string folder_name_parameterization, string inputfolder, string str_rand_avai_file);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_avai_szgroup_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string str_rand_avai_file, vector<string> type_of_avai_field_to_read);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_full_avai_szgroup_nodes_with_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string str_rand_avai_file, vector<string> type_of_avai_field_to_read);
multimap<types::NodeId, double> COMMONSSHARED_EXPORT read_field_of_coeff_diffusion_this_pop(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_growth_transition_matrix(int a_pop, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_percent_age_per_szgroup_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_percent_szgroup_per_age_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization, string inputfolder, string biolsce);
vector<double> COMMONSSHARED_EXPORT read_param_sr(int a_pop,  string folder_name_parameterization, string inputfolder, string biolsce);
vector<double> COMMONSSHARED_EXPORT read_fbar_ages_min_max_and_ftarget(int a_pop,  string folder_name_parameterization, string inputfolder, string biolsce);
vector<double> COMMONSSHARED_EXPORT read_initial_tac(int a_pop,  string folder_name_parameterization, string inputfolder);
map<int, int> COMMONSSHARED_EXPORT read_tac_percent_simulated(string folder_name_parameterization, string inputfolder);
map<int, double> COMMONSSHARED_EXPORT read_hyperstability_param(string folder_name_parameterization, string inputfolder);
map<types::NodeId, double> COMMONSSHARED_EXPORT read_oth_land_nodes_with_pop(string a_semester, string a_month, int a_pop, string folder_name_parameterization, string inputfolder, string fleetsce);
map<string, double> COMMONSSHARED_EXPORT read_relative_stability_keys(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder);
multimap<int, int> COMMONSSHARED_EXPORT read_selected_szgroups_per_pop(string folder_name_parameterization, string inputfolder);
multimap<int, double> COMMONSSHARED_EXPORT read_overall_migration_fluxes(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string biolsce);

// pop size spectra specific
multimap<int, double> COMMONSSHARED_EXPORT read_adults_diet_preference_per_stock_allstks(string folder_name_parameterization,  string inputfolder, string biolsce);
multimap<int, double> COMMONSSHARED_EXPORT read_juveniles_diet_preference_per_stock_allstks(string folder_name_parameterization,  string inputfolder, string biolsce);

// pop interaction (deprecated)
vector< vector<double> > COMMONSSHARED_EXPORT read_species_interactions_mortality_proportion_matrix(int nbpops, string folder_name_parameterization, string inputfolder, string biolsce);
vector< vector<double> > COMMONSSHARED_EXPORT read_preferences_for_species_matrix(int a_pop, int nbpops, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce);




#endif // DATALOADERPOPS_H
