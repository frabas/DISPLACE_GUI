#ifndef DATALOADERBENTHOS_H
#define DATALOADERBENTHOS_H

#include <memory>
#include <string>
#include <map>

namespace msqlitecpp {
namespace v2 {
class Storage;
}
}

struct ParamsForLoad;
struct LoadedData;

class PopSceOptions;

class DynAllocOptions;

class Dataloaderbenthos {
public:
    Dataloaderbenthos();

    int gettype(void);

    int features(std::shared_ptr<msqlitecpp::v2::Storage> indb,
                 const std::string &folder_name_parameterization,
                 const std::string &inputfolder,
                 PopSceOptions &dyn_pop_sce,
                 DynAllocOptions &dyn_alloc_sce,
                 std::string &biolsce,
                 std::string &fleetsce,
                 ParamsForLoad &paramsForLoad,
                 LoadedData &loadedData);
};


std::multimap<int, double>
read_prop_funcgr_biomass_per_node_per_landscape(std::string folder_name_parameterization, std::string inputfolder);

std::multimap<int, double>
read_prop_funcgr_number_per_node_per_landscape(std::string folder_name_parameterization, std::string inputfolder);

std::multimap<int, double>
read_meanw_funcgr_per_landscape(std::string folder_name_parameterization, std::string inputfolder);

std::multimap<int, double>
read_longevity_classes_condition_per_node(std::string folder_name_parameterization, std::string inputfolder);

std::multimap<int, double>
read_logistic_recovery_rates_per_month_per_funcgr(std::string folder_name_parameterization, std::string inputfolder);

std::multimap<int, double>
read_habitat_deltas_per_pop(std::string folder_name_parameterization, std::string inputfolder);

std::multimap<int, double>
read_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr(std::string folder_name_parameterization,
                                                                  std::string inputfolder);

std::multimap<int, double>
read_benthos_number_carrying_capacity_K_per_landscape_per_funcgr(std::string folder_name_parameterization,
                                                                 std::string inputfolder);


#endif // DATALOADERBENTHOS_H
