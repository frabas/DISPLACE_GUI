#include "dataloadermetiers.h"
#include<iostream>

using namespace std;



Dataloadermetiers::Dataloadermetiers()
{

}

int Dataloadermetiers::gettype()
{
    return(1);
}


multimap<int, int> read_metier_target_stocks(int a_met, string folder_name_parameterization, string inputfolder)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/met_target_names.dat";

    ifstream file_metier_target_stocks;
    file_metier_target_stocks.open(filename.c_str());
    if(file_metier_target_stocks.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> metier_target_stocks;
    fill_multimap_from_specifications_i_i(file_metier_target_stocks,  metier_target_stocks);
    file_metier_target_stocks.close();

    return(metier_target_stocks);
}

multimap<int, int> read_metier_suitable_seabottomtypes(int a_met, string folder_name_parameterization, string inputfolder)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_suitable_seabottomtypes.dat";

    ifstream file_metier_suitable_seabottomtypes;
    file_metier_suitable_seabottomtypes.open(filename.c_str());
    if(file_metier_suitable_seabottomtypes.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
   }
    multimap<int, int> metier_suitable_seabottomtypes;
    fill_multimap_from_specifications_i_i(file_metier_suitable_seabottomtypes,  metier_suitable_seabottomtypes);
    file_metier_suitable_seabottomtypes.close();

    return(metier_suitable_seabottomtypes);
}


// FOR METIER
map<int, int> read_metiers_types(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/combined_met_types.dat";

    ifstream metier_types;
    metier_types.open(filename.c_str());
    if(metier_types.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, int> met_types;
    fill_map_from_specifications_i_i (metier_types,  met_types, folder_name_parameterization);
    metier_types.close();

    return(met_types);
}



// FOR METIER
map<int, double> read_percent_revenue_completenesses(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/percent_revenue_completenesses.dat";

    ifstream metier_percent_revenue_completenesses;
    metier_percent_revenue_completenesses.open(filename.c_str());
    if(metier_percent_revenue_completenesses.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    map<int, double> percent_revenue_completenesses;
    fill_map_from_specifications_i_d (metier_percent_revenue_completenesses,  percent_revenue_completenesses, folder_name_parameterization);
    metier_percent_revenue_completenesses.close();

    return(percent_revenue_completenesses);
}


// FOR METIER
map<int, double> read_metiers_fspeed(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_fspeed.dat";

    ifstream metier_fspeed;
    metier_fspeed.open(filename.c_str());
    if(metier_fspeed.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> fspeed;
    fill_map_from_specifications_i_d (metier_fspeed,  fspeed, folder_name_parameterization);
    metier_fspeed.close();

    return(fspeed);
}


// FOR METIER
map<int, double> read_gear_widths_param_a(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_gear_widths_param_a.dat";

    ifstream metier_gear_widths_param_a;
    metier_gear_widths_param_a.open(filename.c_str());
    if(metier_gear_widths_param_a.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
   }

    //cout << "here:" << filename << endl;

    map<int, double> gear_widths_param_a;
    fill_map_from_specifications_i_d (metier_gear_widths_param_a,  gear_widths_param_a, folder_name_parameterization);
    metier_gear_widths_param_a.close();

    //cout << "here:" << gear_widths_param_a[0] << endl;
    //cout << "here:" << gear_widths_param_a[1] << endl;
    //=> do not forget the heading in input files given first line is skipped



    return(gear_widths_param_a);
}


// FOR METIER
map<int, double> read_gear_widths_param_b(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_gear_widths_param_b.dat";

    ifstream metier_gear_widths_param_b;
    metier_gear_widths_param_b.open(filename.c_str());
    if(metier_gear_widths_param_b.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> gear_widths_param_b;
    fill_map_from_specifications_i_d (metier_gear_widths_param_b,  gear_widths_param_b, folder_name_parameterization);
    metier_gear_widths_param_b.close();

    return(gear_widths_param_b);
}


// FOR METIER
map<int, string> read_gear_widths_model_type(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_gear_widths_model_type.dat";

    ifstream metier_gear_widths_model_type;
    metier_gear_widths_model_type.open(filename.c_str());
    if(metier_gear_widths_model_type.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    map<int, string> gear_widths_model_type;
    fill_map_from_specifications_i_s (metier_gear_widths_model_type,  gear_widths_model_type, folder_name_parameterization);
    metier_gear_widths_model_type.close();

    return(gear_widths_model_type);
}


// FOR METIER
multimap<int, double> read_dis_ogives(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_discards_ogives.dat";

    ifstream metier_discards_ogives;
    metier_discards_ogives.open(filename.c_str());
    if(metier_discards_ogives.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> dis_ogives;
    fill_multimap_from_specifications_i_d(metier_discards_ogives,  dis_ogives);
    metier_discards_ogives.close();

    return(dis_ogives);
}



multimap<int, double> read_metiers_betas(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metierspe_betas_"+a_semester+".dat";

    ifstream metierspe_betas_file;
    metierspe_betas_file.open(filename.c_str());
    if(metierspe_betas_file.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> metiers_betas;
    fill_multimap_from_specifications_i_d(metierspe_betas_file,  metiers_betas);
    metierspe_betas_file.close();

    return(metiers_betas);
}

multimap<int, double> read_discardratio_limits(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metierspe_discardratio_limits_"+a_semester+".dat";

    ifstream discardratio_limits_file;
    discardratio_limits_file.open(filename.c_str());
    if(discardratio_limits_file.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    multimap<int, double> discardratio_limits;
    fill_multimap_from_specifications_i_d(discardratio_limits_file,  discardratio_limits);
    discardratio_limits_file.close();

    return(discardratio_limits);
}


multimap<int, int> read_is_avoided_stocks(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metierspe_is_avoided_stocks_"+a_semester+".dat";

    ifstream is_avoided_stocks_file;
    is_avoided_stocks_file.open(filename.c_str());
    if(is_avoided_stocks_file.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> is_avoided_stocks;
    fill_multimap_from_specifications_i_i(is_avoided_stocks_file,  is_avoided_stocks);
    is_avoided_stocks_file.close();

    return(is_avoided_stocks);
}



multimap<int, int> read_metiers_mls_cat(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/metiersspe_"+folder_name_parameterization+"/metierspe_mls_cat_"+a_semester+".dat";

    ifstream metierspe_mls_cat_file;
    metierspe_mls_cat_file.open(filename.c_str());
    if(metierspe_mls_cat_file.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> metierspe_mls_cat;
    fill_multimap_from_specifications_i_i(metierspe_mls_cat_file,  metierspe_mls_cat);
    metierspe_mls_cat_file.close();

    return(metierspe_mls_cat);
}



vector< vector<double> > read_selectivity_per_stock_ogives(int a_met,
                                                           int nbpops,
                                                           int nbszgroup,
                                                           string folder_name_parameterization,
                                                           string inputfolder,
                                                           string fleetsce)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename = inputfolder+"/metiersspe_"+folder_name_parameterization+"/" + a_met_s + "metier_selectivity_per_stock_ogives_fleetsce"+fleetsce+".dat";

    ifstream file_selectivity_per_stock_ogives;
    file_selectivity_per_stock_ogives.open(filename.c_str());
    if(file_selectivity_per_stock_ogives.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    vector< vector<double> > selectivity_per_stock_ogives(nbpops, vector<double>(nbszgroup));
    if (!fill_in_selectivity_per_stock(file_selectivity_per_stock_ogives, selectivity_per_stock_ogives))
        throw std::runtime_error("Error while executuing: fill_in_selectivity_per_stock");

    file_selectivity_per_stock_ogives.close();

    return(selectivity_per_stock_ogives);
}




vector< vector<double> > read_selectivity_per_stock_ogives_for_oth_land(int nbpops,
                                                                        int nbszgroup,
                                                                        string folder_name_parameterization,
                                                                        string inputfolder,
                                                                        string fleetsce)
{

    string filename = inputfolder+"/metiersspe_"+folder_name_parameterization+"/metier_selectivity_per_stock_ogives_fleetsce"+fleetsce+"_for_oth_land.dat";

    ifstream file_selectivity_per_stock_ogives_for_oth_land;
    file_selectivity_per_stock_ogives_for_oth_land.open(filename.c_str());
    if(file_selectivity_per_stock_ogives_for_oth_land.fail())
    {
        vector<vector <double> > selectivity_per_stock_ogives_for_oth_land;
         return selectivity_per_stock_ogives_for_oth_land; // caution: returns an empty object
    }
    vector< vector<double> > selectivity_per_stock_ogives_for_oth_land(nbpops, vector<double>(nbszgroup));
    if (!fill_in_selectivity_per_stock(file_selectivity_per_stock_ogives_for_oth_land, selectivity_per_stock_ogives_for_oth_land))
        throw std::runtime_error("Error while executuing: fill_in_selectivity_per_stock");

    file_selectivity_per_stock_ogives_for_oth_land.close();

    return(selectivity_per_stock_ogives_for_oth_land);
}






int Dataloadermetiers::features(std::shared_ptr<sql::Storage> indb,
                                 const string& folder_name_parameterization,
                                 const string& inputfolder,
                                 PopSceOptions &dyn_pop_sce,
                                 DynAllocOptions &dyn_alloc_sce,
                                 string &biolsce,
                                 string &fleetsce,
                                 ParamsForLoad &paramsForLoad,
                                LoadedData& loadedData)
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
  cout << "Loading metiers features" << endl;


  //input data, metier characteristics: selectivty ogives, beta per pop
  cout << "check whether all metiers informed in the following parameters files... " << endl;
  cout << "read metier betas parameters....ok? " << endl;
  multimap<int, double> metiers_betas = read_metiers_betas(paramsForLoad.sparam3, folder_name_parameterization, inputfolder);
  cout << "read discardratio_limits parameters....ok? " << endl;
  multimap<int, double> discards_rate_limits = read_discardratio_limits(paramsForLoad.sparam3, folder_name_parameterization,
                                                                        inputfolder);
  cout << "read is_avoided_stocks parameters....ok? " << endl;
  multimap<int, int> is_avoided_stockss = read_is_avoided_stocks(paramsForLoad.sparam3, folder_name_parameterization,
                                                                 inputfolder);
  cout << "read mls cat parameters....ok? " << endl;
  multimap<int, int> metiers_mls_cat = read_metiers_mls_cat(paramsForLoad.sparam3, folder_name_parameterization, inputfolder);
  cout << "read metiers types parameters....ok? " << endl;
  map<int, int> metiers_types = read_metiers_types(folder_name_parameterization, inputfolder);
  cout << "read revenue completeness parameters....ok? " << endl;
  map<int, double> percent_revenue_completenesses = read_percent_revenue_completenesses(folder_name_parameterization,
                                                                                        inputfolder);
  cout << "read fspeed parameters....ok? " << endl;
  map<int, double> metiers_fspeed = read_metiers_fspeed(folder_name_parameterization, inputfolder);
  cout << "read metier gear width a parameters....ok? " << endl;
  map<int, double> metiers_gear_widths_param_a = read_gear_widths_param_a(folder_name_parameterization, inputfolder);
  cout << "read metier gear width b parameters....ok? " << endl;
  map<int, double> metiers_gear_widths_param_b = read_gear_widths_param_b(folder_name_parameterization, inputfolder);
  cout << "read metier gear width model type parameters....ok? " << endl;
  map<int, string> metiers_gear_widths_model_type = read_gear_widths_model_type(folder_name_parameterization,
                                                                                inputfolder);

  // oth_land are not metier-specific by nature, but the reader is placed here for coherence...
  vector<vector<double> > selectivity_per_stock_ogives_for_oth_land = read_selectivity_per_stock_ogives_for_oth_land(
          paramsForLoad.iparam1, paramsForLoad.iparam3, folder_name_parameterization, inputfolder, fleetsce);

  // get the name of the metiers
  // copy only unique elements into name_metiers
  cout << "retrieve the metier names.... " << endl;
  vector<int> name_metiers;
  for (multimap<int, double>::iterator iter = metiers_gear_widths_param_a.begin();
       iter != metiers_gear_widths_param_a.end();
       iter = metiers_gear_widths_param_a.upper_bound(iter->first)) {
      name_metiers.push_back(iter->first);
      outc(cout << "metier " << iter->first << endl);
  }
  cout << "nb metiers: " << name_metiers.size() << endl;
  cout
          << "!!CAUTION!! nb metiers retrieved from the metier_gear_widths_param_a.dat file...do not forget the headers in this file! "
          << endl;






  // loop over metiers
  vector <vector<double> > vect_of_metier_betas_vovd(name_metiers.size());
  vector <vector<double> > vect_of_discardratio_limits_vovd(name_metiers.size());
  vector <vector<int> > vect_of_is_avoided_stocks_vovi(name_metiers.size());
  vector <vector<int> > vect_of_metier_mls_cat_vovi(name_metiers.size());
  vector <multimap<int, double> > vect_of_loss_after_1_passage_mmapid(name_metiers.size());
  vector <vector<int> > vect_of_the_metier_target_stocks_vovi(name_metiers.size());
  vector <vector<int> > vect_of_the_metier_suitable_seabottomtypes_vovi(name_metiers.size());
  vector <vector<vector<double> > > vect_of_selectivity_per_stock_ogives_vovovd(name_metiers.size());


  for (unsigned int i = 0; i < name_metiers.size(); i++) {

          int metier_name = i;
          cout << "creating metier " << i << endl;

          vect_of_metier_betas_vovd.at(i) = find_entries_i_d(metiers_betas, metier_name);
          vect_of_discardratio_limits_vovd.at(i) = find_entries_i_d(discards_rate_limits, metier_name);
          vect_of_is_avoided_stocks_vovi.at(i) = find_entries_i_i(is_avoided_stockss, metier_name);
          vect_of_metier_mls_cat_vovi.at(i) = find_entries_i_i(metiers_mls_cat, metier_name);

          vect_of_loss_after_1_passage_mmapid.at(i) = read_loss_after_1_passage_per_landscape_per_func_group(metier_name,
                                                                                                              folder_name_parameterization,
                                                                                                              inputfolder);


          vect_of_selectivity_per_stock_ogives_vovovd.at(i) = read_selectivity_per_stock_ogives(i, paramsForLoad.iparam1, paramsForLoad.iparam4,
                                                                                                   folder_name_parameterization,
                                                                                                   inputfolder, fleetsce);

          multimap<int, int> metier_target_stocks = read_metier_target_stocks(metier_name, folder_name_parameterization,
                                                                              inputfolder);
          multimap<int, int> metier_suitable_seabottomtypes = read_metier_suitable_seabottomtypes(metier_name,
                                                                                                  folder_name_parameterization,
                                                                                                  inputfolder);

          // metier_target_stocks for this particular metier
          multimap<int, int>::iterator lower_metier_target_stocks = metier_target_stocks.lower_bound(i);
          multimap<int, int>::iterator upper_metier_target_stocks = metier_target_stocks.upper_bound(i);
          for (multimap<int, int>::iterator pos = lower_metier_target_stocks; pos != upper_metier_target_stocks; pos++)
              vect_of_the_metier_target_stocks_vovi.at(i).push_back(pos->second);

          // metier_suitable_seabottomtypes for this particular metier
          multimap<int, int>::iterator lower_metier_suitable_seabottomtypes = metier_suitable_seabottomtypes.lower_bound(
                  i);
          multimap<int, int>::iterator upper_metier_suitable_seabottomtypes = metier_suitable_seabottomtypes.upper_bound(
                  i);
          for (multimap<int, int>::iterator pos = lower_metier_suitable_seabottomtypes;
               pos != upper_metier_suitable_seabottomtypes; pos++)
              vect_of_the_metier_suitable_seabottomtypes_vovi.at(i).push_back(pos->second);

}



  cout << "export metier loaded data to simulator.cpp " << endl;

  //  export
  // for Metier() creator
  loadedData.vectiparam1 =name_metiers;
  loadedData.mapiiparam1 =metiers_types;
  loadedData.mapidparam1 =percent_revenue_completenesses;
  loadedData.vovovd1     =vect_of_selectivity_per_stock_ogives_vovovd;
  loadedData.vovd1       =vect_of_metier_betas_vovd;
  loadedData.vovd2       =vect_of_discardratio_limits_vovd;
  loadedData.vovi1       =vect_of_is_avoided_stocks_vovi;
  loadedData.vovi2       =vect_of_metier_mls_cat_vovi;
  loadedData.mapidparam2 =metiers_fspeed;
  loadedData.mapidparam3 =metiers_gear_widths_param_a;
  loadedData.mapidparam4 =metiers_gear_widths_param_b;
  loadedData.mapisparam1 =metiers_gear_widths_model_type;
  loadedData.vectmmapidparam1=vect_of_loss_after_1_passage_mmapid;
  loadedData.vovi1       =vect_of_the_metier_target_stocks_vovi;
  loadedData.vovi2       =vect_of_the_metier_suitable_seabottomtypes_vovi;

  // NOT for Metier() creator
  loadedData.vovd5       =selectivity_per_stock_ogives_for_oth_land;


return 0;
}

