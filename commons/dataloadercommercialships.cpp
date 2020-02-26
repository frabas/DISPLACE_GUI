#include "dataloadercommercialships.h"
#include<iostream>

using namespace std;



Dataloaderships::Dataloaderships()
{

}

int Dataloaderships::gettype()
{
    return(1);
}



//----------------
bool read_ships_features(vector<string>& shipids,
                         vector<double> &imos,
                         vector<double> &yearbuilds, vector<string> &flags,
                         vector<string> &types, vector<double> &typecodes,
                         vector<double> &loas,
                         vector<double> &KWs,
                         vector<double> &breadths,
                         vector<double> &grosstonnages, vector<double> &nbunits,
                         vector<double> &fueluses,
                         vector<double> &NOxEmission_gperKWhs,
                         vector<double> &SOxEmission_percentpertotalfuelmasss,
                         vector<double> &GHGEmissions,
                         vector<double> &PMEmissions,
                         vector<double>& vmaxs,
                         vector<double>& vcruises,
                         vector<double>& lane_ids,
                         string folder_name_parameterization,
                         string inputfolder
                         )
{

    string filename=  inputfolder+"/shipsspe_"+folder_name_parameterization+"/shipsspe_features.dat";

    ifstream ships_features;
    ships_features.open(filename.c_str());
    if(ships_features.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }

    bool r = fill_from_ships_specifications(ships_features, shipids, imos,
                                            yearbuilds, flags, types, typecodes,
                                            loas, KWs, breadths, grosstonnages, nbunits,
                                            fueluses, NOxEmission_gperKWhs,
                                            SOxEmission_percentpertotalfuelmasss,
                                            GHGEmissions, PMEmissions,
                                            vmaxs, vcruises, lane_ids);
    ships_features.close();

    return r;
}



multimap<int, double> read_shiplanes_lat(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/shipsspe_"+folder_name_parameterization+"/shipsspe_lanes_lat.dat";

    ifstream file_shipsspe_lanes_lat;
    file_shipsspe_lanes_lat.open(filename.c_str());
    if(file_shipsspe_lanes_lat.fail())
    {
        cout << "Unfortunately the /shipsspe_lanes_lat.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> shipsspe_lanes_lat;
    fill_multimap_from_specifications_i_d(file_shipsspe_lanes_lat,  shipsspe_lanes_lat);
    file_shipsspe_lanes_lat.close();

    return(shipsspe_lanes_lat);
}



multimap<int, double> read_shiplanes_lon(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/shipsspe_"+folder_name_parameterization+"/shipsspe_lanes_lon.dat";

    ifstream file_shipsspe_lanes_lon;
    file_shipsspe_lanes_lon.open(filename.c_str());
    if(file_shipsspe_lanes_lon.fail())
    {
        cout << "Unfortunately the /shipsspe_lanes_lon.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> shipsspe_lanes_lon;
    fill_multimap_from_specifications_i_d(file_shipsspe_lanes_lon,  shipsspe_lanes_lon);
    file_shipsspe_lanes_lon.close();

    return(shipsspe_lanes_lon);
}






int Dataloaderships::features(std::shared_ptr<sql::Storage> indb,
                              const string &folder_name_parameterization,
                              const string &inputfolder,
                              PopSceOptions const &dyn_pop_sce,
                              DynAllocOptions const &dyn_alloc_sce,
                              string const &biolsce,
                              string const &fleetsce,
                              ParamsForLoad &paramsForLoad,
                              LoadedData &loadedData)
{



 this->Dataloader::features(indb,
                          folder_name_parameterization,
                          inputfolder,
                          dyn_pop_sce,
                          dyn_alloc_sce,
                          biolsce,
                          fleetsce,
                          paramsForLoad,
                          loadedData);
  cout << "Loading ships features" << endl;


  // read general ship features (incl. their specific lanes)
  vector<string> shipids;
  vector<double> imos;
  vector<double> yearbuilds;
  vector<string> flags;
  vector<string> types;
  vector<double> typecodes;
  vector<double> loas;
  vector<double> KWs;
  vector<double> breadths;
  vector<double> grosstonnages;
  vector<double> nbunits;
  vector<double> fueluses;
  vector<double> NOxEmission_gperKWhs;
  vector<double> SOxEmission_percentpertotalfuelmasss;
  vector<double> GHGEmissions;
  vector<double> PMEmissions;
  vector<double> vmaxs;
  vector<double> vcruises;
  vector<double> lane_ids;
  if (!read_ships_features(shipids, imos, yearbuilds, flags, types, typecodes,
                           loas, KWs, breadths, grosstonnages, nbunits,
                           fueluses, NOxEmission_gperKWhs, SOxEmission_percentpertotalfuelmasss,
                           GHGEmissions, PMEmissions,
                           vmaxs, vcruises, lane_ids,
                           folder_name_parameterization, inputfolder)) {
      cerr << "Error loading ship features. Bailing out.\n";
      return 2;
  }

  // read shipping lanes
  multimap<int, double> shiplanes_lat = read_shiplanes_lat(folder_name_parameterization, inputfolder);
  multimap<int, double> shiplanes_lon = read_shiplanes_lon(folder_name_parameterization, inputfolder);


  //  export
  loadedData.vectsparam1=shipids;
  loadedData.vectdparam1=imos;
  loadedData.vectdparam2=yearbuilds;
  loadedData.vectsparam2=flags;
  loadedData.vectsparam3=types;
  loadedData.vectdparam3=typecodes;
  loadedData.vectdparam4=loas;
  loadedData.vectdparam5=KWs;
  loadedData.vectdparam6=breadths;
  loadedData.vectdparam7=grosstonnages;
  loadedData.vectdparam8=nbunits;
  loadedData.vectdparam9=fueluses;
  loadedData.vectdparam10=NOxEmission_gperKWhs;
  loadedData.vectdparam11=SOxEmission_percentpertotalfuelmasss;
  loadedData.vectdparam12=GHGEmissions;
  loadedData.vectdparam13=PMEmissions;
  loadedData.vectdparam14=vmaxs;
  loadedData.vectdparam15=vcruises;
  loadedData.vectdparam16=lane_ids;
  loadedData.mmapidparam1=shiplanes_lon;
  loadedData.mmapidparam2=shiplanes_lat;




return 0;
}

