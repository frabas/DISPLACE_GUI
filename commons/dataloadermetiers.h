#ifndef DATALOADERMETIERS_H
#define DATALOADERMETIERS_H

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

class [[deprecated]] Dataloadermetiers : public virtual Dataloader {
public:
    Dataloadermetiers();

    int gettype(void);

    int features(std::shared_ptr<msqlitecpp::v2::Storage> indb,
                 const string &folder_name_parameterization,
                 const string &inputfolder,
                 PopSceOptions &dyn_pop_sce,
                 DynAllocOptions &dyn_alloc_sce,
                 string &biolsce,
                  string &fleetsce,
                  ParamsForLoad &paramsForLoad,
                  LoadedData & loadedData);

};


// metier specific
map<int, int>  read_metiers_types(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_sel_ogives(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_dis_ogives(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_metiers_betas(string a_semester, string folder_name_parameterization, string inputfolder) ;
multimap<int, double>  read_discardratio_limits(string a_semester, string folder_name_parameterization, string inputfolder) ;
multimap<int, int>  read_is_avoided_stocks(string a_semester, string folder_name_parameterization, string inputfolder) ;
multimap<int, int>  read_metiers_mls_cat(string a_semester, string folder_name_parameterization, string inputfolder) ;
map<int, double>  read_percent_revenue_completenesses(string folder_name_parameterization, string inputfolder);
map<int, double>  read_metiers_fspeed(string folder_name_parameterization, string inputfolder);
map<int, double>  read_gear_widths_param_a(string folder_name_parameterization, string inputfolder);
map<int, double>  read_gear_widths_param_b(string folder_name_parameterization, string inputfolder);
map<int, string>  read_gear_widths_model_type(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_loss_after_1_passage_per_landscape_per_func_group(int a_met, string folder_name_parameterization, string inputfolder);
multimap<int, int>  read_metier_target_stocks(int a_met, string folder_name_parameterization, string inputfolder);
multimap<int, int>  read_metier_suitable_seabottomtypes(int a_met, string folder_name_parameterization, string inputfolder);
vector< vector<double> >  read_selectivity_per_stock_ogives(int a_met, int nbpops, int nbszgroups, string folder_name_parameterization, string inputfolder, string fleetsce);
vector< vector<double> >  read_selectivity_per_stock_ogives_for_oth_land(int nbpops, int nbszgroups, string folder_name_parameterization, string inputfolder, string fleetsce);



#endif // DATALOADERMETIERS_H
