#ifndef DATALOADER_H
#define DATALOADER_H

#include<dataloader.h>

#include <iostream>
#include <string>
#include <options.h>

#include "msqlitecpp/v2/storage.h"

namespace sql = msqlitecpp::v2;
using namespace std;


// abstract class
class Dataloader
{
public:
    Dataloader(){}
    virtual int gettype(void)=0;
    virtual void features(std::shared_ptr<sql::Storage> indb,
                          const string& folder_name_parameterization,
                          const string& inputfolder,
                          PopSceOptions& dyn_pop_sce){ cout << "Loading features" << endl;}
};


class Loader
{
public:
    void loadfeatures(Dataloader *dl,
                      std::shared_ptr<sql::Storage> indb,
                      const string& folder_name_parameterization,
                      const string& inputfolder,
                      PopSceOptions& dyn_pop_sce)
    {
       dl->features(indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce);
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

