#ifndef DATALOADERWINDMILLS_H
#define DATALOADERWINDMILLS_H

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

class [[deprecated]] Dataloaderwindmills : public virtual Dataloader {
public:
    Dataloaderwindmills();

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


map<int, double>  read_size_per_windmill(string folder_name_parameterization, string inputfolder);




#endif // DATALOADERWINDMILLS_H
