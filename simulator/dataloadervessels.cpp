#include "dataloadervessels.h"
#include<iostream>

using namespace std;

Dataloadervessels::Dataloadervessels()
{

}

int Dataloadervessels::gettype()
{
    return(1);
}

void Dataloadervessels::features(std::shared_ptr<sql::Storage> indb,
                                 const string& folder_name_parameterization,
                                 const string& inputfolder,
                                 PopSceOptions &dyn_pop_sce)
{
  this->Dataloader::features(indb,
                             folder_name_parameterization,
                             inputfolder,
                             dyn_pop_sce);
  cout << "Loading vessels features" << endl;



}

