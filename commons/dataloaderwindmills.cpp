#include "dataloaderwindmills.h"
#include<iostream>

using namespace std;



Dataloaderwindmills::Dataloaderwindmills()
{

}

int Dataloaderwindmills::gettype()
{
    return(1);
}




// FOR WINDMILLS
map<int, double> read_size_per_windmill(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/windmillsspe_"+folder_name_parameterization+"/size_per_windmill.dat";

    ifstream file_size_per_windmill;
    file_size_per_windmill.open(filename.c_str());
    if(file_size_per_windmill.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> size_per_windmill;
    fill_map_from_specifications(file_size_per_windmill,  size_per_windmill);
    file_size_per_windmill.close();

    return(size_per_windmill);
}








int Dataloaderwindmills::features(std::shared_ptr<sql::Storage> indb,
                                 const string& folder_name_parameterization,
                                 const string& inputfolder,
                                 PopSceOptions &dyn_pop_sce,
                                 DynAllocOptions &dyn_alloc_sce,
                                 LoadedData& loadedData)
{



    this->Dataloader::features(indb,
                             folder_name_parameterization,
                             inputfolder,
                             dyn_pop_sce,
                             dyn_alloc_sce,
                             loadedData);
  cout << "Loading windmills features" << endl;




  map<int, double> init_size_per_windmill = read_size_per_windmill(folder_name_parameterization, inputfolder);
  cout << "Does the size_per_windmill need a check?" << endl;



  //  export
  loadedData.mapidparam1 =init_size_per_windmill;



return 0;
}

