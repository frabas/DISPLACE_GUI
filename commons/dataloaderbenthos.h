#ifndef DATALOADERBENTHOS_H
#define DATALOADERBENTHOS_H

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
#include "Benthos.h"

#include "helpers.h"
#include "options.h"

#include "msqlitecpp/v2/storage.h"

namespace sql = msqlitecpp::v2;
using namespace std;





class [[deprecated]] Dataloaderbenthos : public virtual Dataloader {
public:
    Dataloaderbenthos();

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


multimap<int, double>  read_prop_funcgr_biomass_per_node_per_landscape(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_prop_funcgr_number_per_node_per_landscape(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_meanw_funcgr_per_landscape(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_longevity_classes_condition_per_node(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_logistic_recovery_rates_per_month_per_funcgr(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_habitat_deltas_per_pop(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_benthos_number_carrying_capacity_K_per_landscape_per_funcgr(string folder_name_parameterization, string inputfolder);




#endif // DATALOADERBENTHOS_H
