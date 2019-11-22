#ifndef DATALOADER_H
#define DATALOADER_H

#include<dataloader.h>

#include <iostream>
#include <string>
#include <options.h>

using namespace std;


// abstract class
class Dataloader
{
public:
    Dataloader(){}
    virtual int gettype(void)=0;
    virtual void features(const string& folder_name_parameterization,
                          const string& inputfolder,
                          PopSceOptions& dyn_pop_sce){ cout << "Loading features" << endl;}
};


class Loader
{
public:
    void loadfeatures(Dataloader *dl,
                      const string& folder_name_parameterization,
                      const string& inputfolder,
                      PopSceOptions& dyn_pop_sce)
    {
       dl->features(folder_name_parameterization,
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

