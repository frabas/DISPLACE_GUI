#ifndef DATALOADER_H
#define DATALOADER_H


#include <iostream>
#include <string>
#include <options.h>

#include "msqlitecpp/v2/storage.h"

#include "myutils.h"
#include "vesselcalendar.h"
#include "helpers.h"
#include "idtypes.h"
#include "idtypeshelpers.h"

namespace sql = msqlitecpp::v2;
using namespace std;



struct [[deprecated]] LoadedData {
    std::vector<string> vectsparam1;
    std::vector<string> vectsparam2;
    std::vector<string> vectsparam3;
    std::vector<string> vectsparam4;
    std::vector<string> vectsparam5;
    std::vector<VesselCalendar> vectcalendar1;
    std::vector<double> vectdparam1;
    std::vector<double> vectdparam2;
    std::vector<double> vectdparam3;
    std::vector<double> vectdparam4;
    std::vector<double> vectdparam5;
    std::vector<double> vectdparam6;
    std::vector<double> vectdparam7;
    std::vector<double> vectdparam8;
    std::vector<double> vectdparam9;
    std::vector<double> vectdparam10;
    std::vector<double> vectdparam11;
    std::vector<double> vectdparam12;
    std::vector<double> vectdparam13;
    std::vector<double> vectdparam14;
    std::vector<double> vectdparam15;
    std::vector<double> vectdparam16;
    std::vector<double> vectdparam17;
    std::vector<double> vectdparam18;
    std::vector<double> vectdparam19;
    std::vector<double> vectdparam20;
    std::vector<double> vectdparam21;
    std::vector<double> vectdparam22;
    std::vector<double> vectdparam23;
    std::vector<double> vectdparam24;
    std::vector<double> vectdparam25;
    std::vector<double> vectdparam26;
    std::vector<double> vectdparam27;
    std::vector<double> vectdparam28;
    std::vector<double> vectdparam29;
    std::vector<double> vectdparam30;
    std::vector<double> vectdparam31;
    std::vector<double> vectdparam32;
    std::vector<double> vectdparam33;
    std::vector<double> vectdparam34;
    std::vector<double> vectdparam35;
    std::vector<double> vectdparam36;
    std::vector<double> vectdparam37;
    std::vector<double> vectdparam38;
    std::vector<double> vectdparam39;
    std::vector<double> vectdparam40;
    std::vector<double> vectdparam41;
    std::vector<double> vectdparam42;
    std::vector<double> vectdparam43;
    std::vector<double> vectdparam44;
    std::vector<double> vectdparam45;
    std::vector<double> vectdparam46;
    std::vector<double> vectdparam47;
    std::vector<double> vectdparam48;
    std::vector<double> vectdparam49;
    std::vector<double> vectdparam50;
    std::vector<int> vectiparam1;
    std::vector<int> vectiparam2;
    std::vector<int> vectiparam3;
    std::vector<int> vectiparam4;
    std::vector<int> vectiparam5;
    std::vector<int> vectiparam6;
    std::vector<int> vectiparam7;
    std::vector<int> vectiparam8;
    std::vector<int> vectiparam9;
    std::vector<int> vectiparam10;   
    std::multimap<int,double> mmapidparam1;
    std::multimap<int,double> mmapidparam2;
    std::multimap<int,double> mmapidparam3;
    std::multimap<int,double> mmapidparam4;
    std::multimap<int,double> mmapidparam5;
    std::multimap<int,double> mmapidparam6;
    std::multimap<int,double> mmapidparam7;
    std::multimap<int,double> mmapidparam8;
    std::multimap<string,double> mmapsdparam1;
    std::multimap<string,double> mmapsdparam2;
    std::multimap<string,double> mmapsdparam3;
    std::multimap<string,double> mmapsdparam4;
    std::multimap<string,double> mmapsdparam5;
    std::multimap<string,double> mmapsdparam6;
    std::multimap<string,double> mmapsdparam7;
    std::multimap<string,double> mmapsdparam8;
    std::multimap<string,types::NodeId > mmapsnparam1;
    std::multimap<string,types::NodeId > mmapsnparam2;
    std::multimap<string,types::NodeId > mmapsnparam3;
    std::multimap<string,types::NodeId > mmapsnparam4;
    std::multimap<types::NodeId,int> mmapniparam1;
    std::multimap<types::NodeId,int> mmapniparam2;
    std::multimap<types::NodeId,int> mmapniparam3;
    std::multimap<types::NodeId,int> mmapniparam4;
    std::multimap<types::NodeId,int> mmapniparam5;
    std::multimap<types::NodeId,int> mmapniparam6;
    std::multimap<types::NodeId,int> mmapniparam7;
    std::multimap<types::NodeId,int> mmapniparam8;
    std::multimap<types::NodeId,int> mmapniparam9;
    std::multimap<types::NodeId,int> mmapniparam10;
    std::multimap<types::NodeId,double> mmapndparam1;
    std::multimap<types::NodeId,double> mmapndparam2;
    std::vector<map<string,double> > vectmapsdparam1;
    std::map<int,int> mapiiparam1;
    std::map<int,int> mapiiparam2;
    std::map<int,double> mapidparam1;
    std::map<int,double> mapidparam2;
    std::map<int,double> mapidparam3;
    std::map<int,double> mapidparam4;
    std::map<int,string> mapisparam1;
    std::vector<map<int,int> > vectmapiiparam1;
    std::vector<map<int,double> > vectmapidparam1;
    std::vector<map<types::NodeId,double> > vectmapndparam1;
    std::vector<multimap<types::NodeId,int> > vectmmapniparam1;
    std::vector<multimap<types::NodeId,double> > vectmmapndparam1;
    std::vector<multimap<types::NodeId,double> > vectmmapndparam2;
    std::vector<multimap<types::NodeId,double> > vectmmapndparam3;
    std::vector<multimap<types::NodeId,double> > vectmmapndparam4;
    std::vector<multimap<int,double> > vectmmapidparam1;
    std::vector<vector <double> > vovd1;
    std::vector<vector <double> > vovd2;
    std::vector<vector <double> > vovd3;
    std::vector<vector <double> > vovd4;
    std::vector<vector <double> > vovd5;
    std::vector<vector <double> > vovd6;
    std::vector<vector <double> > vovd7;
    std::vector<vector <double> > vovd8;
    std::vector<vector <double> > vovd9;
    std::vector<vector <double> > vovd10;
    std::vector<vector <types::NodeId> > vovn1;
    std::vector<vector <int> > vovi1;
    std::vector<vector <int> > vovi2;
    std::vector<vector <int> > vovi3;
    std::vector<vector <int> > vovi4;
    std::vector<vector <int> > vovi5;
    std::vector<vector<vector <double> > > vovovd1;
    std::vector<vector<vector <double> > > vovovd2;
    std::vector<vector<vector <double> > > vovovd3;
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
    multimap <int, double> mmapidparam_init_weight_per_szgroup;
    vector<vector<double> >vovd_species_interactions_mortality_proportion_matrix;
 };


struct [[deprecated]] ParamsForLoad {
    string sparam1;
    string sparam2;
    string sparam3;
    string sparam4;
    int iparam1;
    int iparam2;
    int iparam3;
    int iparam4;
    double dparam1;
    double dparam2;
    double dparam3;
    vector <double > vdparam1;
    vector <double > vdparam2;
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
class [[deprecated]] Dataloader {
public:
    Dataloader()
    {}

    virtual int gettype(void) = 0;

    virtual int features(std::shared_ptr<sql::Storage> indb,
                         const string &folder_name_parameterization,
                         const string &inputfolder,
                         PopSceOptions const &dyn_pop_sce,
                         DynAllocOptions const &dyn_alloc_sce,
                         string const &biolsce,
                         string const &fleetsce,
                         ParamsForLoad &paramsForLoad,
                         LoadedData &loadedData)
    {
        cout << "Loading features" << endl;
        return (0);
    }
};


class [[deprecated]] Loader {
public:
    void loadFeatures(Dataloader *dl,
                      std::shared_ptr<sql::Storage> indb,
                      const string &folder_name_parameterization,
                      const string &inputfolder,
                      PopSceOptions const &dyn_pop_sce,
                      DynAllocOptions const &dyn_alloc_sce,
                      string const &biolsce,
                      string const &fleetsce,
                      ParamsForLoad &paramsForLoad,
                      LoadedData &loadedData)
    {
       dl->features(indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce,
                    dyn_alloc_sce,
                    biolsce,
                    fleetsce,
                    paramsForLoad,
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

