#ifndef DATALOADERSHIPS_H
#define DATALOADERSHIPS_H

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

class [[deprecated]] Dataloaderships : public virtual Dataloader {
public:
    Dataloaderships();

    int gettype(void);

    int features(std::shared_ptr<msqlitecpp::v2::Storage> indb,
                 const string &folder_name_parameterization,
                 const string &inputfolder,
                 PopSceOptions const &dyn_pop_sce,
                 DynAllocOptions const &dyn_alloc_sce,
                 string const &biolsce,
                 string const &fleetsce,
                 ParamsForLoad &paramsForLoad,
                 LoadedData &loadedData) override;

};


bool  read_ships_features(vector<string>& shipids, vector<double> &imos,
                        vector<double> &yearbuilds, vector<string> &flags,
                        vector<string> &types, vector<double> &typecodes,
                        vector<double> &loas, vector<double> &KWs, vector<double> &breadths,
                        vector<double> &grosstonnages, vector<double> &nbunits,
                        vector<double> &fueluses,
                        vector<double> &NOxEmission_gperKWhs,
                        vector<double> &SOxEmission_percentpertotalfuelmasss,
                        vector<double> &GHGEmissions,
                        vector<double> &PMEmissions,
                        vector<double>& vmaxs, vector<double>& vcruises, vector<double>& lane_ids,
                        string folder_name_parameterization, string inputfolder);



multimap<int, double>  read_shiplanes_lon(string folder_name_parameterization, string inputfolder);
multimap<int, double>  read_shiplanes_lat(string folder_name_parameterization, string inputfolder);



#endif // DATALOADERSHIPS_H
