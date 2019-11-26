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


/*
bool read_vessels_features(string a_quarter,
                           vector<string>& vesselids,
                           vector<int>& vid_is_actives,
                           vector<int>& vid_is_part_of_ref_fleets,
                           vector<double>& speeds,
                           vector<double>& fuelcons,
                           vector<double>& lengths,
                           vector<double>& vKWs,
                           vector<double>& carrycapacities,
                           vector<double>& tankcapacities,
                           vector<double>& nbfpingspertrips,
                           vector<double>& resttime_par1s,
                           vector<double>& resttime_par2s,
                           vector<double>& av_trip_duration,
                           vector<double>& mult_fuelcons_when_steaming,
                           vector<double>& mult_fuelcons_when_fishing,
                           vector<double>& mult_fuelcons_when_returning,
                           vector<double>& mult_fuelcons_when_inactive,
                           vector<int>& firm_ids,
                           string folder_name_parameterization,
                           string inputfolder,
                           int selected_vessels_only, vector<VesselCalendar> &calendars)
{

    string filename;
    if(selected_vessels_only==1)
    {
        filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_features_"+a_quarter+"_subset.dat";
    }
    else
    {

        filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_features_"+a_quarter+".dat";
    }
    ifstream vessels_features;
    vessels_features.open(filename.c_str());
    if(vessels_features.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }

    if (!fill_from_vessels_specifications(vessels_features, vesselids, vid_is_actives, vid_is_part_of_ref_fleets,
                                          speeds, fuelcons, lengths, vKWs,
                                          carrycapacities, tankcapacities, nbfpingspertrips,
                                          resttime_par1s, resttime_par2s, av_trip_duration,
                                          mult_fuelcons_when_steaming, mult_fuelcons_when_fishing,
                                          mult_fuelcons_when_returning, mult_fuelcons_when_inactive, firm_ids, calendars)) {
        vessels_features.close();
        return false;
    }

    vessels_features.close();


    return true;
}


//----------------
bool read_vessels_economics_features(
                           vector<string>& vesselids,
                           vector<double>& this_vessel_nb_crews,
                           vector<double>& annual_other_incomes,
                           vector<double>& landing_costs_percents,
                           vector<double>& crewshare_and_unpaid_labour_costs_percents,
                           vector<double>& other_variable_costs_per_unit_efforts,
                           vector<double>& annual_insurance_costs_per_crews,
                           vector<double>& standard_labour_hour_opportunity_costss,
                           vector<double>& standard_annual_full_time_employement_hourss,
                           vector<double>& other_annual_fixed_costss,
                           vector<double>& vessel_values,
                           vector<double>& annual_depreciation_rates,
                           vector<double>& opportunity_interest_rates,
                           vector<double>& annual_discount_rates,
                           string folder_name_parameterization,
                           string inputfolder
                           )
{

    string filename;
    filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/vesselsspe_economic_features.dat";

    ifstream vessels_economic_features;
    vessels_economic_features.open(filename.c_str());
    if(vessels_economic_features.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);

    }

    if (!fill_from_vessels_economic_specifications(vessels_economic_features,
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
                                                   annual_discount_rates)) {
        vessels_economic_features.close();
        return false;
    }

    vessels_economic_features.close();


    return true;
}

*/


void Dataloadervessels::features(std::shared_ptr<sql::Storage> indb,
                                 const string& folder_name_parameterization,
                                 const string& inputfolder,
                                 PopSceOptions &dyn_pop_sce,
                                 LoadedData& loadedData)
{


/*
    this->Dataloader::features(indb,
                             folder_name_parameterization,
                             inputfolder,
                             dyn_pop_sce,
                             loadedData);
  cout << "Loading vessels features" << endl;


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

  cout << "read_vessels_economic_features()" << endl;
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


  //  export
  loadedData.vectparam1 =spe_harbours;
  loadedData.vectparam2 =spe_fgrounds;
  loadedData.vectparam3 =spe_fgrounds_init;
  loadedData.vectparam4 =spe_freq_harbours;
  loadedData.vectparam5 =spe_freq_fgrounds;
  loadedData.vectparam6 =spe_freq_fgrounds_init;
  loadedData.vectparam7 =spe_vessel_betas_per_pop;
  loadedData.vectparam8 =spe_percent_tac_per_pop;
  loadedData.mmapniparam1=possible_metiers;
  loadedData.mmapndparam1=freq_possible_metiers;
  loadedData.int1=param8vid_is_actives[i];
  loadedData.int2=vid_is_part_of_ref_fleets[i];
  loadedData.double1=speeds[i];
  loadedData.double2=fuelcons[i];
  loadedData.double3=lengths[i];
  loadedData.double4=vKWs[i];
  loadedData.double5=carrycapacities[i];
  loadedData.double6=tankcapacities[i];
  loadedData.double7=nbfpingspertrips[i];
  loadedData.double8=resttime_par1s[i];
  loadedData.double9=resttime_par2s[i];
  loadedData.double10=av_trip_duration[i];
  loadedData.double11=mult_fuelcons_when_steaming[i];
  loadedData.double12=mult_fuelcons_when_fishing[i];
  loadedData.double13=mult_fuelcons_when_returning[i];
  loadedData.double14=mult_fuelcons_when_inactive[i];
  loadedData.double15=firm_ids[i];
  loadedData.calendar1=calendars[i];
  loadedData.double16=i < this_vessel_nb_crews.size() ? this_vessel_nb_crews[i] : 0;
  loadedData.double17=i < annual_other_incomes.size() ? annual_other_incomes[i] : 0;
  loadedData.double18=i < landing_costs_percents.size() ? landing_costs_percents[i] : 0;
  loadedData.double19=i < crewshare_and_unpaid_labour_costs_percents.size() ? crewshare_and_unpaid_labour_costs_percents[i] : 0;
  loadedData.double20=i < other_variable_costs_per_unit_efforts.size() ? other_variable_costs_per_unit_efforts[i] : 0;
  loadedData.double21=i < annual_insurance_costs_per_crews.size() ? annual_insurance_costs_per_crews[i] : 0;
  loadedData.double22=i < standard_labour_hour_opportunity_costss.size() ? standard_labour_hour_opportunity_costss[i] : 0;
  loadedData.double23=i < standard_annual_full_time_employement_hourss.size() ? standard_annual_full_time_employement_hourss[i] : 0;
  loadedData.double24=i < other_annual_fixed_costss.size() ? other_annual_fixed_costss[i] : 0;
  loadedData.double25=i < vessel_values.size() ? vessel_values[i] : 0;
  loadedData.double26=i < annual_depreciation_rates.size() ? annual_depreciation_rates[i] : 0;
  loadedData.double27=i < opportunity_interest_rates.size() ? opportunity_interest_rates[i] : 0;
  loadedData.double28=i < annual_discount_rates.size() ? annual_discount_rates[i] : 0;


 */


}

