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


/*

multimap<types::NodeId, double> read_gscale_cpue_per_stk_on_nodes(string a_quarter, string a_vessel, string folder_name_parameterization, string inputfolder)
{

    // frequence on ground of possible metiers for this vessels
    string filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/" + a_vessel +"_gscale_cpue_per_stk_on_nodes_"+a_quarter+".dat";
    ifstream vessels_gscale_cpue_per_stk_on_nodes;
    vessels_gscale_cpue_per_stk_on_nodes.open(filename.c_str());
    if(vessels_gscale_cpue_per_stk_on_nodes.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    //key=fground, value=gamma scale param
    multimap<int, double> gscale_cpue_per_stk_on_nodes;
    fill_multimap_from_specifications_i_d(vessels_gscale_cpue_per_stk_on_nodes,  gscale_cpue_per_stk_on_nodes);
    vessels_gscale_cpue_per_stk_on_nodes.close();

#ifdef VERBOSE
    // check input
    bool check = 0;
    if(check)
    {
        multimap<int,double>::iterator lower_gr = gscale_cpue_per_stk_on_nodes.lower_bound(1600);
        multimap<int,double>::iterator upper_gr = gscale_cpue_per_stk_on_nodes.upper_bound(1600);
        dout(cout << "for this vessel,  for this specific ground (1600): ");
        for (multimap<int, double>::iterator pos2=lower_gr; pos2 != upper_gr; pos2++)
        {
            dout(cout << pos2->second << " ");
        }
        dout(cout << endl);
    }
#endif

    return(types::helpers::toKeyIdMultimap<types::NodeId>(gscale_cpue_per_stk_on_nodes));
}

*/






int Dataloadermetiers::features(std::shared_ptr<sql::Storage> indb,
                                 const string& folder_name_parameterization,
                                 const string& inputfolder,
                                 PopSceOptions &dyn_pop_sce,
                                 DynAllocOptions &dyn_alloc_sce,
                                 string biolsce,
                                 string fleetsce,
                                 string& quarter,
                                 string& month,
                                 string& semester,
                                 int NBAGE,
                                 int NBSZGROUP,
                                LoadedData& loadedData)
{



   this->Dataloader::features(indb,
                            folder_name_parameterization,
                            inputfolder,
                            dyn_pop_sce,
                            dyn_alloc_sce,
                            biolsce,
                            fleetsce,
                            quarter,
                            month,
                            semester,
                            NBAGE,
                            NBSZGROUP,
                            loadedData);
  cout << "Loading metiers features" << endl;


/*
  // read general vessel features
  // (quarter specific, mainly because of the gamma parameters)
  vector<string> vesselids;
  vector<int> vid_is_actives;
  vector<int> vid_is_part_of_ref_fleets;
  vector<double> speeds;
  vector<double> fuelcons;
  vector<double> lengths;
  vector<double> vKWs;
  vector<double> carrycapacities;
  vector<double> tankcapacities;
  vector<double> nbfpingspertrips;
  vector<double> resttime_par1s;
  vector<double> resttime_par2s;
  vector<double> av_trip_duration;
  vector<double> mult_fuelcons_when_steaming;
  vector<double> mult_fuelcons_when_fishing;
  vector<double> mult_fuelcons_when_returning;
  vector<double> mult_fuelcons_when_inactive;
  vector<int> firm_ids;
  vector<VesselCalendar> calendars;

  // TODO: pass these as input to the function
  string a_quarter  = "quarter1";
  string a_month    = "month1";
  string a_semester = "semester1";
  int selected_vessels_only = 0; //use all vessels. if 1, then use a subset of vessels as defined in read_vessel_features()
  //...


  if (!read_vessels_features(a_quarter, vesselids, vid_is_actives, vid_is_part_of_ref_fleets,
                             speeds, fuelcons, lengths, vKWs,
                             carrycapacities, tankcapacities, nbfpingspertrips,
                             resttime_par1s, resttime_par2s, av_trip_duration,
                             mult_fuelcons_when_steaming, mult_fuelcons_when_fishing,
                             mult_fuelcons_when_returning, mult_fuelcons_when_inactive, firm_ids,
                             folder_name_parameterization, inputfolder, selected_vessels_only, calendars)) {
      std::cerr << "Cannot read vessel features.\n";
      return -1;
  }


  vector<double> this_vessel_nb_crews;
  vector<double> annual_other_incomes;
  vector<double> landing_costs_percents;
  vector<double> crewshare_and_unpaid_labour_costs_percents;
  vector<double> other_variable_costs_per_unit_efforts;
  vector<double> annual_insurance_costs_per_crews;
  vector<double> standard_labour_hour_opportunity_costss;
  vector<double> standard_annual_full_time_employement_hourss;
  vector<double> other_annual_fixed_costss;
  vector<double> vessel_values;
  vector<double> annual_depreciation_rates;
  vector<double> opportunity_interest_rates;
  vector<double> annual_discount_rates;

  cout << "read_vessels_economic_features() in loadVessels()" << endl;
  if (!read_vessels_economics_features(
          vesselids,
          this_vessel_nb_crews,
          annual_other_incomes,
          landing_costs_percents,
          crewshare_and_unpaid_labour_costs_percents,
          other_variable_costs_per_unit_efforts,
          annual_insurance_costs_per_crews,
          standard_labour_hour_opportunity_costss,
          standard_annual_full_time_employement_hourss,
          other_annual_fixed_costss,
          vessel_values,
          annual_depreciation_rates,
          opportunity_interest_rates,
          annual_discount_rates,
          folder_name_parameterization, inputfolder
  )) {
      std::cerr << "Cannot read vessel economic features.\n";
      return -1;
  }


  // read the more complex objects (i.e. when several info for a same vessel)...
  // also quarter specific but semester specific for the betas because of the survey design they are comning from...
  auto fgrounds = read_fgrounds(a_quarter, folder_name_parameterization, inputfolder);
  auto fgrounds_init = read_fgrounds_init(a_quarter, folder_name_parameterization, inputfolder);
  auto harbours = read_harbours(a_quarter, folder_name_parameterization, inputfolder);

  multimap<string, double> freq_fgrounds = read_freq_fgrounds(a_quarter, folder_name_parameterization, inputfolder);
  multimap<string, double> freq_fgrounds_init = read_freq_fgrounds_init(a_quarter, folder_name_parameterization,
                                                                        inputfolder);
  multimap<string, double> freq_harbours = read_freq_harbours(a_quarter, folder_name_parameterization, inputfolder);
  multimap<string, double> vessels_betas = read_vessels_betas(a_semester, folder_name_parameterization, inputfolder);
  multimap<string, double> vessels_tacs = read_vessels_tacs(a_semester, folder_name_parameterization, inputfolder);



  multimap<string, double> fishing_credits;
  if (dyn_alloc_sce.option(Options::fishing_credits)) {
      fishing_credits = read_initial_fishing_credits(folder_name_parameterization, inputfolder);
  }



  // loop over vids
  vector <multimap<types::NodeId, int> > vect_of_possible_metiers_mmap(vesselids.size());
  vector <multimap<types::NodeId, double> > vect_of_freq_possible_metiers_mmap(vesselids.size());
  vector <multimap<types::NodeId, double> > vect_of_gshape_cpue_per_stk_on_nodes_mmap(vesselids.size());
  vector <multimap<types::NodeId, double> > vect_of_gscale_cpue_per_stk_on_nodes_mmap(vesselids.size());

  for (unsigned int i = 0; i < vesselids.size(); i++)
  {
      outc(cout << "create vessel " << i << endl);

      // read vessel and quarter specific multimap
      // quarter specific to capture a piece of seasonality in the fishnig activity
      vect_of_possible_metiers_mmap.at(i) = read_possible_metiers(a_quarter, vesselids[i], folder_name_parameterization, inputfolder);
      vect_of_freq_possible_metiers_mmap.at(i) = read_freq_possible_metiers(a_quarter, vesselids[i], folder_name_parameterization,
                                                         inputfolder);

      //cpue_per_stk_on_nodes = read_cpue_per_stk_on_nodes(a_quarter, vesselids[i], folder_name_parameterization);
      vect_of_gshape_cpue_per_stk_on_nodes_mmap.at(i) = read_gshape_cpue_per_stk_on_nodes(a_quarter, vesselids[i],
                                                                       folder_name_parameterization, inputfolder);
      vect_of_gscale_cpue_per_stk_on_nodes_mmap.at(i) = read_gscale_cpue_per_stk_on_nodes(a_quarter, vesselids[i],
                                                                       folder_name_parameterization, inputfolder);



    }



  //  export
  loadedData.vectsparam1 =vesselids;
  loadedData.mmapsnparam1 =harbours;
  loadedData.mmapsnparam2 =fgrounds;
  loadedData.mmapsnparam3 =fgrounds_init;
  loadedData.mmapsdparam1 =freq_harbours;
  loadedData.mmapsdparam2 =freq_fgrounds;
  loadedData.mmapsdparam3 =freq_fgrounds_init;
  loadedData.mmapsdparam4 =vessels_betas;
  loadedData.mmapsdparam5 =vessels_tacs;
  loadedData.mmapsdparam6 =fishing_credits;
  loadedData.vectmmapniparam1=vect_of_possible_metiers_mmap;
  loadedData.vectmmapndparam1=vect_of_freq_possible_metiers_mmap;
  loadedData.vectmmapndparam2=vect_of_gshape_cpue_per_stk_on_nodes_mmap;
  loadedData.vectmmapndparam3=vect_of_gscale_cpue_per_stk_on_nodes_mmap;
  loadedData.vectiparam1=vid_is_actives;
  loadedData.vectiparam2=vid_is_part_of_ref_fleets;
  loadedData.vectdparam1=speeds;
  loadedData.vectdparam2=fuelcons;
  loadedData.vectdparam3=lengths;
  loadedData.vectdparam4=vKWs;
  loadedData.vectdparam5=carrycapacities;
  loadedData.vectdparam6=tankcapacities;
  loadedData.vectdparam7=nbfpingspertrips;
  loadedData.vectdparam8=resttime_par1s;
  loadedData.vectdparam9=resttime_par2s;
  loadedData.vectdparam10=av_trip_duration;
  loadedData.vectdparam11=mult_fuelcons_when_steaming;
  loadedData.vectdparam12=mult_fuelcons_when_fishing;
  loadedData.vectdparam13=mult_fuelcons_when_returning;
  loadedData.vectdparam14=mult_fuelcons_when_inactive;
  loadedData.vectiparam3=firm_ids;
  loadedData.vectcalendar1=calendars;
  loadedData.vectdparam16=this_vessel_nb_crews;
  loadedData.vectdparam17=annual_other_incomes;
  loadedData.vectdparam18=landing_costs_percents;
  loadedData.vectdparam19=crewshare_and_unpaid_labour_costs_percents;
  loadedData.vectdparam20=other_variable_costs_per_unit_efforts;
  loadedData.vectdparam21=annual_insurance_costs_per_crews;
  loadedData.vectdparam22=standard_labour_hour_opportunity_costss;
  loadedData.vectdparam23=standard_annual_full_time_employement_hourss;
  loadedData.vectdparam24=other_annual_fixed_costss;
  loadedData.vectdparam25=vessel_values;
  loadedData.vectdparam26=annual_depreciation_rates;
  loadedData.vectdparam27=opportunity_interest_rates;
  loadedData.vectdparam28=annual_discount_rates;


*/

return 0;
}

