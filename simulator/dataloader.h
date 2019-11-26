#ifndef DATALOADER_H
#define DATALOADER_H


#include <iostream>
#include <string>
#include <options.h>

#include "msqlitecpp/v2/storage.h"

#include "myutils.h"

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
    std::multimap<int,double> mmapidparam1;
    std::multimap<int,double> mmapidparam2;
    std::multimap<int,double> mmapidparam3;
    std::multimap<int,double> mmapidparam4;
    std::multimap<int,double> mmapidparam5;
    std::multimap<int,double> mmapidparam6;
    std::multimap<int,double> mmapidparam7;
    std::multimap<int,double> mmapidparam8;
    std::multimap<types::NodeId,int> mmapniparam1;
    std::multimap<types::NodeId,double> mmapndparam1;
    int int1;
    int int2;
    int int3;
    int int4;
    int int5;
    double double1;
    double double2;
    double double3;
    double double4;
    double double5;
    double double6;
    double double7;
    double double8;
    double double9;
    double double10;
    double double11;
    double double12;
    double double13;
    double double14;
    double double15;
    double double16;
    double double17;
    double double18;
    double double19;
    double double20;
    double double21;
    double double22;
    double double23;
    double double24;
    double double25;
    double double26;
    double double27;
    double double28;
    double double29;
    double double30;
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
    void loadFeatures(Dataloader *dl,
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

