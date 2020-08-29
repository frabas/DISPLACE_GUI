//
// Created by fuga on 02/11/2019.
//

#ifndef DISPLACE_VESSELSLOADER_H
#define DISPLACE_VESSELSLOADER_H


#include "myutils.h"
#include "utils/spimpl.h"

namespace msqlitecpp { namespace v2 { class Storage; }}

class VesselsLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    explicit VesselsLoader(msqlitecpp::v2::Storage &db);

    std::vector<std::string> getListOfAllVessels();

    map<int, double> getInitFuelPrices();

    struct VesselData {
        std::vector<types::NodeId> harbours;
        std::vector<types::NodeId> fground;
        std::map<int, double> fgroundFreq;
        std::vector<double> freq_harbours;
        std::vector<double> freq_fgrounds;
        std::vector<double> freq_fgrounds_init;
        std::vector<double> vessel_betas_per_pop;
        std::vector<double> percent_tac_per_pop;
        multimap<types::NodeId, int> possible_metiers;
        multimap<types::NodeId, double> freq_possible_metiers;
        multimap<types::NodeId, double> gshape_cpue_per_stk_on_nodes;
        multimap<types::NodeId, double> gscale_cpue_per_stk_on_nodes;
        int vid_is_active;
        int vid_is_part_of_ref_fleet;
        double speed;
        double fuelcons;
        double length;
        double KW;
        double carrycapacity;
        double tankcapacity;
        double nbfpingspertrip;
        double resttime_par1;
        double resttime_par2;
        double av_trip_duration;
        double mult_fuelcons_when_steaming;
        double mult_fuelcons_when_fishing;
        double mult_fuelcons_when_returning;
        double mult_fuelcons_when_inactive;
        int _firm_id;
        VesselCalendar cd;
        double this_vessel_nb_crew;
        double annual_other_income;
        double landing_costs_percent;
        double crewshare_and_unpaid_labour_costs_percent;
        double other_variable_costs_per_unit_effort;
        double annual_insurance_costs_per_crew;
        double standard_labour_hour_opportunity_costs;
        double standard_annual_full_time_employement_hours;
        double other_annual_fixed_costs;
        double vessel_value;
        double annual_depreciation_rate;
        double opportunity_interest_rate;
        double annual_discount_rate;
    };

    std::shared_ptr<VesselData> getVesselData(std::string vesselname, int period);

    std::vector<double> getEconomicFeature(std::string);
};


#endif //DISPLACE_VESSELSLOADER_H
