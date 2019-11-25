#ifndef DATALOADER_H
#define DATALOADER_H

#include<dataloader.h>

#include <iostream>
#include <string>
#include <options.h>

#include "msqlitecpp/v2/storage.h"

namespace sql = msqlitecpp::v2;
using namespace std;

struct LoadedData {
    std::vector<double> vectparam1;
    std::vector<double> vectparam2;
    std::vector<double> vectparam3;
    std::vector<double> vectparam4;
    std::vector<double> vectparam5;
    std::vector<double> vectparam6;
    std::vector<double> vectparam7;
    std::vector<double> vectparam8;
    std::vector<double> vectparam9;
    std::vector<double> vectparam10;
};


// e.g. for Benthos() we need to pass:
//init_prop_funcgr_biomass_per_node,
//init_prop_funcgr_number_per_node,
//init_meanw_funcgr_per_node,
//init_recovery_rates_per_funcgr,
//init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr,
//init_benthos_number_carrying_capacity_K_per_landscape_per_funcgr,
//init_h_betas_per_pop,
//longevity_classes_condition_per_node




// abstract class
class Dataloader
{
public:
    Dataloader(){}
    virtual int gettype(void)=0;
    virtual void features(std::shared_ptr<sql::Storage> indb,
                          const string& folder_name_parameterization,
                          const string& inputfolder,
                          PopSceOptions& dyn_pop_sce,
                          LoadedData& loadedData){ cout << "Loading features" << endl;}
};


class Loader
{
public:
    void loadfeatures(Dataloader *dl,
                      std::shared_ptr<sql::Storage> indb,
                      const string& folder_name_parameterization,
                      const string& inputfolder,
                      PopSceOptions& dyn_pop_sce,
                      LoadedData& loadedData)
    {
       dl->features(indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce,
                    loadedData);
    }
};


/* example of use to put in simulator main.cpp:
 *
 * Loader *l = new Loader;
 *
 * Dataloadervessels vl;
 * l->loadFeatures(&vl)
 *
 * Dataloaderbenthos bl;
 * l->loadFeatures(&bl)
 *
 */

#endif // DATALOADER_H

