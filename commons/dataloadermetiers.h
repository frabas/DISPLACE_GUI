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
                 PopSceOptions const &dyn_pop_sce,
                 DynAllocOptions const &dyn_alloc_sce,
                 string const &biolsce,
                 string const &fleetsce,
                 ParamsForLoad &paramsForLoad,
                 LoadedData &loadedData) override;

};



#endif // DATALOADERMETIERS_H
