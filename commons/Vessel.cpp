// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include <idtypes.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#define PING_RATE 1				 // tstep=> 1 hour
#define NAUTIC 1.852			 // 1 knots=1.852 km
#define PI 3.14159265
#define NBSZGROUP 14

#include "Vessel.h"
#include <helpers.h>
#include <assert.h>

#include <Node.h>
#include <Benthos.h>
#include <Metier.h>
#include <Population.h>
#include <comstructs.h>

#include <dtree/dtnode.h>
#include <dtree/decisiontreemanager.h>
#include <dtree/externalstatemanager.h>
#include <dtree/commonstateevaluators.h>
#include <dtree/evaluators/timeseriesevaluator.h>
#include <dtree/variables.h>
#include <dtree/vesselsevaluators.h>

#include "shortestpath/AStarShortestPathFinder.h"

#include <boost/make_shared.hpp>

#include <functional>
#include <stdexcept>


std::mutex aStarMutex;
AStarShortestPathFinder aStarPathFinder;

std::vector<std::shared_ptr<dtree::StateEvaluator> > Vessel::mStateEvaluators;



vector<double> compute_distance_fgrounds_on_the_fly(vector<Node* >& nodes, types::NodeId from,
                                         vector<types::NodeId> grounds)
{
    outc (cout  << "look at the distances on the fly" << endl);

    outc (cout  << "look at the distances from node " <<   from.toIndex() << endl);

    vector <double> distance_fgrounds(grounds.size(), 0);
    for (unsigned int i=0; i<grounds.size(); i++)
      {
        auto vertex= nodes.at(grounds.at(i).toIndex());
        auto vertex2= nodes.at(from.toIndex());
        double dx = vertex->get_x() - vertex2->get_x();
        double dy = vertex->get_y() - vertex2->get_y();

        distance_fgrounds.at(i)=sqrt(dx * dx + dy * dy);
        if(grounds.at(i).toIndex()==from.toIndex()) distance_fgrounds.at(i)=1000; // avoid finding the lowest dist with itself
      }

    outc(cout  << "look at the distances from node " << from.toIndex()  <<"  ...OK "<< endl);

 return(distance_fgrounds);
}


Vessel::Vessel()
{
    idx_vessel = 0;
    init();
}


Vessel::~Vessel()
{

}

Vessel::Vessel(Node* p_location, int idx, string a_name)
{
    m_location = p_location;
    idx_vessel = idx;
    p_location->set_vid(idx_vessel);
    x =m_location->get_x();
    y =m_location->get_y();
    course=0;
    name = a_name;
    vid_is_active=1;
    vid_is_part_of_ref_fleet=0;
    inharbour = true;
    timeforrest = 0.0;
    state = 3;
    cumfuelcons=0;
    cumcatches=0;
    consotogetthere=0;
    carrycapacity=2000;
    tankcapacity=20000;
    nbfpingspertrip=3;
    message=0;

    init();
}


Vessel::Vessel(Node* p_location, int a_idx_vessel, string a_name,  int nbpops, int nbszgroups,
               const vector<types::NodeId> &_harbours, const vector<types::NodeId> &_fgrounds, const vector<types::NodeId> &_fgrounds_init,
               const vector<double> &_freq_harbours, const vector<double> &_freq_fgrounds, const vector<double> &_freq_fgrounds_init,
               const vector<double> &_vessel_betas_per_pop,
               const vector<double> &_percent_tac_per_pop,
               const multimap<types::NodeId, int> &_possible_metiers, const multimap<types::NodeId, double> &_freq_possible_metiers,
               int a_vid_is_active, int a_vid_is_part_of_ref_fleet, double a_speed, double a_fuelcons,  double a_length, double a_KW,
               double  a_carrycapacity, double a_tankcapacity, double a_nbfpingspertrip,
               double a_resttime_par1, double a_resttime_par2, double a_av_trip_duration,
               double _mult_fuelcons_when_steaming, double _mult_fuelcons_when_fishing,
               double _mult_fuelcons_when_returning, double _mult_fuelcons_when_inactive, int _firm_id, VesselCalendar cd,
               double _this_vessel_nb_crew,
               double _annual_other_income,
               double _landing_costs_percent,
               double _crewshare_and_unpaid_labour_costs_percent,
               double _other_variable_costs_per_unit_effort,
               double _annual_insurance_costs_per_crew,
               double _standard_labour_hour_opportunity_costs,
               double _standard_annual_full_time_employement_hours,
               double _other_annual_fixed_costs,
               double _vessel_value,
               double _annual_depreciation_rate,
               double _opportunity_interest_rate,
               double _annual_discount_rate)
    : calendar(cd)
{
    is_vessel_exited=0;
    m_location = p_location;
    idx_vessel = a_idx_vessel;
    firm_id=_firm_id;
    p_location->set_vid(a_idx_vessel);
    x =m_location->get_x();
    y =m_location->get_y();
    next_x=x;
    next_y=y;
    course=0;
    name = a_name;
    harbours = _harbours;		 // overwrite by the setter() in main...
    fgrounds = _fgrounds;		 // overwrite by the setter() in main...
    fgrounds_init = _fgrounds_init;		 // overwrite by the setter() in main...
    // overwrite by the setter() in main...
    freq_harbours = _freq_harbours;
    // overwrite by the setter() in main...
    freq_fgrounds = _freq_fgrounds;
    freq_fgrounds_init = _freq_fgrounds_init;
    // overwrite by the setter() in main...
    vessel_betas_per_pop = _vessel_betas_per_pop;
    // overwrite by the setter() in main...
    percent_tac_per_pop = _percent_tac_per_pop;
    // overwrite by the setter() in main...
    possible_metiers = _possible_metiers;
    // overwrite by the setter() in main...
    freq_possible_metiers = _freq_possible_metiers;
    vid_is_active=a_vid_is_active;
    vid_is_part_of_ref_fleet=a_vid_is_part_of_ref_fleet;
    speed = a_speed;			 //  *0.8; // CAUTION try to calib
    fuelcons = a_fuelcons;
    length = a_length;
    KW = a_KW;
    inharbour = true;
    timeforrest = 0.0;
    state = 3;
    tstep_dep = 0;
    cumfuelcons=0;
    cumcatches=0;
    cumsteaming=0;
    consotogetthere=0;
    reason_to_go_back=0;
    //  FILLED FROM DATA
    carrycapacity= a_carrycapacity;
    tankcapacity=a_tankcapacity; // FILLED FROM DATA
    //  FILLED FROM DATA
    nbfpingspertrip=a_nbfpingspertrip;
    // FILLED FROM DATA
    resttime_par1=a_resttime_par1;
    // FILLED FROM DATA
    resttime_par2=a_resttime_par2;
    // FILLED FROM DATA
    av_trip_duration=a_av_trip_duration;
    mult_fuelcons_when_steaming= _mult_fuelcons_when_steaming;// FILLED FROM DATA
    mult_fuelcons_when_fishing= _mult_fuelcons_when_fishing;// FILLED FROM DATA
    mult_fuelcons_when_returning= _mult_fuelcons_when_returning;// FILLED FROM DATA
    mult_fuelcons_when_inactive= _mult_fuelcons_when_inactive;// FILLED FROM DATA
    distprevpos=0;
    timeatsea=0;
    traveled_dist_this_trip=0;
    areasweptthistrip=0;
    inactive=true;
    natio=true;					 // FILLED FROM DATA
    message=0;					 // no message
    previous_harbour_idx=p_location->get_idx_node();
    targeting_non_tac_pop_only=1;// init at 1

    // deduce the vessel nationality from the vessel name
    nationality = nationalityFromName(get_name());

    // init individual tac
    for(int i = 0; i < nbpops; i++)
    {
        individual_tac_per_pop.push_back(0);
        individual_tac_per_pop_at_year_start.push_back(0);
        prop_remaining_individual_quotas.push_back(1); // caution: with start with 1 for all even if no quota as it is a decrease that will be detected when choosing the min prop....
        prop_remaining_global_quotas.push_back(1); // caution: with start with 1 for all even if no quota as it is a decrease that will be detected when choosing the min prop....
        is_choked.push_back(0); // inform when the quota is exhausted by stock
    }

    // init at 0 the matrix of catches
    dout(cout  << "init matrix of catches" << endl);
    vector< vector<double> > init_catch_pop_at_szgroup(nbpops, vector<double>(nbszgroups));// trip based landings...
    vector< vector<double> > init_ping_catch_pop_at_szgroup(nbpops, vector<double>(nbszgroups)); // ...instantaneous catches (land+disc)
    vector< vector<double> > init_discards_pop_at_szgroup(nbpops, vector<double>(nbszgroups));
    catch_pop_at_szgroup= init_catch_pop_at_szgroup;
    ping_catch_pop_at_szgroup=init_ping_catch_pop_at_szgroup;
    discards_pop_at_szgroup= init_discards_pop_at_szgroup;
    for(int i = 0; i < nbpops; i++)
    {

        for(int j = 0; j < nbszgroups; j++)
        {

            catch_pop_at_szgroup[i][j] = 0;
            ping_catch_pop_at_szgroup[i][j] = 0;
            //dout(cout  << catch_pop_at_szgroup[i][j] << " ");
            discards_pop_at_szgroup[i][j] = 0;
            //dout(cout  << discards_pop_at_szgroup[i][j] << " ");
        }
        //dout(cout  << endl);
    }

    // length class
    if(length<15) {
        length_class = 0; //"u15m";
        mLengthClassId = Under15;
    } else if(length>=15 && length <18) {
        length_class = 1; //"15-18m";
        mLengthClassId = Between15and18;
    } else if(length>=18 && length <24) {
        length_class = 2; // "18-24m";
        mLengthClassId = Between18and24;
    } else if(length>=24 && length <40) {
        length_class = 3; //"24-40m";
        mLengthClassId = Between24and40;
    } else if(length>=40) {
        length_class = 4; // "o40m";
        mLengthClassId = Over40;
    }

    this_vessel_nb_crew=_this_vessel_nb_crew;
    annual_other_income=_annual_other_income;
    landing_costs_percent= _landing_costs_percent;
    crewshare_and_unpaid_labour_costs_percent= _crewshare_and_unpaid_labour_costs_percent;
    other_variable_costs_per_unit_effort= _other_variable_costs_per_unit_effort;
    annual_insurance_costs_per_crew= _annual_insurance_costs_per_crew;
    standard_labour_hour_opportunity_costs= _standard_labour_hour_opportunity_costs;
    standard_annual_full_time_employement_hours= _standard_annual_full_time_employement_hours;
    other_annual_fixed_costs= _other_annual_fixed_costs;
    vessel_value= _vessel_value;
    annual_depreciation_rate= _annual_depreciation_rate;
    opportunity_interest_rate= _opportunity_interest_rate;
    annual_discount_rate= _annual_discount_rate;



    dout(cout <<"vessel creator...OK" << endl);
    init();
}


void Vessel::init()
{
    lastTrip_revenues = lastTrip_explicit_revenues = lastTrip_profit = avgRevenues = avgProfit = 0;
    numTrips = 0;

    nationality = nationalityFromName(get_name());

    // Lazy initialization of the global State Evaluator.
    if (mStateEvaluators.size() == 0) {
        for (int i = 0; i < dtree::VarLast; ++i) {
            mStateEvaluators.push_back(std::shared_ptr<dtree::StateEvaluator>());
        }

        // Add here the variables associations
        // GoFishing
        mStateEvaluators[dtree::vesselMetierIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::MetierStateEvaluator);
        mStateEvaluators[dtree::vesselSizeIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselSizeStateEvaluator);
        mStateEvaluators[dtree::stockTargetIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselTargetEvaluator);
        mStateEvaluators[dtree::lastTripRevenueIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::AverageRevenuesComparationStateEvaluator());
        mStateEvaluators[dtree::lastTripProfitIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::AverageProfitComparationsStateEvaluator());
        mStateEvaluators[dtree::vesselMetierIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::MetierStateEvaluator());
        mStateEvaluators[dtree::fuelPriceIs] = std::shared_ptr<dtree::StateEvaluator>(new displace::dtree::TimeSeriesEvaluator<displace::simulation::TimeSeriesManager::Fuelprice>());
        mStateEvaluators[dtree::fishPriceTargetStockIs] = std::shared_ptr<dtree::StateEvaluator>(new displace::dtree::TimeSeriesEvaluator<displace::simulation::TimeSeriesManager::Fishprice>());
        mStateEvaluators[dtree::windSpeedIs] = std::shared_ptr<dtree::StateEvaluator>(new displace::dtree::TimeSeriesEvaluator<displace::simulation::TimeSeriesManager::WSpeed>());
        mStateEvaluators[dtree::todayIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselTodayIsStateEvaluator);
        mStateEvaluators[dtree::monthIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselMonthIsStateEvaluator);
        mStateEvaluators[dtree::riskOfBycatchAvoidedStksNowIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselRiskOfBycatchAvoidedStksNowIsStateEvaluator);
        mStateEvaluators[dtree::individualQuotaLeftOnAvoidedStksNowIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselindividualQuotaLeftOnAvoidedStksNowIsStateEvaluator);
        mStateEvaluators[dtree::globalQuotaLeftOnAvoidedStksNowIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselglobalQuotaLeftOnAvoidedStksNowIsStateEvaluator);

        // StartFishing (on this ground)
        mStateEvaluators[dtree::suitableBottomTypeIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselsuitableBottomTypeIsStateEvaluator);
        mStateEvaluators[dtree::riskOfBycatchAvoidedStksHereIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselRiskOfBycatchAvoidedStksHereIsStateEvaluator);
        mStateEvaluators[dtree::individualQuotaLeftOnAvoidedStksHereIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselindividualQuotaLeftOnAvoidedStksHereIsStateEvaluator);
        mStateEvaluators[dtree::globalQuotaLeftOnAvoidedStksHereIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselglobalQuotaLeftOnAvoidedStksHereIsStateEvaluator);


        // ChangeGround
        mStateEvaluators[dtree::feelingForCatchingElsewhere] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselFeelingForCatchingElsewhereStateEvaluator);
        mStateEvaluators[dtree::seeingOtherVesselFishingElsewhere] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselSeeingOtherVesselFishingElsewhereStateEvaluator);



        // StopFishing
        mStateEvaluators[dtree::fuelTankIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselFuelTankStateEvaluator);
        mStateEvaluators[dtree::catchVolumeIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselCatchVolumeStateEvaluator);
        mStateEvaluators[dtree::nbOfDaysAtSeaSoFarIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselNbOfDaysAtSeaSoFarIsStateEvaluator);
        mStateEvaluators[dtree::endOfTheDayIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselEndOfTheDayIsStateEvaluator);


        // chooseGround
        mStateEvaluators[dtree::smartCatch] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselSmartCatchStateEvaluator);
        mStateEvaluators[dtree::highPotentialCatch] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselHighPotentialCatchStateEvaluator);
        mStateEvaluators[dtree::notThatFar] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselNotThatFarStateEvaluator);
        mStateEvaluators[dtree::knowledgeOfThisGround] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselKnowledgeOfThisGroundStateEvaluator);
        mStateEvaluators[dtree::riskOfBycatchAllStksThisGroundIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselRiskOfBycatchAllStksIsStateEvaluator);
        mStateEvaluators[dtree::riskOfBycatchAvoidedStksThisGroundIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselRiskOfBycatchAvoidedStksIsStateEvaluator);
        mStateEvaluators[dtree::individualQuotaLeftOnAvoidedStksIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselindividualQuotaLeftOnAvoidedStksIsStateEvaluator);
        mStateEvaluators[dtree::globalQuotaLeftOnAvoidedStksIs] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselglobalQuotaLeftOnAvoidedStksIsStateEvaluator);
        mStateEvaluators[dtree::isInAreaClosure] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselIsInAreaClosureEvaluator);
        mStateEvaluators[dtree::lowestTariff] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselLowestTariffStateEvaluator);
        mStateEvaluators[dtree::avoidHighTariffAreas] =
                std::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselAvoidHighTariffAreasStateEvaluator);




    }

}

Vessel::Vessel(string name, Node* a_location)
    : name(name), m_location(a_location)
{
}


//------------------------------------------------------------//
//------------------------------------------------------------//
// getters...
//------------------------------------------------------------//
//------------------------------------------------------------//

int Vessel::get_idx () const
{
    return(idx_vessel);
}

int Vessel::get_firm_id () const
{
    return(firm_id);
}


int Vessel::read_message () const
{
    return(message);
}


string Vessel::get_name () const
{
    return(name);
}

int Vessel::get_vid_is_active () const
{
    return(vid_is_active);
}

int Vessel::get_vid_is_part_of_ref_fleet() const
{
    return(vid_is_part_of_ref_fleet);
}

Node* Vessel::get_loc() const
{
    return(m_location);
}


Metier* Vessel::get_metier() const
{
    return(metier);
}


double Vessel::get_speed () const
{
    return(speed);
}


double Vessel::get_length () const
{
    return(length);
}


double Vessel::get_KW () const
{
    return(KW);
}


int Vessel::get_length_class() const
{
    return(length_class);
}


string Vessel::get_nationality () const
{
    return(nationality);
}


const vector<types::NodeId> &Vessel::get_harbours() const
{
    return(harbours);
}


const vector<types::NodeId> &Vessel::get_fgrounds() const
{
    return(fgrounds);
}

const vector<types::NodeId> &Vessel::get_fgrounds_init() const
{
    return(fgrounds_init);
}


vector<types::NodeId> &Vessel::get_fgrounds_in_closed_areas()
{
    return(fgrounds_in_closed_areas);
}


const std::vector<Node* > &Vessel::get_map_of_nodes() const
 {
     return(map_of_nodes);
 }


const vector<double> &Vessel::get_freq_harbours() const
{
    return(freq_harbours);
}


const vector<double> &Vessel::get_freq_fgrounds() const
{
    return(freq_fgrounds);
}

const vector<double> &Vessel::get_freq_fgrounds_init() const
{
    return(freq_fgrounds_init);
}


const vector<double> &Vessel::get_cumcatch_fgrounds() const
{
    return(cumcatch_fgrounds);
}

const vector<double> &Vessel::get_cumdiscard_fgrounds() const
{
    return(cumdiscard_fgrounds);
}

const vector<double> &Vessel::get_experienced_bycatch_prop_on_fgrounds () const
{
    return(experienced_bycatch_prop_on_fgrounds);
}

const vector<double> &Vessel::get_experienced_avoided_stks_bycatch_prop_on_fgrounds () const
{
    return(experienced_avoided_stks_bycatch_prop_on_fgrounds);
}


types::NodeId Vessel::get_previous_harbour_idx () const
{
    return(previous_harbour_idx);
}


const vector<vector<double> > &Vessel::get_cumcatch_fgrounds_per_pop() const
{
    return(cumcatch_fgrounds_per_pop);
}

const vector<vector<double> > &Vessel::get_cumdiscard_fgrounds_per_pop() const
{
    return(cumdiscard_fgrounds_per_pop);
}

const vector<double> &Vessel::get_cumeffort_fgrounds() const
{
    return(cumeffort_fgrounds);
}


const vector<double> &Vessel::get_experiencedcpue_fgrounds()const
{
    return(experiencedcpue_fgrounds);
}


const vector<vector<double> > &Vessel::get_experiencedcpue_fgrounds_per_pop()const
{
    return(experiencedcpue_fgrounds_per_pop);
}


const vector<double> &Vessel::get_freq_experiencedcpue_fgrounds()const
{
    return(freq_experiencedcpue_fgrounds);
}


const vector<vector<double> > &Vessel::get_freq_experiencedcpue_fgrounds_per_pop()const
{
    return(freq_experiencedcpue_fgrounds_per_pop);
}


const vector<double> &Vessel::get_vessel_betas_per_pop() const
{
    return(vessel_betas_per_pop);
}


const vector<double> &Vessel::get_percent_tac_per_pop () const
{
    return(percent_tac_per_pop);
}

const vector<double> &Vessel::get_fishing_credits () const
{
    return(fishing_credits);
}

const multimap<types::NodeId, int> &Vessel::get_possible_metiers() const
{
    return(possible_metiers);
}


const multimap<types::NodeId, double> &Vessel::get_freq_possible_metiers() const
{
    return(freq_possible_metiers);
}


const list<types::NodeId> &Vessel::get_roadmap() const
{
    return(roadmap);
}


bool Vessel::get_inharbour() const
{
    return(inharbour);
}


bool Vessel::get_inactive() const
{
    return(inactive);
}

int Vessel::get_is_vessel_exited() const
{
    return(is_vessel_exited);
}


bool Vessel::get_natio() const
{
    return(natio);
}


int Vessel::get_state () const
{
    return(state);
}


int Vessel::get_reason_to_go_back () const
{
    return(reason_to_go_back);
}


int Vessel::get_tstep_dep () const
{
    return(tstep_dep);
}


double Vessel::get_x () const
{
    return(x);
}


double Vessel::get_y () const
{
    return(y);
}


double Vessel::get_course () const
{
    return(course);
}


double Vessel::get_fuelcons () const
{
    return(fuelcons);
}


double Vessel::get_cumfuelcons () const
{
    return(cumfuelcons);
}

double Vessel::get_sweptareathistrip () const
{
    return(areasweptthistrip);
}

double Vessel::get_subsurfacesweptareathistrip () const
{
    return(subsurfaceareasweptthistrip);
}

double Vessel::get_consotogetthere () const
{
    return(consotogetthere);
}


double Vessel::get_distprevpos () const
{
    return(distprevpos);
}


double Vessel::get_timeforrest () const
{
    return(timeforrest);
}


double Vessel::get_timeatsea () const
{
    return(timeatsea);
}

double Vessel::get_mult_fuelcons_when_steaming () const
{
    return(mult_fuelcons_when_steaming);
}

double Vessel::get_mult_fuelcons_when_fishing () const
{
    return(mult_fuelcons_when_fishing);
}

double Vessel::get_mult_fuelcons_when_returning () const
{
    return(mult_fuelcons_when_returning);
}

double Vessel::get_mult_fuelcons_when_inactive () const
{
    return(mult_fuelcons_when_inactive);
}


double Vessel::get_traveled_dist_this_trip () const
{
    return(traveled_dist_this_trip);
}


double Vessel::get_tankcapacity () const
{
    return(tankcapacity);
}


double Vessel::get_carrycapacity () const
{
    return(carrycapacity);
}


int Vessel::get_nbfpingspertrip () const
{
    return(nbfpingspertrip);
}


double Vessel::get_resttime_par1 () const
{
    return(resttime_par1);
}


double Vessel::get_resttime_par2 () const
{
    return(resttime_par2);
}


double Vessel::get_av_trip_duration () const
{
    return(av_trip_duration);
}


const vector<int> &Vessel::get_idx_used_metiers_this_trip()
{
    return(idx_used_metiers_this_trip);
}


double Vessel::get_cumcatches () const
{
    return(cumcatches);
}

double Vessel::get_cumdiscards () const
{
    return(cumdiscards);
}

double Vessel::get_cumsteaming () const
{
    return(cumsteaming);
}


const vector<vector<double> > &Vessel::get_catch_pop_at_szgroup() const
{
    return(catch_pop_at_szgroup);
}

const vector<vector<double> > &Vessel::get_ping_catch_pop_at_szgroup() const
{
    return(ping_catch_pop_at_szgroup);
}


const vector<vector<double> > &Vessel::get_discards_pop_at_szgroup() const
{
    return(discards_pop_at_szgroup);
}

const vector<vector<double> > &Vessel::get_gshape_cpue_nodes_species() const
{
    return(gshape_cpue_nodes_species);

}


const vector<vector<double> > &Vessel::get_gscale_cpue_nodes_species() const
{
    return(gscale_cpue_nodes_species);

}

int Vessel::get_individual_tac (int sp) const
{
    return(individual_tac_per_pop.at(sp));
}

int Vessel::get_individual_tac_per_pop_at_year_start (int sp) const
{
    return(individual_tac_per_pop_at_year_start.at(sp));
}

double Vessel::get_prop_remaining_individual_quotas (int sp) const
{
    return(prop_remaining_individual_quotas.at(sp));
}

double Vessel::get_prop_remaining_global_quotas (int sp) const
{
    return(prop_remaining_global_quotas.at(sp));
}

vector<double> Vessel::get_is_choked () const
{
    return(is_choked);
}


double Vessel::get_min_prop_remaining_individual_quotas_on_avoided_stks ()
{
    vector<int> avoided_stocks=this->get_metier()->get_is_avoided_stocks();

    // for looking for the min prop of quota left but only within the avoided_stocks subset...
    vector<double> prop_remaining_individual_quotas_for_avoided_stks;
    for (int stk=0; stk<prop_remaining_individual_quotas.size();++stk){
          if(avoided_stocks.at(stk)) prop_remaining_individual_quotas_for_avoided_stks.push_back(prop_remaining_individual_quotas.at(stk));
    }

    if(prop_remaining_individual_quotas_for_avoided_stks.size()==0) return(1.0);

    return( *min_element(prop_remaining_individual_quotas_for_avoided_stks.begin(),
                         prop_remaining_individual_quotas_for_avoided_stks.end()) );
}

double Vessel::get_min_prop_remaining_individual_quotas ()
{

    return( *min_element(prop_remaining_individual_quotas.begin(),
                         prop_remaining_individual_quotas.end()) );
}


double Vessel::get_min_prop_remaining_global_quotas_on_avoided_stks ()
{
    vector<int> avoided_stocks=this->get_metier()->get_is_avoided_stocks();



    // for looking for the min prop of quota left but only within the avoided_stocks subset...
    vector<double> prop_remaining_global_quotas_for_avoided_stks;
    for (int stk=0; stk<prop_remaining_global_quotas.size();++stk){
//cout << "prop_remaining_global_quotas.at(stk) is " << prop_remaining_global_quotas.at(stk) << " and avoided_stocks.at(stk) " << avoided_stocks.at(stk) << endl;
          if(avoided_stocks.at(stk)) prop_remaining_global_quotas_for_avoided_stks.push_back(prop_remaining_global_quotas.at(stk));
    }

    if(prop_remaining_global_quotas_for_avoided_stks.size()==0) return(1.0);

    return( *min_element(prop_remaining_global_quotas_for_avoided_stks.begin(),
                         prop_remaining_global_quotas_for_avoided_stks.end()) );
 }

double Vessel::get_min_prop_remaining_global_quotas ()
{

    return( *min_element(prop_remaining_global_quotas.begin(),
                         prop_remaining_global_quotas.end()) );
}


int Vessel::get_targeting_non_tac_pop_only () const
{
    return(targeting_non_tac_pop_only);
}

types::NodeId Vessel::get_smartcatch () const
{
    return(smartcatch);
}

types::NodeId  Vessel::get_highpotentialcatch () const
{
    return(highpotentialcatch);
}

types::NodeId  Vessel::get_notthatfar () const
{
    return(notthatfar);
}

types::NodeId  Vessel::get_lowesttariff () const
{
    return(lowesttariff);
}

types::NodeId  Vessel::get_mosthistoricallyused () const
{
    return(mosthistoricallyused);
}

double Vessel::get_GVA() const
{
    return(GVA);
}

double Vessel::get_GVAPerRevenue() const
{
    return(GVAPerRevenue);
}

double Vessel::get_LabourSurplus() const
{
    return(LabourSurplus);
}

double Vessel::get_GrossProfit() const
{
    return(GrossProfit);
}

double Vessel::get_NetProfit() const
{
    return(NetProfit);
}

double Vessel::get_NetProfitMargin() const
{
    return(NetProfitMargin);
}

double Vessel::get_GVAPerFTE() const
{
    return(GVAPerFTE);
}

double Vessel::get_RoFTA() const
{
    return(RoFTA);
}

double Vessel::get_BER() const
{
    return(BER);
}

double Vessel::get_CRBER() const
{
    return(CRBER);
}

double Vessel::get_NetPresentValue() const
{
    return(NetPresentValue);
}

int Vessel::get_numTrips() const
{
    return(numTrips);
}

double Vessel::get_this_vessel_nb_crew() const
{
    return(this_vessel_nb_crew);
}

double Vessel::get_annual_other_income() const
{
    return(annual_other_income);
}

double Vessel::get_landing_costs_percent() const
{
    return(landing_costs_percent);
}

double Vessel::get_crewshare_and_unpaid_labour_costs_percent() const
{
    return(crewshare_and_unpaid_labour_costs_percent);
}

double Vessel::get_other_variable_costs_per_unit_effort() const
{
    return(other_variable_costs_per_unit_effort);
}

double Vessel::get_annual_insurance_costs_per_crew() const
{
    return(annual_insurance_costs_per_crew);
}

double Vessel::get_standard_labour_hour_opportunity_costs() const
{
    return(standard_labour_hour_opportunity_costs);
}

double Vessel::get_standard_annual_full_time_employement_hours() const
{
    return(standard_annual_full_time_employement_hours);
}

double Vessel::get_other_annual_fixed_costs() const
{
    return(other_annual_fixed_costs);
}

double Vessel::get_vessel_value() const
{
    return(vessel_value);
}

double Vessel::get_annual_depreciation_rate() const
{
    return(annual_depreciation_rate);
}

double Vessel::get_opportunity_interest_rate() const
{
    return(opportunity_interest_rate);
}

double Vessel::get_annual_discount_rate() const
{
    return(annual_discount_rate);
}

double Vessel::getDaysSpentInRestrictedAreaThisMonth(int met_idx)
{
    return(daysSpentInRestrictedAreaThisMonth.at(met_idx));
}


//------------------------------------------------------------//
//------------------------------------------------------------//
// setters...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::addADayPortionToDaysSpentInRestrictedAreaThisMonth(int idx_met, double a_portion)
{
    daysSpentInRestrictedAreaThisMonth.at(idx_met)+=a_portion;
}

void Vessel::reinitDaysSpentInRestrictedAreaThisMonthtoZero()
{
   //cout << "the daysSpentInRestrictedAreaThisMonth size is " << daysSpentInRestrictedAreaThisMonth.size() << endl;
    for (int i=0; i<daysSpentInRestrictedAreaThisMonth.size();++i)
        daysSpentInRestrictedAreaThisMonth.at(i)=0.0;
}

void Vessel::set_previous_harbour_idx(types::NodeId _previous_harbour_idx)
{
    previous_harbour_idx= _previous_harbour_idx;
}

void Vessel::set_firm_id(int _firm_id)
{
    firm_id= _firm_id;
}


void Vessel::set_vid_is_active(int _vid_is_active)
{
    vid_is_active= _vid_is_active;
}

void Vessel::set_vid_is_part_of_ref_fleet(int _vid_is_part_of_ref_fleet)
{
    vid_is_part_of_ref_fleet= _vid_is_part_of_ref_fleet;
}


void Vessel::set_speed(double _speed)
{
    speed= _speed;
}

void Vessel::set_vessel_value(double _vessel_value)
{
    vessel_value= _vessel_value;
}


void Vessel::set_fuelcons(double _fuelcons)
{
    fuelcons= _fuelcons;
}


void Vessel::set_resttime_par1(double _resttime_par1)
{
    resttime_par1= _resttime_par1;
}


void Vessel::set_resttime_par2(double _resttime_par2)
{
    resttime_par2= _resttime_par2;
}


void Vessel::set_av_trip_duration(double _av_trip_duration)
{
    av_trip_duration= _av_trip_duration;
}


void Vessel::set_course(double _course)
{
    course= _course;
}


void Vessel::receive_message(int _message)
{
    message= _message;
}


void Vessel::reset_message()
{
    message= 0;
}

void Vessel::set_map_of_nodes (const vector<Node* > &_map_of_nodes)
{
    map_of_nodes=_map_of_nodes;
}

void Vessel::set_spe_harbours (const vector<types::NodeId> &_harbours)
{
    harbours= _harbours;
}


void Vessel::set_spe_fgrounds (const vector<types::NodeId> &_fgrounds)
{
    fgrounds=_fgrounds;
}

void Vessel::set_spe_fgrounds_init (const vector<types::NodeId> &_fgrounds_init)
{
    fgrounds_init=_fgrounds_init;
}


void Vessel::set_fgrounds_in_closed_areas (const vector<types::NodeId> &_fgrounds)
{
    fgrounds_in_closed_areas=_fgrounds;
}



void Vessel::set_spe_freq_harbours (const vector<double> &_freq_harbours)
{
    freq_harbours= _freq_harbours;
}


void Vessel::set_spe_freq_fgrounds (const vector<double> &_freq_fgrounds)
{
    freq_fgrounds=_freq_fgrounds;
}

void Vessel::set_spe_freq_fground(int index, double _fground)
{
    freq_fgrounds.at(index) = _fground;
}

void Vessel::set_spe_freq_fgrounds_init (const vector<double> &_freq_fgrounds_init)
{
    freq_fgrounds=_freq_fgrounds_init;
}


void Vessel::set_spe_cumcatch_fgrounds (const vector<double> &_cumcatch_fgrounds)
{
    cumcatch_fgrounds=_cumcatch_fgrounds;
}

void Vessel::set_spe_cumdiscard_fgrounds (const vector<double> &_cumdiscard_fgrounds)
{
    cumdiscard_fgrounds=_cumdiscard_fgrounds;
}


void Vessel::set_spe_experienced_bycatch_prop_on_fgrounds (const vector<double> &_experienced_bycatch_prop_on_fgrounds)
{
    experienced_bycatch_prop_on_fgrounds=_experienced_bycatch_prop_on_fgrounds;
}

void Vessel::set_spe_experienced_avoided_stks_bycatch_prop_on_fgrounds (const vector<double> &_experienced_avoided_stks_bycatch_prop_on_fgrounds)
{
    experienced_avoided_stks_bycatch_prop_on_fgrounds=_experienced_avoided_stks_bycatch_prop_on_fgrounds;
}


void Vessel::set_spe_cumeffort_fgrounds (const vector<double> &_cumeffort_fgrounds)
{
    cumeffort_fgrounds=_cumeffort_fgrounds;
}


void Vessel::set_spe_experiencedcpue_fgrounds (const vector<double> &_experiencedcpue_fgrounds)
{
    experiencedcpue_fgrounds=_experiencedcpue_fgrounds;
}


void Vessel::set_spe_betas_per_pop (const vector<double> &_betas_per_pop)
{
    vessel_betas_per_pop=_betas_per_pop;
}


void Vessel::set_spe_percent_tac_per_pop (const vector<double> &_tacs_per_pop)
{
    percent_tac_per_pop=_tacs_per_pop;
}


void Vessel::set_fishing_credits (const vector<double> &_fishing_credits)
{
    fishing_credits=_fishing_credits;
}


void Vessel::set_spe_possible_metiers (const multimap<types::NodeId, int> &_possible_metiers)
{
    possible_metiers=_possible_metiers;

    if(_possible_metiers.size()>0){

    // find max idx metier among possible metiers this vessel is having
    auto max_idx_possible_metiers = std::max_element(_possible_metiers.begin(), _possible_metiers.end(),
        [](const pair<types::NodeId, int>& p1, const pair<types::NodeId, int>& p2) {
            return p1.second < p2.second; });

    // then fill in with 0 up to this idx
    while (daysSpentInRestrictedAreaThisMonth.size() <= max_idx_possible_metiers->second)
        daysSpentInRestrictedAreaThisMonth.push_back(0.0);
   }
   else
    {
     daysSpentInRestrictedAreaThisMonth.push_back(0.0);
    }

}


void Vessel::set_spe_freq_possible_metiers  (const multimap<types::NodeId, double> &_freq_possible_metiers)
{
    freq_possible_metiers=_freq_possible_metiers;
}


void Vessel::init_cpue_nodes_species(int nbnodes, int nbspecies)
{

    // init at 0 the matrix of cpues
    dout(cout  << "init matrix of cpues" << endl);
    vector< vector<double> > init_cpue_nodes_species(nbnodes, vector<double>(nbspecies));
    cpue_nodes_species= init_cpue_nodes_species;
    for(int i = 0; i < nbnodes; i++)
    {

        for(int j = 0; j < nbspecies; j++)
        {

            cpue_nodes_species.at(i).at(j) = 0;
            //dout(cout  << cpue_nodes_species[i][j] << " ");
        }
        //dout(cout  << endl);
    }

}


void Vessel::init_gshape_cpue_nodes_species(int nbnodes, int nbspecies)
{

    // init at 0 the matrix of cpues
    dout(cout  << "init matrix of gshape cpues" << endl);
    vector< vector<double> > init_gshape_cpue_nodes_species(nbnodes, vector<double>(nbspecies));
    gshape_cpue_nodes_species= init_gshape_cpue_nodes_species;
    for(int i = 0; i < nbnodes; i++)
    {

        for(int j = 0; j < nbspecies; j++)
        {

            gshape_cpue_nodes_species.at(i).at(j) = 0;
            //dout(cout  << gshape_cpue_nodes_species[i][j] << " ");
        }
        //dout(cout  << endl);
    }

}


void Vessel::init_gscale_cpue_nodes_species(int nbnodes, int nbspecies)
{

    // init at 0 the matrix of cpues
    dout(cout  << "init matrix of gscale cpues" << endl);
    vector< vector<double> > init_gscale_cpue_nodes_species(nbnodes, vector<double>(nbspecies));
    gscale_cpue_nodes_species= init_gscale_cpue_nodes_species;
    for(int i = 0; i < nbnodes; i++)
    {

        for(int j = 0; j < nbspecies; j++)
        {

            gscale_cpue_nodes_species.at(i).at(j) = 0;
            //dout(cout  << gscale_cpue_nodes_species[i][j] << " ");
        }
        //dout(cout  << endl);
    }

}


void Vessel::set_cpue_nodes_species(int idx_node, const vector<double> &newval)
{
    for(unsigned int sp = 0; sp < cpue_nodes_species[idx_node].size(); sp++)
    {
        cpue_nodes_species[idx_node][sp]= newval[sp];
    }

}


void Vessel::set_gshape_cpue_nodes_species(int idx_node, const vector<double> &newval)
{
    for(unsigned int sp = 0; sp < gshape_cpue_nodes_species[idx_node].size(); sp++)
    {
        gshape_cpue_nodes_species[idx_node][sp]= newval[sp];
    }

}


void Vessel::set_gscale_cpue_nodes_species(int idx_node, const vector<double> &newval)
{
    for(unsigned int sp = 0; sp < gscale_cpue_nodes_species[idx_node].size(); sp++)
    {
        gscale_cpue_nodes_species[idx_node][sp]= newval[sp];
    }

}


void Vessel::set_cumcatch_fgrounds (const vector<double>  &_cumcatch_fgrounds)
{
    cumcatch_fgrounds=_cumcatch_fgrounds;
}


void Vessel::set_cumdiscard_fgrounds (const vector<double>  &_cumdiscard_fgrounds)
{
    cumdiscard_fgrounds=_cumdiscard_fgrounds;
}


void Vessel::set_experienced_bycatch_prop_on_fgrounds (const vector<double>  &_experienced_bycatch_prop_on_fgrounds)
{
    experienced_bycatch_prop_on_fgrounds=_experienced_bycatch_prop_on_fgrounds;
}

void Vessel::set_experienced_avoided_stks_bycatch_prop_on_fgrounds (const vector<double>  &_experienced_avoided_stks_bycatch_prop_on_fgrounds)
{
    experienced_avoided_stks_bycatch_prop_on_fgrounds=_experienced_avoided_stks_bycatch_prop_on_fgrounds;
}



void Vessel::set_cumcatch_fgrounds_per_pop (const vector<vector<double> >  &_cumcatch_fgrounds_per_pop)
{
    cumcatch_fgrounds_per_pop=_cumcatch_fgrounds_per_pop;
}

void Vessel::set_cumdiscard_fgrounds_per_pop (const vector<vector<double> >  &_cumdiscard_fgrounds_per_pop)
{
    cumdiscard_fgrounds_per_pop=_cumdiscard_fgrounds_per_pop;
}


void Vessel::set_cumeffort_fgrounds (const vector<double>  &_cumeffort_fgrounds)
{
    cumeffort_fgrounds=_cumeffort_fgrounds;
}


void Vessel::set_experiencedcpue_fgrounds (const vector<double>  &_experiencedcpue_fgrounds)
{
    experiencedcpue_fgrounds=_experiencedcpue_fgrounds;
}


void Vessel::set_experiencedcpue_fgrounds_per_pop (const vector<vector<double> >  &_experiencedcpue_fgrounds_per_pop)
{
    experiencedcpue_fgrounds_per_pop=_experiencedcpue_fgrounds_per_pop;
}


void Vessel::set_freq_experiencedcpue_fgrounds (const vector<double>  &_freq_experiencedcpue_fgrounds)
{
    freq_experiencedcpue_fgrounds=_freq_experiencedcpue_fgrounds;
}


void Vessel::set_freq_experiencedcpue_fgrounds_per_pop (const vector<vector<double> > &_freq_experiencedcpue_fgrounds_per_pop)
{
    freq_experiencedcpue_fgrounds_per_pop=_freq_experiencedcpue_fgrounds_per_pop;
}


void Vessel::set_roadmap (const list<types::NodeId> &_roadmap)
{
    roadmap=_roadmap;
}


void Vessel::set_inharbour (bool logic)
{
    inharbour = logic;
}


void Vessel::set_inactive (bool logic)
{
    inactive = logic;
}

void Vessel::set_vessel_exited (int logic)
{
    is_vessel_exited = logic;
}


void Vessel::set_natio (bool logic)
{
    natio = logic;
}


void Vessel::set_state(int _state)
{
    state=_state;
}


void Vessel::set_reason_to_go_back(int _reason_to_go_back)
{
    reason_to_go_back=_reason_to_go_back;
}


void Vessel::set_tstep_dep(int _tstep_dep)
{
    tstep_dep=_tstep_dep;
}


void Vessel::set_timeforrest(double _timeforrest)
{
    timeforrest=_timeforrest;
}


void Vessel::set_cumfuelcons(double _cumfuelcons)
{
    cumfuelcons=_cumfuelcons;
}

void Vessel::set_sweptareathistrip(double _sweptareathistrip)
{
    areasweptthistrip=_sweptareathistrip;
}

void Vessel::set_subsurfacesweptareathistrip(double _subsurfacesweptareathistrip)
{
    subsurfaceareasweptthistrip=_subsurfacesweptareathistrip;
}



void Vessel::set_cumsteaming(double _cumsteaming)
{
    cumsteaming=_cumsteaming;
}


void Vessel::set_consotogetthere(double _consotogetthere)
{
    consotogetthere=_consotogetthere;
}


void Vessel::set_distprevpos(double _distprevpos)
{
    distprevpos=_distprevpos;
}


void Vessel::set_timeatsea(double _timeatsea)
{
    timeatsea=_timeatsea;
}


void Vessel::set_traveled_dist_this_trip(double _traveled_dist_this_trip)
{
    traveled_dist_this_trip=_traveled_dist_this_trip;
}


void Vessel::set_cumcatches(double _cumcatches)
{
    cumcatches=_cumcatches;
}

void Vessel::set_cumdiscards(double _cumdiscards)
{
    cumdiscards=_cumdiscards;
}

void Vessel::clear_idx_used_metiers_this_trip()
{
    idx_used_metiers_this_trip.clear();
}


void Vessel::set_nbfpingspertrip(int _nbfpingpertrip)
{
    nbfpingspertrip=_nbfpingpertrip;
}


void Vessel::set_individual_tac_this_pop(ofstream& export_individual_tacs, int tstep, vector<Population* >& populations, vector<int> implicit_pops, int pop, int init, double a_tac)
{

    if(init)
    {
        // individual vessel share
        vector<double> percent_tac_per_pop = this->get_percent_tac_per_pop ();
        // global simulated tac
        double global_tac_this_pop = populations.at(pop)->get_tac()->get_current_tac();
        // contribution compared to "other" landings.
        double percent_simulated_tac_this_pop = populations.at(pop)->get_tac()->get_percent_for_simulated_vessels();
        // contribution compared to "other" landings.
        map<string,double> relative_key = populations.at(pop)->get_tac()->get_relative_stability_key();

        // convert global tac in kg
        individual_tac_per_pop.at(pop) = global_tac_this_pop*1000 *
                percent_simulated_tac_this_pop/100 *
                percent_tac_per_pop.at(pop)/100 *
                relative_key[this->get_nationality()]/100;
        individual_tac_per_pop_at_year_start.at(pop)=individual_tac_per_pop.at(pop);

        dout(cout << "this vessel: individual TAC set to " << individual_tac_per_pop.at(pop) <<
             " for this pop " << pop <<
             " from global tac " << global_tac_this_pop <<
             " percent_simulated_tac_this_pop " << percent_simulated_tac_this_pop <<
             " percent_tac_per_pop " << percent_tac_per_pop.at(pop) <<
             " relative key " << relative_key[this->get_nationality()] << endl);

        // inform if this vessel is actually linked with the management under investigation...
        // (if not then the vessel will not be forced to stay on quayside when TACs exhausted)
        if(individual_tac_per_pop.at(pop)!=0 &&
                !binary_search (implicit_pops.begin(), implicit_pops.end(),  pop  )  // check explicit pops...
                ) this->set_targeting_non_tac_pop_only (0);

        // export: caution, here export the individual quota
        int discard_all =0;
        export_individual_tacs << setprecision(0) << fixed;
        export_individual_tacs << tstep << " " <<
                                  this->get_name() << " " <<
                                  pop << " " <<
                                  individual_tac_per_pop.at(pop) << " " <<
                                  discard_all << endl;

    }
    else
    {
        if(a_tac!=0.0)
        {
            individual_tac_per_pop.at(pop) =a_tac;
            prop_remaining_individual_quotas.at(pop) =  a_tac/individual_tac_per_pop_at_year_start.at(pop);
            // the new tac is likely to be the initial tac decreased by the vessel catches so far.
        }
        else
        {

            // export: caution, here export discards
            int discard_all =1;
            export_individual_tacs << setprecision(0) << fixed;
            export_individual_tacs << tstep << " " <<
                                      this->get_name() << " " <<
                                      pop << " " <<
                                      a_tac << " " << // tac at 0
                                      discard_all << endl;

            // e.g. if discard for this vessel this pop then force a 0:
            individual_tac_per_pop.at(pop) = 0;
        }
    }
}


void Vessel::set_targeting_non_tac_pop_only(int _targeting_non_tac_pop_only)
{
    targeting_non_tac_pop_only=_targeting_non_tac_pop_only;
}

void Vessel::set_is_choked(int pop, int val)
{
    is_choked.at(pop)=val;
}

void Vessel::updateTripsStatistics(const std::vector<Population* >& populations, vector<int>& implicit_pops, int tstep,
                                   const DynAllocOptions &dyn_alloc_sce)
{

    outc(cout  << "...updateTripsStatistics()" << endl);

    double cumProfit = avgProfit * numTrips;
    double cumRevenues = avgRevenues * numTrips;

    if (numTrips > 0) {
        avgRevenues += (cumRevenues + lastTrip_revenues) / numTrips;
        avgProfit += (cumProfit + lastTrip_profit) / numTrips;
    } else {
        avgRevenues = avgProfit = 0.0;
    }




    lastTrip_revenues = 0.0;
    lastTrip_explicit_revenues=0.0;
    lastTrip_profit = 0.0;
    const vector< vector<double> > &a_catch_pop_at_szgroup = get_catch_pop_at_szgroup();
    for(unsigned int pop = 0; pop < a_catch_pop_at_szgroup.size(); pop++)
    {

        vector<int> comcat_at_szgroup =   populations[pop]->get_comcat_at_szgroup();

        outc(cout  << "...for pop " << pop << endl);


        // assuming a hardcoded price flexibility at 25% for all stocks
        // and price equation Pt=Po *  sum(Qt^e)/sum(Qo^e) with e price flexibility rate and Po initial average price
        double price_multiplier=1.0;
        if(tstep>8761 &&
                !binary_search (implicit_pops.begin(), implicit_pops.end(),  pop  ) ) // i.e. apply from second y only (also bc no end_of_years reached yet...)
        {
           vector <double> amount_fish_per_y;
           int a_unit=1;
           if(dyn_alloc_sce.option(Options::TACs))
           {
              amount_fish_per_y = populations[pop]->get_tac()->get_ts_tac();
              a_unit=1000; // because tac in tons
           }
           else
           {
              amount_fish_per_y= populations[pop]->get_landings_at_end_of_years();
              a_unit=1; // because landings in kilo
           }
           double amount_to= amount_fish_per_y.at(0)*a_unit;
           double numerator=0.0;
           double denominator=0.0;
           for(int i=0; i<amount_fish_per_y.size();++i)
           {
              if(amount_fish_per_y.at(i)<=1) amount_fish_per_y.at(i)=amount_to; // so that num/denom will be 1...to avoid nan or a large price_multiplier when amount caught is very very low
              numerator += pow(amount_fish_per_y.at(i)*a_unit, -0.25);
              denominator += pow(amount_to, -0.25);
           }
           price_multiplier=numerator/denominator;
           dout(cout << "for this pop " << pop << ", price_multiplier is "<< price_multiplier << endl);
           if(!isfinite(price_multiplier))
           {
               dout(cout << "ouch...for this pop " << pop << ", price_multiplier is corrected to 1 " << endl);
               price_multiplier=1.0; // extra precaution if any unexpected NaN.
           }
           dout(cout << "for this pop " << pop << ", price_multiplier is "<< price_multiplier << endl);
        }



        for(unsigned int sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
        {
            int comcat_this_size =comcat_at_szgroup.at(sz);
           // outc(cout  << "...comcat_this_size " << comcat_this_size << " for sz " << sz  << endl);
            lastTrip_revenues += a_catch_pop_at_szgroup[pop][sz] *
                                 get_loc()->get_prices_per_cat(pop, comcat_this_size) * price_multiplier *
                                  (100 / this->get_metier()->get_percent_revenue_completeness()); // scale up!
        }

        int namepop = populations[pop]->get_name();
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  namepop  ) )
        {
            for(unsigned int sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
            {
                int comcat_this_size =comcat_at_szgroup.at(sz);
                lastTrip_explicit_revenues += a_catch_pop_at_szgroup[pop][sz] * get_loc()->get_prices_per_cat(pop, comcat_this_size) *price_multiplier;
            }

        }


    }

    double fuelcost = get_cumfuelcons() * get_loc()->get_fuelprices(length_class);
    lastTrip_profit = lastTrip_revenues - fuelcost;

    ++numTrips;


    // AER economic indicators, updated along the trip level;
    TotLandingIncome += (lastTrip_revenues)* (100 - this->landing_costs_percent)/100;
 // cout << "TotLandingIncome is "<< TotLandingIncome << endl;
    TotHoursAtSea    += get_cumsteaming();            // cumul from the simu start
 // cout << "TotHoursAtSea is "<< TotHoursAtSea << endl;
    TotFuelCosts     += fuelcost;
 // cout << "TotFuelCosts is "<< TotFuelCosts << endl;
    TotVarCosts      += (this->other_variable_costs_per_unit_effort * get_cumsteaming());
 //   cout << "this->other_variable_costs_per_unit_effort is "<< this->other_variable_costs_per_unit_effort << " this vessel " << this->get_name() <<  endl;
 //   cout << "get_cumsteaming() "<< get_cumsteaming() << endl;
 //  cout << "TotVarCosts is "<< TotVarCosts << endl;
    GVA               = (TotLandingIncome+(annual_other_income*tstep/8761)) - TotFuelCosts - TotVarCosts - (other_annual_fixed_costs*tstep/8761);
                                               // other_variable_costs_per_unit_effort includes Repair costs, Ice costs, etc.
 //  cout << "GVA is "<< GVA << endl;

    if(annual_other_income==0) {
    annual_other_income=1;
    }



    if(TotLandingIncome>0) {
           GVAPerRevenue =   GVA/(TotLandingIncome+(annual_other_income*tstep/8761));
     }                                                 // AER indicator
    if (GVAPerRevenue<0) GVAPerRevenue=0; // do not allow negative values because should be between 0 and 1



//    cout << "GVAPerRevenue is "<< GVAPerRevenue << endl;
    GrossProfit       =  (GVA*(100-crewshare_and_unpaid_labour_costs_percent)/100) ;
                                               // AER indicator - gross cash flow
//  cout << "GrossProfit is "<< GrossProfit << endl;



    double LabourOpportunityCosts  = standard_labour_hour_opportunity_costs * this_vessel_nb_crew *  TotHoursAtSea;
// cout << "LabourOpportunityCosts is "<< LabourOpportunityCosts << endl;
    LabourSurplus                  = (GVA * crewshare_and_unpaid_labour_costs_percent/100) -
                                      (annual_insurance_costs_per_crew * this_vessel_nb_crew) -
                                       LabourOpportunityCosts;
// cout << "LabourSurplus is "<< LabourSurplus << endl;

    double CapitalOpportunityCosts = vessel_value * opportunity_interest_rate/100 *tstep/8761;
// cout << "CapitalOpportunityCosts is "<< CapitalOpportunityCosts << endl;

    if(vessel_value<0) vessel_value=0;
    NetProfit         =  GrossProfit - CapitalOpportunityCosts - (vessel_value* (100-annual_depreciation_rate)/100*tstep/8761);
// cout << "NetProfit is "<< NetProfit << endl;


   double divider=(TotLandingIncome+(annual_other_income*tstep/8761));
   if(divider>0) {
         NetProfitMargin=(NetProfit*100)/divider;
    } else {
       NetProfitMargin=0;
   }
    // AER indicator (in %)
// cout << "NetProfitMargin is "<< NetProfitMargin << endl;


    if(GVA>0 && standard_annual_full_time_employement_hours>0)
        GVAPerFTE         =  GVA/ (TotHoursAtSea/standard_annual_full_time_employement_hours);
                                               // a proxy of Labour Productivity
// cout << "GVAPerFTE is "<< GVAPerFTE << endl;

    if((vessel_value* (100-annual_depreciation_rate)/100*tstep/8761)>1){
    RoFTA             = (NetProfit+CapitalOpportunityCosts)/(vessel_value* (100-annual_depreciation_rate)/100*tstep/8761);
                                               // Capital Productivity i.e. Return on Fixed Tangible Assets
    } else{
    RoFTA             =0;
    }
// cout << "RoFTA is "<< RoFTA << endl;

   if(GVA>0) BER               = (other_annual_fixed_costs*tstep/8761)+CapitalOpportunityCosts+(1-(vessel_value* (100-annual_depreciation_rate)/100*tstep/8761)) /
                           (1-(  ((GVA*crewshare_and_unpaid_labour_costs_percent/100) + TotFuelCosts + TotVarCosts)   /TotLandingIncome) );
                                               // Break-Even Revenue
// cout << "BER is "<< BER << endl;

    if(BER>0){
    CRBER             = (TotLandingIncome + (annual_other_income*tstep/8761)) / BER;
                                               // Revenue to Break-even revenue Ratio
    } else{
    CRBER             =0;
    }

// cout << "CRBER is "<< CRBER << endl;

    if(GVA>0)   NetPresentValue = GVA* (1/pow((1+annual_discount_rate/100),(ceil(tstep / 8761))));
                                               // annual_discount_rate is in percent
// cout << "NetPresentValue is "<< NetPresentValue << endl;

    outc(cout  << "...updateTripsStatistics()...OK" << endl);

}

/** \brief Starting from the dtree root, traverse it evaluating any node and the relative Variable.
 * The return value from StateEvaluator::evaluate() is rounded and casted to int to define the next node
 * */
double Vessel::traverseDtree(int tstep, dtree::DecisionTree *tree)
{
    int bin = -1;
    double value = 0.0f;

    std::shared_ptr<dtree::Node> prevnode;
    std::shared_ptr<dtree::Node> node = tree->root();
    while (node.get()) {
        if (node->getChildrenCount() == 0) { // is a leaf node
                       // std::cout << "Node Value= " << node->value() << std::endl;
            return node->value();
        }

        value = 0.0;
        if (mStateEvaluators[static_cast<int>(node->variable())] != 0) {
            value = mStateEvaluators[static_cast<int>(node->variable())]->evaluate(tstep, this);
           // cout << "vessel " << this->get_name() << " evaluation gets back " << value << endl;
        } else {
            throw std::runtime_error("Unsupported variable evaulation requested.");
        }

        bin = static_cast<int>(std::floor(value*node->getChildrenCount() + 0.5));

       // std::cout << "value=" << value << " bin=" << bin << std::endl;
        if (bin < 0) bin = 0;
        if (bin > node->getChildrenCount()-1)
            bin = node->getChildrenCount()-1;
        prevnode = node;
        node = node->getChild(bin);
    }

    // if here, we may have a problem.

    if (prevnode != 0) {
        std::ostringstream ss;
        ss << "Invalid null node reached while traversing decision tree. value=" << value << " bin= " << bin;
        ss << " map: " << prevnode->getMapping(bin);
        throw std::runtime_error(ss.str());
    } else {
        throw std::runtime_error("Traversing an empty dtree.");
    }
}

string Vessel::nationalityFromName(const string &name)
{
    return name.substr(0, 3);
}


void Vessel::set_smartcatch(types::NodeId _smartcatch)
{
    smartcatch=_smartcatch;
}

void Vessel::set_highpotentialcatch(types::NodeId _highpotentialcatch)
{
    highpotentialcatch=_highpotentialcatch;
}

void Vessel::set_notthatfar(types::NodeId _notthatfar)
{
    notthatfar=_notthatfar;
}

void Vessel::set_lowesttariff(types::NodeId _lowesttariff)
{
    lowesttariff=_lowesttariff;
}


void Vessel::set_mosthistoricallyused(types::NodeId _mosthistoricallyused)
{
    mosthistoricallyused=_mosthistoricallyused;
}

//------------------------------------------------------------//
//------------------------------------------------------------//
// related to vessel movements...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::set_xy(double _x, double _y)
{
    x= _x;
    y= _y;
}


void Vessel::set_next_xy(double nx, double ny)
{
    next_x= nx;
    next_y= ny;
}


void  Vessel::erode_roadmap ()
{
    roadmap.pop_front();
}


void Vessel::move_to(double nx, double ny)
{
    m_location->set_xy(nx, ny);
}


//void Vessel::move_to(std::shared_ptr<Node> pnext_node)
void Vessel::move_to(Node* pnext_node)
{
    m_location= pnext_node;

    // update the node
    int idx= get_idx();
    vector<int> v = pnext_node->get_vid();
    sort (v.begin(), v.end());
    // add only if not already there...
    if (!binary_search (v.begin(), v.end(), idx)) pnext_node->set_vid(idx);

    // actually, where am I now?
    if(this->get_loc()->get_is_harbour())
    {
        this->set_inharbour (true);
    }
    else
    {
        this->set_inharbour (false);
    }
}


void Vessel::set_metier(Metier* pnew_metier)
{
    metier= pnew_metier;
}

void Vessel::find_next_point_on_the_graph(vector<Node *> &nodes)
{
    lock();
    find_next_point_on_the_graph_unlocked(nodes);
    unlock();
}


void Vessel::find_next_point_on_the_graph_unlocked(vector<Node* >& nodes)
{
    if(roadmap.size()==0)
    {
        cout << "DEBUG !! " << this->get_name() << endl;
    return;
    }

    assert(roadmap.size() > 0);

    list<types::NodeId>::iterator pos = roadmap.begin();
    list<types::NodeId>::iterator pos2 = roadmap.end();

    bool returning_to_harbour= nodes[(*(--pos2)).toIndex()]->get_is_harbour();
    dout(cout  <<  " returning_to_harbour " << returning_to_harbour  << endl);

    if(roadmap.size()==0)
    {
        cout << "DEBUG !! " << endl;
    }

    if(roadmap.size()==1)
    {

        //do a discrete jump on the final node
        dout (cout << "JUMP?? FROM NODE " << this->get_loc()->get_idx_node().toIndex()
              << " " << this->get_loc()->get_x() << " " << this->get_loc()->get_y()
              <<  " TO NODE " << nodes[(*pos).toIndex()]->get_idx_node().toIndex()
                << " " << nodes[(*pos).toIndex()]->get_x() << " " << nodes[(*pos).toIndex()]->get_y()  << endl);

        double dist_for_one_ping = this->get_speed() * PING_RATE * NAUTIC;
        double dist_next_node = dist(this->get_x(), this->get_y(), nodes[(*pos).toIndex()]->get_x(), nodes[(*pos).toIndex()]->get_y());
        if(dist_for_one_ping > dist_next_node)
        {
            dout(cout  << "YES: JUMP!!" << endl);
            this->move_to(nodes[(*pos).toIndex()]);
            next_x= this->get_loc()->get_x();
            next_y= this->get_loc()->get_y();
            x = next_x;
            y = next_y;
            roadmap.erase(pos);
            set_distprevpos(dist_next_node) ;
        }
        else
        {
            dout(cout  << "NO: APPROACH!!" << endl);
            double b = bearing (this->get_x(), this->get_y(), nodes[(*pos).toIndex()]->get_x(), nodes[(*pos).toIndex()]->get_y());
            vector<double> xy = destB(this->get_x(), this->get_y(), b, dist_for_one_ping);
            this->set_xy(xy[0], xy[1]);
            set_distprevpos(dist_for_one_ping) ;
            //if(dist!=dist) {} // c++ trick for like testing for is.nan

        }

        // update
        if(returning_to_harbour)
        {
            set_cumfuelcons( get_cumfuelcons() + (get_fuelcons()*PING_RATE* get_mult_fuelcons_when_returning()) ) ;
            set_consotogetthere( get_consotogetthere() + (get_fuelcons()*PING_RATE*get_mult_fuelcons_when_returning()) ) ;		}
        else
        {
            set_cumfuelcons( get_cumfuelcons() + (get_fuelcons()*PING_RATE*get_mult_fuelcons_when_steaming()) ) ;
            set_consotogetthere( get_consotogetthere() + (get_fuelcons()*PING_RATE*get_mult_fuelcons_when_steaming()) ) ;		}
        set_cumsteaming( get_cumsteaming() + PING_RATE ) ;
        set_state(2);
        dout ( cout << "in find_next_point_on_the_graph: distance prev pt: " << dist_next_node
               << ", conso to get there: " << get_consotogetthere() << endl);

    }
    else
    {
        dout(cout  << "SEMI-CONTINUOUS MOVE...from here " << this->get_loc()->get_idx_node().toIndex() << endl);
        dout(cout  << " to NODE " << nodes[(*pos).toIndex()]->get_idx_node().toIndex() << endl);
        bool flag = false;
        // travel on the graph as long as dist_traveled is reached.
        dout(cout << "SEMI-CONTINUOUS MOVE...FROM NODE " << this->get_loc()->get_idx_node().toIndex()
             << " TO NODE " << nodes[(*pos).toIndex()]->get_idx_node() << endl);
        next_x= nodes[(*pos).toIndex()]->get_x();
        next_y= nodes[(*pos).toIndex()]->get_y();
        dout(cout  << "x " << x << " y " << y  << " next_x " << next_x << " next_y " << next_y << endl);
        double dist_next_node = dist(x, y, next_x, next_y);
        double dist_traveled = this->get_speed() * PING_RATE * NAUTIC;
        this->set_distprevpos(dist_next_node) ;
        double b = bearing (x, y, next_x, next_y);
        this->set_course(b);
        dout(cout  << "bearing between " << this->get_loc()->get_idx_node().toIndex() << " and " <<nodes[(*pos).toIndex()]->get_idx_node().toIndex() <<" " << b << endl);
        dout(cout  << "START BEARING " << this->get_course() <<endl);

        dout(cout << "distance to next node (before moving): " << dist_next_node
             << ", dist to be traveled: " << dist_traveled  << endl);

        double dist_sauv;		 // required to get the remaining dist
        while(dist_traveled > dist_next_node)
        {
            next_x= nodes[(*pos).toIndex()]->get_x();
            next_y= nodes[(*pos).toIndex()]->get_y();
            double dist_next_node = dist(x, y, next_x, next_y);
            dist_sauv = dist_traveled;
            dist_traveled = dist_traveled - dist_next_node;
            dout(cout  << "dist_traveled  " << dist_traveled << endl);
            // system("pause");
            if(dist_traveled <=0.0)
            {
                // restitute
                dist_traveled = dist_sauv;
                break;
            }
            // change the current node
            // this->move_to(nodes[*pos]);
            // dout(cout  << "pass through node " << nodes[*pos]->get_idx_node().toIndex() << endl);
            //  x = next_x;
            // y = next_y;
            if(roadmap.size()>1)
            {
                this->move_to(nodes[(*pos).toIndex()]);
                dout(cout  << "pass through node " << nodes[(*pos).toIndex()]->get_idx_node().toIndex() << endl);
                x = next_x;
                y = next_y;
                // erode
                roadmap.pop_front();
                pos= roadmap.begin();
            }
            else
            {
                double dist_for_one_ping = this->get_speed() * PING_RATE * NAUTIC;
                double dist_next_node = dist(this->get_x(), this->get_y(), nodes[(*pos).toIndex()]->get_x(), nodes[(*pos).toIndex()]->get_y());
                if(dist_for_one_ping > dist_next_node)
                {

                    dout(cout  << "YES: JUMP TO THE FINAL NODE" << endl);
                    // JUMP
                    this->move_to(nodes[(*pos).toIndex()]);
                    next_x= this->get_loc()->get_x();
                    next_y= this->get_loc()->get_y();
                    x = next_x;	 // note: correct 24Feb14: was missing then was creating overlapped nodes, which was creating the straight line symptom due to dist at nan!!
                    y = next_y;	 // note: correct 24Feb14: was missing then was creating overlapped nodes, which was creating the straight line symptom due to dist at nan!!
                    // erode
                    roadmap.pop_front();
                    pos= roadmap.begin();
                    set_state(2);
                    flag = true;
                    break;
                }
                else
                {
                    dout(cout  << "NO: APPROACH THE FINAL NODE" << endl);
                    b = bearing (this->get_x(), this->get_y(), nodes[(*pos).toIndex()]->get_x(), nodes[(*pos).toIndex()]->get_y());
                    vector<double> xy = destB(this->get_x(), this->get_y(), b, dist_for_one_ping);
                    this->set_xy(xy[0], xy[1]);
                    set_distprevpos(dist_for_one_ping) ;
                    set_state(2);
                    flag = true;

                }

            }

        }						 // end while

        if(!flag)
        {
            pos= roadmap.begin();
            b = bearing (this->get_loc()->get_x(), this->get_loc()->get_y(), nodes[(*pos).toIndex()]->get_x(), nodes[(*pos).toIndex()]->get_y());
            dout(cout  << "bearing between " << this->get_loc()->get_idx_node().toIndex() << " and " <<nodes[(*pos).toIndex()]->get_idx_node().toIndex() <<" " << b << endl);
            //  } else{
            //       b = bearing (this->get_loc()->get_x(), this->get_loc()->get_y(), nodes[*pos]->get_x(), nodes[*pos]->get_y());
            // }
            this->set_course(b);
            dout(cout  << "END BEARING " <<  this->get_course() <<endl);
            dout(cout  << "destB " << " x " << x <<" y " << y << " course " << course  << " remaining dist_traveled "  << dist_traveled << endl);
            vector<double> xy = destB(x, y, course, dist_traveled);
            this->set_xy(xy[0], xy[1]);
            set_state(2);
            dout(cout  << "vessel in x " << this->get_x() << " y " << this->get_y()  << endl);

        }

        // update
        if(returning_to_harbour)
        {
            dout(cout  << "returning" << endl);
            set_cumfuelcons( get_cumfuelcons() + (get_fuelcons()*PING_RATE*get_mult_fuelcons_when_returning()) ) ;
            set_consotogetthere( get_consotogetthere() + (get_fuelcons()*PING_RATE*get_mult_fuelcons_when_returning()) ) ;		}
        else
        {
            dout(cout  << "steaming to" << endl);
            set_cumfuelcons( get_cumfuelcons() + (get_fuelcons()*PING_RATE*get_mult_fuelcons_when_steaming()) ) ;
            set_consotogetthere( get_consotogetthere() + (get_fuelcons()*PING_RATE*get_mult_fuelcons_when_steaming()) ) ;		}
        set_cumsteaming( get_cumsteaming() + PING_RATE ) ;
        set_timeatsea(get_timeatsea()+ PING_RATE);
        set_traveled_dist_this_trip (get_traveled_dist_this_trip() + this->get_speed() * PING_RATE * NAUTIC);
        set_state(2);
        //		this->set_roadmap(roadmap);
    }

    //if(this->get_x()>8 && this->get_x()<10 && this->get_y()<52 )
    //{
    //    dout(cout  << "x "<< this->get_x() << " y "<< this->get_y() << endl);
    //    dout(cout  << "DEBUG!! VESSEL "<< this->get_name() << " TRYING TO REACH " << "BY LAND!!" << endl); // detect to correct the straight line symptom
    // }
}


//------------------------------------------------------------//
//------------------------------------------------------------//
// the catch process...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::do_catch(ofstream& export_individual_tacs, vector<Population* >& populations, vector<Node* >& nodes, vector<Benthos* >& benthoshabs,
                      vector<int>& implicit_pops, vector<int>& grouped_tacs, int& tstep, double& graph_res,bool& is_tacs, bool& is_individual_vessel_quotas,
                      bool& check_all_stocks_before_going_fishing, bool& is_discard_ban, bool& is_grouped_tacs, double& tech_creeping_multiplier,
                      bool& is_fishing_credits,  bool& is_impact_benthos_N)
{
    lock();

    dout(cout  << "BEGIN do_catch()" << endl);

    // check the matrix of catches
    //dout(cout  << "in do_catch(): before: CATCH PER NBSZGROUP" << endl);
    //for(int i = 0; i < catch_pop_at_szgroup.size(); i++)
    //{
    //    for(int j = 0; j < catch_pop_at_szgroup[i].size(); j++)
    //    {
    //        dout(cout  << catch_pop_at_szgroup[i][j] << " ");
    //    }
    //    dout(cout  << endl);
    //}


    // VESSEL EFFECT
    vector<double> v_betas_per_pop  = this->get_vessel_betas_per_pop ();
    double v_kw                     = this->get_KW ();
    double v_vsize                  = this->get_length ();

    // METIER EFFECT
    //vector<double> dis_ogive        = this->get_metier()->get_discards_ogive();
    //vector<double> sel_ogive        = this->get_metier()->get_selectivity_ogive();
    vector< vector<double> > selectivity_per_stock = this->get_metier()->get_selectivity_per_stock_ogives();
    vector<double> m_betas_per_pop  = this->get_metier()->get_betas_per_pop();
    vector<double> discardratio_limits  = this->get_metier()->get_discardratio_limits();
    vector<int> m_mls_cat_per_pop  = this->get_metier()->get_mls_cat_per_pop();
    double fspeed                   = this->get_metier()->get_fspeed();
    double gear_width_a             = this->get_metier()->get_gear_width_a();
    double gear_width_b             = this->get_metier()->get_gear_width_b();
    string gear_width_model         = this->get_metier()->get_gear_width_model();

    // HABITAT EFFECT
    vector<double> h_betas_per_pop  = benthoshabs.at( this->get_loc()->get_benthos_id() )->get_h_betas_per_pop();



    // SWEPT AREA
    double gear_width=0.0;
    double swept_area=0.0;
    double surface_and_subsurface_swept_area=0.0;
    if(gear_width_model=="a*(LOA^b)")  gear_width=gear_width_a* pow(v_vsize,gear_width_b);
    if(gear_width_model=="a*(kW^b)")   gear_width=gear_width_a* pow(v_kw,gear_width_b);
    if(gear_width_model=="(a*LOA)+b")  gear_width=(gear_width_a*v_vsize) +gear_width_b;
    if(gear_width_model=="(a*kW)+b")   gear_width=(gear_width_a*v_kw) +gear_width_b;
    if(gear_width_model=="(a*b*LOA/20")   gear_width=gear_width_a*gear_width_b*v_vsize/20;
    if(gear_width_model=="if_LOA<20_a_else_b") {
        if(v_vsize<20){
            gear_width=gear_width_a;
        } else{
            gear_width=gear_width_b;
        }
    }
    // converted to the right unit i.e. km2 and assuming fishing at fspeed knots
    //  gear_width_a an gear_width_b assumed to be taken from Eigaard et al 2016 ICESJMS Table 4.
    // i.e. for relationships returning door spread or seine rope in metres.
    if(this->get_metier()->get_metier_type()==1){
        swept_area = gear_width/1000 * PING_RATE * fspeed*NAUTIC; // for trawlers
        surface_and_subsurface_swept_area = swept_area*0.329; // penetrating for trawlers (Eigaard et al 2016; TODO input from input files instead)
    } else{
        swept_area =  PI*pow(((gear_width/1000)/(2*PI)),2);  // seiners and gillnetters
        surface_and_subsurface_swept_area = swept_area*0.01; // no penetration for seiners and gillnetters (Eigaard et al 2016; TODO input from input files instead)
    } //=> at this stage, swept area is km^2, but will be exported in m^2 in the logbook file to avoid any artificial raising when using in a ratio e.g. revenue per swept area

    dout(cout << " for this model " << gear_width_model << " the gear width is " << gear_width
         << "from KW "<<  v_kw << " and vessel size "<< v_vsize << " and param a " << gear_width_a << " param b " <<gear_width_b
         << ", swept area this fishing event is then:" << swept_area
         << " compared to the cell area which is " << graph_res*graph_res << endl ;);
    this->get_loc()->add_to_cumsweptarea(swept_area);
    this->get_loc()->add_to_cumsubsurfacesweptarea(surface_and_subsurface_swept_area);
    this->set_sweptareathistrip(this->get_sweptareathistrip() + swept_area);
    this->set_subsurfacesweptareathistrip(this->get_subsurfacesweptareathistrip() + surface_and_subsurface_swept_area);

    // FIND OUT THE DECREASE FACTOR AFTER THE PASSAGE
    int a_landscape                  =           this->get_loc()->get_marine_landscape();
    //vector<double> a_benthos_biomass =           this->get_loc()->get_benthos_biomass_per_funcgr();
    multimap<int,double> loss        =           this->get_metier()->get_loss_after_1_passage();
    vector<double> loss_after_1_passage_per_func_group= find_entries_i_d (loss,  a_landscape);


    // THEN, DEPLETE THE UNDERLYING BENTHOS ON THIS NODE...
    double decrease_factor_on_benthos_funcgroup;
    double area_ratio1 = ((graph_res*graph_res)-swept_area)/(graph_res*graph_res);
    double area_ratio2 = swept_area/(graph_res*graph_res);

    if(area_ratio1<0)
    {
        area_ratio1=0;
        area_ratio2=1;
    }

    for (unsigned int funcid=0; funcid< this->get_loc()->get_benthos_tot_biomass().size(); funcid++)
    {
        // if(swept_area>0.0001)
        // {

        dout(cout  << "before " << this->get_loc()->get_benthos_tot_biomass(funcid));
        dout(cout  << "for the landscape is " << a_landscape);
        dout(cout  << "and the metier " <<  this->get_metier()->get_name());

        // decrease_factor_on_benthos_funcgroup= 1-(1-(loss_after_1_passage_per_func_group.at(funcid)*(swept_area/(graph_res*graph_res)) ) );
        //this->get_loc()->set_benthos_tot_biomass(funcid, this->get_loc()->get_benthos_tot_biomass(funcid)*(1-decrease_factor_on_benthos_funcgroup));

        // Inspired from Pitcher et al 2016
        if(is_impact_benthos_N)
        {
            decrease_factor_on_benthos_funcgroup  = 1-exp(loss_after_1_passage_per_func_group.at(funcid));
            double current_nb                    = this->get_loc()->get_benthos_tot_number(funcid);
            double next_nb                       = (area_ratio1*current_nb) + (area_ratio2*current_nb*(1-decrease_factor_on_benthos_funcgroup));
            this->get_loc()->set_benthos_tot_number(funcid, next_nb); // update
        }
        else
        { // impact on biomass instead...
            decrease_factor_on_benthos_funcgroup  = 1-exp(loss_after_1_passage_per_func_group.at(funcid));
            double current_bio                    = this->get_loc()->get_benthos_tot_biomass(funcid);
            double next_bio                       = (area_ratio1*current_bio) + (area_ratio2*current_bio*(1-decrease_factor_on_benthos_funcgroup));
            this->get_loc()->set_benthos_tot_biomass(funcid, next_bio); // update
        }
        outc (cout << "for this func " << funcid << " the loss_after_1_passage_per_func_group is "
              << loss_after_1_passage_per_func_group.at(funcid) << " for this landscape "<< a_landscape
              << ", then decrease_factor_on_benthos_funcgroup this fishing event is: "
              << decrease_factor_on_benthos_funcgroup << endl);

        dout(cout  << "after " << this->get_loc()->get_benthos_tot_biomass(funcid));
        //}

    }


    // NODE ATTRIBUTES
    vector <int> pop_names = this->get_loc()->get_pop_names_on_node();
    sort (pop_names.begin(), pop_names.end());
    auto idx_node = this->get_loc()->get_idx_node();
    int code_area = this->get_loc()->get_code_area();
    auto the_grds = this->get_fgrounds();
    // relative node index to this vessel
    int idx_node_r= find(the_grds.begin(), the_grds.end(), idx_node) - the_grds.begin();

    // VARIABLES VALID FOR THIS FISHING EVENT ONLY
    double totLandThisEvent=1;
    double totAvoiStksLandThisEvent=1;
    double totDiscThisEvent=0.0001;
    double totAvoiStksDiscThisEvent=0.0001;

    // TARIFFS ON THE NODE
    vector<double> cumulcatches = this->get_loc()->get_cumcatches_per_pop();
    vector<double> cumuldiscards = this->get_loc()->get_cumdiscards_per_pop();
    if(is_fishing_credits)
    {
        vector<double> tariff_this_cell = this->get_loc()->get_tariffs(); // tariff per hour because visit (no more) one site per hour
        vector<double> fishing_credits = this->get_fishing_credits();
        // check
        cout << "this node " << this->get_loc()->get_idx_node().toIndex() <<
                " has tariffs0 " << tariff_this_cell.at(0) << endl;
        cout << "this vessel " << this->get_name() <<
                " has credits " << fishing_credits.at(0) << endl;
        fishing_credits.at(0) = fishing_credits.at(0) - tariff_this_cell.at(0);
        this->set_fishing_credits(fishing_credits);
        cout << "this vessel " << this->get_loc()->get_idx_node().toIndex() <<
                " has remaining credits " << this->get_fishing_credits().at(0) << endl;
    }




    // for loop over pop
    vector <double> global_quotas(catch_pop_at_szgroup.size(), 0);
    vector <int> individual_quotas(catch_pop_at_szgroup.size(), 0);
    vector <double> grouped_quotas(*max_element(grouped_tacs.begin(),grouped_tacs.end())+1, 0);
    if(is_tacs)
    {
        // IQs
        if(tstep>1  && is_individual_vessel_quotas)
        {
            for (unsigned int pop=0; pop<catch_pop_at_szgroup.size(); pop++)
            {
               individual_quotas.at(pop) = this->get_individual_tac(pop); // default when IQ
               if(is_grouped_tacs) grouped_quotas.at(grouped_tacs.at(pop)) +=individual_quotas.at(pop); // sum up if grouped quotas
            }
            if(is_grouped_tacs)
            {
               for (unsigned int pop=0; pop<catch_pop_at_szgroup.size(); pop++)
                {
                   individual_quotas.at(pop) = grouped_quotas.at(grouped_tacs.at(pop)); // feed back
                }

            }
        }
         // global TACs
        //if(tstep>8761  && !is_individual_vessel_quotas)
        if(!is_individual_vessel_quotas)
        {
            // remember that grouped_tacs is in form of e.g. 1 1 1 2 2 3 3 4 4 5 5 5 5 5 etc. as many as stocks.
            for (unsigned int pop=0; pop<catch_pop_at_szgroup.size(); pop++)
            {
             global_quotas.at(pop) = populations.at(pop)->get_tac()->get_current_tac(); // default when global TAC
             if(is_grouped_tacs) grouped_quotas.at(grouped_tacs.at(pop)) +=global_quotas.at(pop); // sum up if grouped quotas
            }
            if(is_grouped_tacs)
            {
               for (unsigned int pop=0; pop<catch_pop_at_szgroup.size(); pop++)
                {
                   global_quotas.at(pop) = grouped_quotas.at(grouped_tacs.at(pop)); // feed back
                }

            }
        }

     } // end is_tacs



    // OUTPUTS
    // declare with length nbpops
    vector<double> tot_catch_per_pop(  catch_pop_at_szgroup.size() );

    //this->clear_catch_pop_at_szgroup();

    // for loop over pop
    for (unsigned int pop=0; pop<catch_pop_at_szgroup.size(); pop++)
    {
        int namepop = populations[pop]->get_name();
        // is this pop not implicit? AND is this node in the range of the pop? remember code 10 is for out of range (see R code)
        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  namepop  ) && code_area!=10)
        {
            // is this pop present on this node?
            if (binary_search (pop_names.begin(), pop_names.end(),  namepop  ))
            {
                dout(cout  << "this pop " << populations.at(pop)->get_name() << " is present on this node... " << endl);


                /*
                // check
                if(this->get_loc()->get_idx_node()==186 && namepop==3)
                {
                    vector <double> N_at_szgroup= this->get_loc()->get_Ns_pops_at_szgroup(3);
                    vector <double> removals_at_szgroup= this->get_loc()->get_removals_pops_at_szgroup(3);
                    vector <double> at_month_start= this->get_loc()->get_Ns_pops_at_szgroup_at_month_start(3);

                    for(int sz=0; sz<N_at_szgroup.size(); sz++) cout << "N_at_szgroup at the start of do_catch sz " <<  N_at_szgroup.at(sz) << endl;
                    for(int sz=0; sz<N_at_szgroup.size(); sz++) cout << "removals_at_szgroup at the start of do_catch sz " <<  removals_at_szgroup.at(sz) << endl;
                    for(int sz=0; sz<N_at_szgroup.size(); sz++) cout << "at_month_start at the start of do_catch sz " <<  at_month_start.at(sz) << endl;

                }
                */

                // 1. COMPUTE TOTAL AVAILABLE BIOMASS
                // ON THIS NODE FOR THIS POP
                vector<double>  Ns_at_szgroup_pop = this->get_loc()->get_Ns_pops_at_szgroup(namepop );
                // just a copy to check for consistency at the end...
                vector<double>  init_Ns_at_szgroup_pop = Ns_at_szgroup_pop;

                vector <int>selected_szgroups        = populations[pop]->get_selected_szgroups();
                vector <double>wsz                   = populations[pop]->get_weight_at_szgroup();


                // // // // // //
                // HARDCODING SPECIFIC TO MYFISH MIG (TO BE REMOVED!!!)
                /*if(pop==10 && this->get_loc()->get_x()<12)
                {
                    vector <double> initial_wsz;
                    initial_wsz.push_back(0.0001266451);
                    initial_wsz.push_back(0.004076539);
                    initial_wsz.push_back(0.02048018);
                    initial_wsz.push_back(0.05930595);
                    initial_wsz.push_back(0.1312184);
                    initial_wsz.push_back(0.2473941);
                    initial_wsz.push_back(0.4194203);
                    initial_wsz.push_back(0.6592298);
                    initial_wsz.push_back(0.9790549);
                    initial_wsz.push_back(1.391393);
                    initial_wsz.push_back(1.90898);
                    initial_wsz.push_back(2.544769);
                    initial_wsz.push_back(3.311913);
                    initial_wsz.push_back(4.223748);
                    wsz=initial_wsz; // replace if sd22
                }
                */// // // // // //




                // init
                vector <double>all_biomass         = Ns_at_szgroup_pop;
                // init
                vector <double>avail_biomass         = Ns_at_szgroup_pop;
                // init
                vector <double>alloc_key             = Ns_at_szgroup_pop;
                // init
                vector <double>catch_per_szgroup     = Ns_at_szgroup_pop;
                // init
                vector <double>landings_per_szgroup     = Ns_at_szgroup_pop;
                // init
                vector <double>discards_per_szgroup     = Ns_at_szgroup_pop;
                // init
                vector <double>removals_per_szgroup  = Ns_at_szgroup_pop;
                // init
                vector <double>pressure_per_szgroup_pop  = Ns_at_szgroup_pop;
                // init
                vector <double>new_Ns_at_szgroup_pop = Ns_at_szgroup_pop;
                //init
                double tot = 0;
                double tot2                     = 0;
                double tot_avai_for_land            = 0;
                double tot_avai_for_disc            = 0;
                int    MLS_cat                      = m_mls_cat_per_pop[pop];



                // compute available biomass via selectivity
                for(int szgroup=0; szgroup < (int)avail_biomass.size(); szgroup++)
                {
                    catch_per_szgroup[szgroup]=0; // init
                    ping_catch_pop_at_szgroup[pop][szgroup]=0; // (re)init

                    all_biomass[szgroup]   =  Ns_at_szgroup_pop[szgroup]*wsz[szgroup];
                    avail_biomass[szgroup] =  all_biomass[szgroup]      *selectivity_per_stock[pop][szgroup]; // available for landings only

                  //if(this->get_name()=="GRK_KERK57_1_51" && pop==4){
                  //    cout  << "-------------------------" << endl;
                  //    cout  << "explicit: Ns_at_szgroup_pop[szgroup] for szgroup " << szgroup << " is " << Ns_at_szgroup_pop[szgroup] << endl;
                  //    cout  << "explicit: wsz[szgroup] for szgroup " << szgroup << " is " << wsz[szgroup] << endl;
                  //    cout  << "explicit: selectivity_per_stock[pop][szgroup] for szgroup " << szgroup << " is " << selectivity_per_stock[pop][szgroup] << endl;
                  //  }

                    // cumul
                    tot = tot+avail_biomass[szgroup];
                    tot2 = tot2+all_biomass[szgroup];
                    if(szgroup >=MLS_cat) {
                        tot_avai_for_land= tot_avai_for_land+avail_biomass[szgroup];
                    } else{
                        tot_avai_for_disc= tot_avai_for_disc+all_biomass[szgroup];
                    }
                    dout(cout  << "wsz[szgroup] " <<wsz[szgroup] << endl);
                    dout(cout  << "selectivity_per_stock[pop] " <<selectivity_per_stock[pop][szgroup] << endl);
                    dout(cout  << "avail_biomass[szgroup] " <<avail_biomass[szgroup] << endl);
                }

                //if(this->get_name()=="GRK_KERK57_1_51" && pop==4){
                //    cout  << "tot biomass available on this node " << tot << endl;
                //}

                dout(cout  << "tot biomass available on this node " << tot << endl);

                // by the way, if no biomass at all then update the spatial pop distribution!
                if(tot2 <=1.0) this->get_loc()->remove_pop_names_on_node(namepop);

                //if(tot!=tot)  // c++ trick for like testing for is.nan
                //{
                //    int a;
                //    cout << "1: nan detected" << endl;
                //    cout << "1: nan detected...Pause: type a number to continue";
                //    cin >> a;
                //}

                if(std::fabs(tot) > 1e-3)
                {
                    // remember that metier at NA in parameterisation (see the Rcode) have been set to -20
                    // in order to avoid (potentially high!) catches with the wrong metier

                    // 2. COMPUTE TOTAL CATCH IN WEIGHT as the response from the glm model on cpue
                    // (kg per hour i.e. PING:RATE at 1, for a calibration year, poisson regression, see the R code)
                    // rescaling multiplying by 1000, see R code.
                    vector<double> avai_pops_at_selected_szgroup = nodes.at(idx_node.toIndex())->get_avai_pops_at_selected_szgroup(pop);

                    // compute the avai*sel term
                    double avai_betas=0.0;
                    double avai_beta_param=0;
                    for (int selszi=0; selszi<avai_pops_at_selected_szgroup.size();++selszi)
                        {
                        if(selszi==0) avai_beta_param =populations[pop]->get_avai0_beta();
                        if(selszi==1) avai_beta_param =populations[pop]->get_avai2_beta();
                        if(selszi==2) avai_beta_param =populations[pop]->get_avai3_beta();
                        if(selszi==3) avai_beta_param =populations[pop]->get_avai5_beta();
                        if(selszi==4) avai_beta_param =populations[pop]->get_avai7_beta(); // TO DO: simplify this crap...
                        avai_betas +=  avai_beta_param * avai_pops_at_selected_szgroup.at(selszi) *1000 *selectivity_per_stock[pop][selected_szgroups.at(selszi)];
                        }


                    // vessel effect
                    tot_catch_per_pop[pop] = min(tot, exp( // vessel effect
                                                     v_betas_per_pop[pop]*1 +
                                                   // metier effect
                                                      m_betas_per_pop[pop]*1 +
                                                   // habitat (benthos) effect
                                                      h_betas_per_pop[pop]*1 +
                                                   // fish avai*sel effect
                                                          avai_betas  // poisson regression, see the R code
                                                )*populations[pop]->get_cpue_multiplier()* tech_creeping_multiplier );
                                        // 'min' is there for not allowing catching more than available!

                    dout(cout  << "-------------------------" << endl);
                    dout(cout  << "cpue_multiplier is " << populations[pop]->get_cpue_multiplier() << endl);
                    dout(cout  << "tot_catch_per_pop[pop] for pop " << pop << " is " << tot_catch_per_pop[pop] << endl);

                    //if(this->get_name()=="GRK_KERK57_1_51" && pop==4){
                    //   cout  << "explicit: tot_catch_per_pop[pop] for pop " << pop << " is " << tot_catch_per_pop[pop] << endl;
                    //   cout  << "explicit: given  that avai biomass is " << tot << endl;
                    //}

                    // REMENBER THAT THE IDEAL WOULD BE DO THE THE GLM ON ABSOLUTE NUMBER OF INDIVIDUAL ON EACH NODE......
                    // BUT THIS INFO IS NOT AVAILABLE OF COURSE (WE ONLY HAVE THE N FOR THE FIRST OF JANUARY)
                    // THATS WHY WE USE THE RELATIVE AVAILABILITY AS A PROXY.


                    //for(int i = 0; i < cumcatch_fgrounds.size(); i++){
                    //cout << "on the grounds of this vessel " << the_grds.at(i) << " cumcatch is " << cumcatch_fgrounds.at(i) << endl;
                    //}
// if(this->get_name()=="DNK000011569" &&  pop==2) cout << "tot_catch_per_pop[pop] is " << tot_catch_per_pop[pop] << endl;


                    // compute the landings vs. discard part
                    // in function of MLS and gear selectivity ogive and the N
                    vector<double> Ns_at_szgroup_pop_scaled = Ns_at_szgroup_pop; // init
                    for(int sizgroup=0; sizgroup<(int)Ns_at_szgroup_pop.size(); sizgroup++) {
                        Ns_at_szgroup_pop_scaled.at(sizgroup)=Ns_at_szgroup_pop_scaled.at(sizgroup)/
                                *(max_element(Ns_at_szgroup_pop.begin(), Ns_at_szgroup_pop.end()));
// if(this->get_name()=="DNK000011569" &&  pop==2) cout << "this szgroup " <<  sizgroup << " Ns_at_szgroup_pop_scaled.at(sizgroup) is " << Ns_at_szgroup_pop_scaled.at(sizgroup) << endl;
                    }
                    int inter=0;
                    int a_szgroup=0;
                    while(a_szgroup<=MLS_cat){
                        inter = a_szgroup;
                        a_szgroup+=1;
                    }
                    dout(if(inter>Ns_at_szgroup_pop_scaled.size()) cout<< "MLS categories cannot be > 13" << endl;)
                    double left_to_MLS=0;
                    double right_to_MLS=0;
// if(this->get_name()=="DNK000011569" &&  pop==2) cout << " inter is " <<  inter  << " and MLS_cat is "  << MLS_cat << endl;
                    if(selectivity_per_stock[pop].at(inter)>Ns_at_szgroup_pop_scaled.at(inter)){
                        left_to_MLS  = trapezoidal(0, inter, selectivity_per_stock[pop]) + trapezoidal(inter, MLS_cat, Ns_at_szgroup_pop_scaled); // discards
                        right_to_MLS = trapezoidal(MLS_cat, NBSZGROUP-1, Ns_at_szgroup_pop_scaled); // landings
// if(this->get_name()=="DNK000011569" &&  pop==2) cout << " here" << endl;

                    } else{
                        left_to_MLS  = trapezoidal(0, MLS_cat, selectivity_per_stock[pop]); // discards
                        right_to_MLS = trapezoidal(MLS_cat, inter, selectivity_per_stock[pop])+trapezoidal(inter, NBSZGROUP-1, Ns_at_szgroup_pop_scaled); // landings
// if(this->get_name()=="DNK000011569" &&  pop==2) cout << " there" << endl;

                    }

                    double tot_landings_this_pop=tot_catch_per_pop[pop];
                    double discardfactor = left_to_MLS/right_to_MLS; // (dis/lan)
                    //  discardfactor = dis/lan != discard rate...btw, converting a discard rate into discardratio is disc/land=x/(1-x) with x=disc/(disc+land)
                    discardfactor = min( discardratio_limits[pop] , discardfactor); // metier and pop specific limit
                    // => caution: discard factor bounded to not exceed a value, otherwise high unrealistic disrcards will be produced when no adult left on zones
                    double tot_discards_this_pop=tot_catch_per_pop[pop]*discardfactor ;
                    // then disagregate per szgroup....

// if(this->get_name()=="DNK000011569" &&  pop==2) cout << "discards from tot_catch_per_pop[pop]* left_to_MLS/right_to_MLS is " << tot_discards_this_pop << endl;
// if(this->get_name()=="DNK000011569" &&  pop==2) cout << "because left_to_MLS is " << left_to_MLS << " and right_to_MLS is " << right_to_MLS << endl;
// if(this->get_name()=="DNK000011569" &&  pop==2) cout << "....and discardfactor is " << discardfactor << endl;

                    // 3. DISAGREGATE TOTAL LANDINGS IN WEIGHT INTO SZGROUP
                    //  AND CONVERT INTO REMOVALS IN NUMBER
                    vector <double> totN = populations[pop]->get_tot_N_at_szgroup_just_after_redistribution();
                    // init
                    vector<double> new_avai_pops_at_selected_szgroup=avai_pops_at_selected_szgroup;


                    int a_count=0;
                    for(int szgroup=0; szgroup <(int)avail_biomass.size(); szgroup++)
                    {
                        if(avail_biomass[szgroup]!=0)
                        {
                            // compute alloc key
                            // proportion
                            if(szgroup>=MLS_cat){
                                alloc_key[szgroup]=avail_biomass[szgroup] /(tot_avai_for_land);
                            } else{
                                alloc_key[szgroup]=0;
                            }

                            dout(cout  << "alloc_key[szgroup] " <<alloc_key[szgroup] << endl);
                            // disaggregate total catch (in weight) for this pop according to the alloc key
                            landings_per_szgroup[szgroup]= tot_landings_this_pop*alloc_key[szgroup];

                            outc(cout << " sz " << szgroup <<
                                 " pop " << pop <<
                                 " tot landings this pop " << tot_landings_this_pop <<
                                 " landings per sz "      <<  landings_per_szgroup[szgroup] << endl
                                 );

                            //catch_per_szgroup[szgroup]=catch_per_szgroup[szgroup] /(1-dis_ogive[szgroup]);
                            // replaced by: (9 March 2015)

                            // 3bis. DISAGREGATE TOTAL DISCARDS IN WEIGHT INTO SZGROUP
                            //  AND CONVERT INTO REMOVALS IN NUMBER

                            // compute alloc key
                            // proportion
                            if(szgroup<MLS_cat){
                                alloc_key[szgroup]=all_biomass[szgroup] /(tot_avai_for_disc);
                            } else{
                                alloc_key[szgroup]=0;
                            }

                            dout(cout  << "alloc_key[szgroup] " <<alloc_key[szgroup] << endl);
                            // disaggregate total catch (in weight) for this pop according to the alloc key
                            discards_per_szgroup[szgroup]= tot_discards_this_pop*alloc_key[szgroup];

                            outc(
                                        cout << " sz " << szgroup <<
                                        " pop " << pop <<
                                        " tot discards this pop " << tot_discards_this_pop <<
                                        " discards per sz "      << discards_per_szgroup[szgroup] << endl
                                        );


                            catch_per_szgroup[szgroup]=landings_per_szgroup[szgroup]+discards_per_szgroup[szgroup];
                            dout(cout  << " sz " << szgroup <<
                                 " catch_per_szgroup[szgroup] " << catch_per_szgroup[szgroup] << endl);

                            // then get the removals in terms of N
                            removals_per_szgroup[szgroup]= catch_per_szgroup[szgroup]/wsz[szgroup];
                            if(removals_per_szgroup[szgroup]!=removals_per_szgroup[szgroup])
                            {
                                cout <<"nan detected! in removals_per_szgroup[szgroup] ... "<< endl;
                            }

                            dout(cout  << " weight_per_szgroup[szgroup] " << wsz[szgroup] << endl);
                            //if(idx_node==2436 && namepop==9) dout(cout << " in do_catch, removals_per_szgroup[szgroup] " << removals_per_szgroup[szgroup] << endl);

                            // finally, impact the N...
                            // ...but do not allow negative abundance!
                            // then correct the catches accordingly.
                            if(removals_per_szgroup[szgroup]>Ns_at_szgroup_pop[szgroup])
                            {
                                catch_per_szgroup[szgroup]=
                                        (Ns_at_szgroup_pop[szgroup])*wsz[szgroup];
                                if(szgroup>=MLS_cat){
                                    landings_per_szgroup[szgroup]=
                                            (Ns_at_szgroup_pop[szgroup])*wsz[szgroup];

                                }


                                dout (cout << "the szgroup " << szgroup <<
                                      "for this pop " << pop << " is fully depleted on this node " <<
                                      idx_node << "! catch is "<<
                                      catch_per_szgroup[szgroup] << endl);
                                // everything is caught...
                                removals_per_szgroup[szgroup]=Ns_at_szgroup_pop[szgroup];
                                // nothing left!
                                new_Ns_at_szgroup_pop[szgroup]=0;

                            }
                            else
                            {


                                new_Ns_at_szgroup_pop[szgroup]=Ns_at_szgroup_pop[szgroup]-removals_per_szgroup[szgroup];

                                /*
                                // check (before)
                                vector <double> a_avai_bef = nodes.at(idx_node)->get_avai_pops_at_selected_szgroup(pop);
                                dout(cout << "a_avai bef: " << endl);
                                for(int it=0; it<a_avai_bef.size(); it++)
                                {
                                    dout(cout  << a_avai_bef.at(it) << " " << endl);
                                }

                                // check (before)
                                dout(cout  << "removals_per_szgroup bef: " << endl);
                                dout(cout  << removals_per_szgroup.at(szgroup) << " " << endl);

                                // check (before)
                                dout(cout  << "Ns_at_szgroup_pop bef: " << endl);
                                dout(cout  << Ns_at_szgroup_pop.at(szgroup) << " " << endl);

                                // check (before)
                                dout(cout  << "new_Ns_at_szgroup_pop bef: " << endl);
                                dout(cout  << new_Ns_at_szgroup_pop.at(szgroup) << " " << endl);
                                */

                                // update the availability key accounting for the current extraction (hourly feedback loop on the pop dynamics)
                                // (note that another (implicit) feedback is when available biomass =0 on the node)
                                // (the annual feedback is via get_cpue_multiplier())
                                // let the avai drift from the initial value...caution: avai do not sum to 1 any more after the first extraction event
                                // (note that Ns_at_szgroup_pop[szgroup]/totN[szgroup] = avai just after a distribute_N event.)
                                // init
                                double val=0;
                               // if(szgroup==selected_szgroups.at(a_count) && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
                               //    {
                               //     val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
                               //     new_avai_pops_at_selected_szgroup.at(a_count)=val;
                               //     if(a_count<(selected_szgroups.size()-1)) a_count+=1;
                               //     }
                               // nodes.at(idx_node.toIndex())->set_avai_pops_at_selected_szgroup(pop, new_avai_pops_at_selected_szgroup);

                                /*

                                // check (after)
                                dout(cout << "a val " << val << " for this szgroup  " << szgroup << endl);

                                // check (after)
                                vector <double> a_avai_aft = nodes.at(idx_node)->get_avai_pops_at_selected_szgroup(pop);
                                dout(cout << "a_avai aft: " << endl);
                                for(int it=0; it<a_avai_aft.size(); it++)
                                {
                                    dout(cout << a_avai_aft.at(it) << " " << endl);
                                }
                                */

                            }
                            // add the preexisting removals (from other vessels) on this node for this szgroup
                            vector <double>cumul_removals_at_szgroup_pop=this->get_loc()->get_removals_pops_at_szgroup(namepop);
                            removals_per_szgroup[szgroup]+=cumul_removals_at_szgroup_pop[szgroup];

                            //if(idx_node==186 && namepop==3) dout(cout << " cumulated removals_per_szgroup[szgroup] " << removals_per_szgroup[szgroup] << endl);

                            // caution: cumul landings at the trip level
//if(this->get_name()=="DNK000011569" &&  pop==2) cout << "....discards_pop_at_szgroup[pop][szgroup] szgroup " << szgroup <<" is " << discards_pop_at_szgroup[pop][szgroup] << endl;
//if(this->get_name()=="DNK000011569" &&  pop==2) cout << "....landings_per_szgroup[pop][szgroup] szgroup " << szgroup <<" is " << landings_per_szgroup[szgroup] << endl;
                            catch_pop_at_szgroup[pop][szgroup] += landings_per_szgroup[szgroup]; // (landings only) in weight
                            discards_pop_at_szgroup[pop][szgroup] += discards_per_szgroup[szgroup];// in weight
                            ping_catch_pop_at_szgroup[pop][szgroup]=landings_per_szgroup[szgroup]+discards_per_szgroup[szgroup]; // ping catch in weight
                        }
                        else
                        {
                            dout(cout  << "no biomass for this szgroup to fish there! " << endl);
                            new_Ns_at_szgroup_pop[szgroup]=0;
                            catch_per_szgroup[szgroup]=0;
                            landings_per_szgroup[szgroup]=0;
                            discards_per_szgroup[szgroup]=0;
                            removals_per_szgroup[szgroup]=0;
                            // add the preexisting removals (from other vessels) on this node for this szgroup
                            vector <double>cumul_removals_at_szgroup_pop=this->get_loc()->get_removals_pops_at_szgroup(namepop);
                            removals_per_szgroup[szgroup]+=cumul_removals_at_szgroup_pop[szgroup];

                            // loglike exported variables
                            catch_pop_at_szgroup[pop][szgroup] += 0; // in weight
                            discards_pop_at_szgroup[pop][szgroup] += 0;// in weight
                            ping_catch_pop_at_szgroup[pop][szgroup] = 0;


                        }

                    } // end szgroup



                    // new Ns on this node= old Ns - removals
                    this->get_loc()->set_Ns_pops_at_szgroup(  namepop, new_Ns_at_szgroup_pop);

                    // a new removals cumul on this node
                    // i.e. the preexisting removals from oth vessels + from this vessel
                    this->get_loc()->set_removals_pops_at_szgroup(  namepop, removals_per_szgroup);

                    /*
                    if(this->get_loc()->get_idx_node()==186 && namepop==3)
                    {
                        vector <double> N_at_szgroup= this->get_loc()->get_Ns_pops_at_szgroup(3);
                        vector <double> removals_at_szgroup= this->get_loc()->get_removals_pops_at_szgroup(3);
                        vector <double> at_month_start= this->get_loc()->get_Ns_pops_at_szgroup_at_month_start(3);

                        for(int sz=0; sz<N_at_szgroup.size(); sz++) dout(cout << "N_at_szgroup at the end of do_catch sz " <<  N_at_szgroup.at(sz) << endl);
                        for(int sz=0; sz<N_at_szgroup.size(); sz++) dout(cout << "removals_at_szgroup at the end of do_catch sz " <<  removals_at_szgroup.at(sz) << endl);
                        for(int sz=0; sz<N_at_szgroup.size(); sz++) dout(cout << "at_month_start at the end of do_catch sz " <<  at_month_start.at(sz) << endl);

                        for(int sz=0; sz<N_at_szgroup.size(); sz++)
                        {
                            if(removals_at_szgroup.at(sz)>at_month_start.at(sz))
                            {
                                dout(cout << "something wrong here!..." << endl);
                                int xx;
                                cin >>xx;
                            }
                        }

                    }
                    */

                    // then, the vessel catches for this pop so far is...
                    // (indeed, this might not correspond to 'tot_catch_per_pop' when no biomass
                    //   or when removals > than that is actually available)
                    double a_cumul_weight_this_pop_this_vessel=0;
                    if(is_discard_ban)
                    {
                        for(unsigned int a_szgroup = 0; a_szgroup < catch_per_szgroup.size(); a_szgroup++)
                        {
                            a_cumul_weight_this_pop_this_vessel +=catch_per_szgroup[a_szgroup];
                        }
                    }
                    else
                    {
                        for(unsigned int a_szgroup = 0; a_szgroup < landings_per_szgroup.size(); a_szgroup++)
                        {
                            a_cumul_weight_this_pop_this_vessel +=landings_per_szgroup[a_szgroup];
                        }

                    }

                    // the management is also applied the first year, (caution, the first year is also the calibration year)
                    if(is_tacs)
                    {
                        if(tstep>1 && is_individual_vessel_quotas)
                        {
                            // TAC MANAGEMENT MEASURE:
                            // THE TAC: check against first the individual quota, then the global tac...
                            // 1. get the individual vessel tac for this pop
                            int remaining_individual_tac_this_pop = individual_quotas.at(pop);

                            // 1bis. caution
                            if(is_grouped_tacs)
                            {
                                for (unsigned int apop=0; apop<catch_pop_at_szgroup.size(); apop++)
                                {
                                    if(apop!=pop && (grouped_tacs.at(apop)==grouped_tacs.at(pop)))
                                     {
                                        for(unsigned int szgroup=0; szgroup < catch_pop_at_szgroup[apop].size();++szgroup)
                                        {
                                        a_cumul_weight_this_pop_this_vessel += catch_pop_at_szgroup[apop][szgroup]; // add catches on other grouped pops
                                        }
                                      }
                                 }
                            }


                            // 2. compare (AT THE VESSEL SCALE)
                            if(a_cumul_weight_this_pop_this_vessel>remaining_individual_tac_this_pop)
                            {
                                dout(cout  << this->get_name()  << ": individual quota is "<< remaining_individual_tac_this_pop << " for this pop: overshooted" << endl);

                                // reaction = discard
                                dout (cout << "...then this vessel (with quota " << remaining_individual_tac_this_pop << ") now discards all (" <<
                                      a_cumul_weight_this_pop_this_vessel <<") for this pop " << pop << "!!! " << endl);

                                // force a zero quota on this pop. (and export discards)
                                a_cumul_weight_this_pop_this_vessel=0.0; // discard all!
                                this->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, implicit_pops, pop, 0, 0.0);
                                // => if all quotas at 0 then the vessel will stay on quayside in should_i_go_fishing()...
                                // what a waste !!...

                                for(unsigned int szgroup=0; szgroup < catch_pop_at_szgroup[pop].size();++szgroup)
                                {
                                    discards_pop_at_szgroup[pop][szgroup]+=catch_pop_at_szgroup[pop][szgroup];// discard all!
                                    catch_pop_at_szgroup[pop][szgroup]=0; // discard all!
                                    ping_catch_pop_at_szgroup[pop][szgroup]=discards_pop_at_szgroup[pop][szgroup]; // catches=discards

                                }

                            }
                            else
                            {
                                dout(cout  << "individual quota this pop still ok...but now decrease the amount by the last catches." << endl);
                                this->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, implicit_pops, pop, 0,
                                                                  remaining_individual_tac_this_pop- a_cumul_weight_this_pop_this_vessel);

                            }
                        }
                        // 3. get the global tac for this pop
                        dout(cout  << "BEFORE " << populations.at(pop)->get_landings_so_far());

                        double so_far=0.0;
                        so_far = (populations.at(pop)->get_landings_so_far()) +
                                a_cumul_weight_this_pop_this_vessel;
                        // "real-time" update, accounting for this last bit of catches
                        populations.at(pop)->set_landings_so_far(so_far);
                        // note that oth_land (per node) are also added to landings_so_far but at the start of each month.

                        dout(cout  << "the new catch is  " << tot_catch_per_pop[pop] << endl);
                        dout(cout  << "landings so far for pop " << pop << ", AFTER: " << populations.at(pop)->get_landings_so_far());

                        if(is_grouped_tacs)
                        {
                            for (unsigned int apop=0; apop<catch_pop_at_szgroup.size(); apop++)
                            {
                                if(apop!=pop && (grouped_tacs.at(apop)==grouped_tacs.at(pop))) so_far += (populations.at(apop)->get_landings_so_far()); // add catches on other grouped pops
                            }
                        }

                        if(!is_individual_vessel_quotas)
                        // because the first year is the calibration year:
                        //if(tstep>8761  && !is_individual_vessel_quotas)
                        {
                            // 4. compare in tons (AT THE GLOBAL SCALE)
                            if( (so_far/1000) > (global_quotas.at(pop)))
                            {
                                populations.at(pop)->get_tac()->set_is_tac_exhausted(1);

                                prop_remaining_global_quotas.at(pop) =  (so_far/1000) / (global_quotas.at(pop));

                                dout (cout << "prop used " <<
                                      prop_remaining_global_quotas.at(pop)  <<
                                      "  global quota of " << global_quotas.at(pop) << " this pop: overshoot..." << endl);

                                // reaction
                                dout(cout  << "Global TAC reached...then discard all for this pop " << pop << "!!! " << endl);
                                dout(cout  << "...and declare you are choked by " << pop << "!!! " << endl);
                                this->set_is_choked(pop, 1);
                                populations.at(pop)->set_landings_so_far(so_far -a_cumul_weight_this_pop_this_vessel);
                                // => back correction (disable if you want to know the discarded part in annual_indic.
                                // ...i.e. discarded = so_far - current_tac)
                                // what a waste !!...
                                a_cumul_weight_this_pop_this_vessel=0;// discard all!

                                for(unsigned int szgroup=0; szgroup < catch_pop_at_szgroup[pop].size();++szgroup)
                                {
                                    dout(cout << "tac exhausted for pop "<< pop << ": discards all !!!!  because " << "(" << so_far << "/1000) > (" << global_quotas.at(pop) << ")" << endl);
                                    discards_pop_at_szgroup[pop][szgroup]+=catch_pop_at_szgroup[pop][szgroup];// discard all!
                                    catch_pop_at_szgroup[pop][szgroup]=0; // discard all! => no landings
                                    ping_catch_pop_at_szgroup[pop][szgroup]=discards_pop_at_szgroup[pop][szgroup]; // => catches=discards

                                }

                            }
                            else
                            {
                                dout (cout << "used " <<
                                      (so_far/1000) / (global_quotas.at(pop))*100  <<
                                      " % global quota of " << global_quotas.at(pop) << " this pop: ok." << endl);
                            }

                        }			 // end individual TAC management
                    } // end TAC management
                    else
                    {
                        // track the so_far here as well
                        double so_far=0.0;
                        so_far = (populations.at(pop)->get_landings_so_far()) +
                                a_cumul_weight_this_pop_this_vessel;
                        // "real-time" update, accounting for this last bit of catches
                        populations.at(pop)->set_landings_so_far(so_far);

                    }


                    // update dynamic trip-based cumul for this node
                    // CUMUL FOR THE TRIP (all species confounded)
                    this->cumcatches+= a_cumul_weight_this_pop_this_vessel;
                    this->get_loc()->set_cumcatches_per_pop(namepop, cumulcatches.at(pop) + a_cumul_weight_this_pop_this_vessel);
                    // catches
                    cumcatch_fgrounds.at(idx_node_r) += a_cumul_weight_this_pop_this_vessel;
                    // catches per pop
                    cumcatch_fgrounds_per_pop.at(idx_node_r).at(pop) += a_cumul_weight_this_pop_this_vessel;
                    // effort
                    cumeffort_fgrounds.at(idx_node_r) += PING_RATE;


                    // cumul to later compute the proportion of discard to potentially influence future decision-making
                    for (unsigned int sz=0; sz<landings_per_szgroup.size(); ++sz){
                        totLandThisEvent += landings_per_szgroup[sz];
                        totDiscThisEvent += discards_per_szgroup[sz];
                    }
                    this->cumdiscards+= totDiscThisEvent;
                    this->get_loc()->set_cumdiscards_per_pop(namepop, cumuldiscards.at(pop) + totDiscThisEvent);
                    cumdiscard_fgrounds.at(idx_node_r) += totDiscThisEvent;


                    if(this->get_metier()->get_is_avoided_stocks(pop)==1)
                    {
                        totAvoiStksLandThisEvent+= totLandThisEvent;
                        totAvoiStksDiscThisEvent+= totDiscThisEvent;
                    }


                    // check
                    /*
                     for(int i=0; i<new_Ns_at_szgroup_pop.size(); i++)
                     {
                         if(new_Ns_at_szgroup_pop[i]!=new_Ns_at_szgroup_pop[i])  // c++ trick for like testing for is.nan
                         {
                             int a;
                             dout(cout << "namepop " <<namepop << endl);
                             dout(cout << "Ns_at_szgroup_pop[szgroup] " <<Ns_at_szgroup_pop[i] << endl);
                             dout(cout << "wsz[szgroup] " <<wsz[i] << endl);
                             dout(cout << "sel_ogive[szgroup] " <<sel_ogive[i] << endl);
                             dout(cout << "avail_biomass[szgroup] " <<avail_biomass[i] << endl);
                             dout(cout << "alloc_key[szgroup] " <<alloc_key[i] << endl);
                             dout(cout << " catch_per_szgroup[szgroup] " << catch_per_szgroup[i] << endl);
                             dout(cout << " weight_per_szgroup[szgroup] " << wsz[i] << endl);
                             dout(cout << " removals_per_szgroup[szgroup] " << removals_per_szgroup[i] << endl);
                             dout(cout << " new_Ns_at_szgroup_pop[szgroup] " << new_Ns_at_szgroup_pop[i] << endl);

                             dout(cout << "here: nan detected" << endl);
                             dout(cout << "here: nan detected...Pause: type a number to continue");
                             cin >> a;
                         }
                     }
                     */

                    // check
                    //if(tot_catch_per_pop[pop]==0 || tot==0){
                    //   // make a pause and check...
                    //    int a;
                    //    dout(cout << "Pause: type a number to continue");
                    //    cin >> a;
                    //}

                }
                else
                {
                    dout(cout  << "tot biomass available on this node is 0 => no catch, sorry... " << endl);
                }

                // check for consistency i.e. no gain in N !!
                //for(unsigned int szgroup=0; szgroup <init_Ns_at_szgroup_pop.size(); szgroup++)
                //{
                //    if(new_Ns_at_szgroup_pop.at(szgroup) > init_Ns_at_szgroup_pop.at(szgroup))
                //    {
                //        cout << "inconsistency in vessel->do_catch() for this pop " << populations.at(pop)->get_name() <<
                //             " on this node " << idx_node <<
                //            " for this szgroup " <<  szgroup <<endl;
                //        dout(cout << "new_Ns_at_szgroup_pop is " << new_Ns_at_szgroup_pop.at(szgroup) << endl);
                //        dout(cout << "while init_Ns_at_szgroup_pop is " << init_Ns_at_szgroup_pop.at(szgroup) << endl);
                //        int a_int;
                //        cin >> a_int; // pause
                //    }
                //}

            }
            else
            {
                dout(cout  << "no indiv of this pop " << populations[pop]->get_name() << " on this node... " << endl);
            }



            // CHECK
            // what is exported in loglike file for this pop is:
            //if(this->get_name()=="GRK_KERK57_1_51" && pop==4){
            //   double cumul=0.0;
            //    for(size_t sz = 0; sz < catch_pop_at_szgroup[pop].size(); sz++)
            //    {
            //        cumul = cumul + catch_pop_at_szgroup[pop][sz];
            //    }
            //cout << " actually the catches for this explicit pop "<< pop << " that would be exported if trip would end now" << cumul << endl;
            //}



        }
        else  // implicit pop:
        {
            dout(cout  << "this pop " << populations.at(pop)->get_name() << " is implicit (or outside the range)...catch knowing cpue only! " << endl);
            // tips: put tot catch into the first bin...we don´t care which szgroup here...
            //vector<double> cpues = find_entries_i_d (cpue_per_stk_on_nodes, idx_node);
            auto grds = this->get_fgrounds();
            // relative node index to this vessel
            int idx_node_v= find(grds.begin(), grds.end(), idx_node) - grds.begin();
            //if((this->get_name())=="DNK000007718")cout <<this->get_name() << ": the cpue for this pop " << populations.at(pop)->get_name()
            //        << " on this node "<< idx_node << endl;

            //double cpue = cpue_nodes_species.at(idx_node_v).at(pop); // look into the vector of vector....
            // replaced by:
            // look into the vector of vector....
            double a_shape;
            double a_scale;
            double cpue;
            if(idx_node_v<(int)gshape_cpue_nodes_species.size()){
                a_shape = gshape_cpue_nodes_species.at(idx_node_v).at(pop);
                // look into the vector of vector....
                a_scale = gscale_cpue_nodes_species.at(idx_node_v).at(pop);

                // a dangerous fix because potential for silently messing up the parameterisation
                // messy espacially if a_scale should be stricly 0
                // (when this vessel is actually not fishing this stock):
                if(a_shape<=0 || a_scale <0)
                {

                    //cout << "Something weird with the Gamma parameters: some negative values loaded...." << endl;
                    //for(size_t f = 0; f < fgrounds.size(); ++f)
                    //{
                    //cout <<  " this gr  gscale is: " << gscale_cpue_nodes_species.at(f).at(pop) << endl;
                    //cout <<  " this gr  of gshape is: " << gshape_cpue_nodes_species.at(f).at(pop) << endl;
                    //}
                    a_shape=1;
                    a_scale=1; //  check with hist(rgamma(100, shape=1, scale=1))

                }
                cpue = rgamma(a_shape, a_scale);
            }
            else
            { // it occurs when fgrounds are not the initial ones...e.g. usual_fgrounds from harbour
                // (in this case the location is not that important (because implicit species),
                // it is just assuming that the TAC for this species will be exhausted wherever the area it is caught)
                int randomIndex = rand() % gshape_cpue_nodes_species.size();
                a_shape = gshape_cpue_nodes_species.at(randomIndex).at(pop);
                // look into the vector of vector....
                a_scale = gscale_cpue_nodes_species.at(randomIndex).at(pop);

                // a dangerous fix:
                if(a_shape<0 || a_scale <0)
                {

                    cout << "Something weird with the Gamma parameters: some negative values loaded...." << endl;
                    //for(size_t f = 0; f < fgrounds.size(); ++f)
                    //{
                    //cout <<  " this vessel is is: " << this->get_name() << endl;
                    //cout <<  " this gr  gscale is: " << gscale_cpue_nodes_species.at(f).at(pop) << endl;
                    //cout <<  " this gr  of gshape is: " << gshape_cpue_nodes_species.at(f).at(pop) << endl;
                    //}
                    a_shape=1;
                    a_scale=0;
                }
                cpue = rgamma(a_shape, a_scale);

            }
           dout(cout  << "implicit: cpue for pop " << pop << " is " << cpue << endl);

            // TAC management effect for implicit species
            if(tstep>1 && is_tacs && is_individual_vessel_quotas)
            {
                // check the individual quota for this pop
                double a_cumul_weight_this_pop_this_vessel=0;
                int remaining_individual_tac_this_pop = this->get_individual_tac(pop);
                if(remaining_individual_tac_this_pop <=0)
                {
                    dout(cout  << this->get_name() << ": individual quota this IMPLICIT pop"<< pop << " EXHAUSTED! " << endl);
                    cpue = 0; // quota exhausted for this implicit stock: no catch
                }
                else
                {
                    dout(cout  << this->get_name() <<  ": individual quota this IMPLICIT pop  "<< pop <<
                         " still ok...but now decrease the amount by the last catches. Note that it remains "<< remaining_individual_tac_this_pop << endl);
                    a_cumul_weight_this_pop_this_vessel=cpue*PING_RATE;
                    this->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, implicit_pops, pop, 0,
                                                      remaining_individual_tac_this_pop- a_cumul_weight_this_pop_this_vessel);
                    dout(cout  << this->get_name() <<  ": individual quota this IMPLICIT pop  "<< pop <<
                         " is now "<< remaining_individual_tac_this_pop- a_cumul_weight_this_pop_this_vessel << endl);
                }
            }


            dout (cout<<this->get_name() << ": the cpue for this pop " << populations.at(pop)->get_name()
                  << " on this node "<< idx_node << " is " << cpue << " given " << a_shape << " " << a_scale <<  endl);

            if(cpue!=0)
            {
                // ON THIS NODE, cpue per hour, see the R code
                catch_pop_at_szgroup[pop][0] += cpue*PING_RATE;
                ping_catch_pop_at_szgroup[pop][0]=cpue*PING_RATE;
                discards_pop_at_szgroup[pop][0] = 0;



                // CUMUL
                this->cumcatches+= catch_pop_at_szgroup[pop][0];
                this->cumdiscards+= discards_pop_at_szgroup[pop][0];
                this->get_loc()->set_cumcatches_per_pop(namepop, cumulcatches.at(pop) + catch_pop_at_szgroup[pop][0]);
                this->get_loc()->set_cumdiscards_per_pop(namepop, cumuldiscards.at(pop) + 0);
                /*
                if(pop==21 && (this->get_name())=="DNK000012028"){
                    dout(cout << "cpue " <<cpue << endl);
                    dout(cout << "catch_pop_at_szgroup[pop][0] " <<catch_pop_at_szgroup[pop][0] << endl);
                        int a;
                        dout(cout << "Pause: type a number to continue");
                        cin >> a;
                }
                */

                // update dynamic trip-based cumul for this node
                // catches
                cumcatch_fgrounds.at(idx_node_r) += cpue*PING_RATE;
                cumdiscard_fgrounds.at(idx_node_r) += 0;
                // catches per pop
                cumcatch_fgrounds_per_pop.at(idx_node_r).at(pop) += cpue*PING_RATE;
                cumdiscard_fgrounds_per_pop.at(idx_node_r).at(pop) += 0;
                // effort
                cumeffort_fgrounds.at(idx_node_r) += PING_RATE;

            }
            else
            {
                catch_pop_at_szgroup.at(pop).at(0) +=0;
                ping_catch_pop_at_szgroup.at(pop).at(0)+=0;
            }
        }



        // contribute to accumulated catches on this node
        this->get_loc()->add_to_cumcatches_per_pop(cumcatch_fgrounds_per_pop.at(idx_node_r).at(pop), pop);
        this->get_loc()->add_to_cumdiscards_per_pop(cumdiscard_fgrounds_per_pop.at(idx_node_r).at(pop), pop);


        // collect and accumulate tot_C_at_szgroup at the end of the pop loop
        // (accumulate in szgroup 0 if implicit pop)
        vector <double> newTotC= populations.at(pop)->get_tot_C_at_szgroup();
        vector <double> newTotD= populations.at(pop)->get_tot_D_at_szgroup();
        for(unsigned int szgroup=0; szgroup < catch_pop_at_szgroup[pop].size();++szgroup)
        {
           newTotC.at(szgroup) = newTotC.at(szgroup) + catch_pop_at_szgroup.at(pop).at(szgroup);
           newTotD.at(szgroup) = newTotD.at(szgroup) + discards_pop_at_szgroup.at(pop).at(szgroup);
        }
        populations.at(pop)->set_tot_C_at_szgroup(newTotC);
        populations.at(pop)->set_tot_D_at_szgroup(newTotD);

    } // end pop


    // compute the proportion of discard of this event on that ground
    // (of all explicit species AND on selected stocks i.e. the avoided ones) to potentially influence future decision-making (see ChooseGround dtree)
    experienced_bycatch_prop_on_fgrounds.at(idx_node_r)= totDiscThisEvent/(totLandThisEvent+totDiscThisEvent);
    experienced_avoided_stks_bycatch_prop_on_fgrounds.at(idx_node_r)= totAvoiStksDiscThisEvent/(totAvoiStksLandThisEvent+totAvoiStksDiscThisEvent);


    // contribute to accumulated catches on this node
    this->get_loc()->add_to_cumcatches(cumcatch_fgrounds.at(idx_node_r));
    this->get_loc()->add_to_cumdiscards(cumdiscard_fgrounds.at(idx_node_r));
    double discratio =  this->get_loc()->get_cumdiscards() / (this->get_loc()->get_cumdiscards()+this->get_loc()->get_cumcatches());
    discratio = discratio>0 ? discratio : 0.0;
    this->get_loc()->set_cumdiscardsratio(discratio);


    // check the matrix of catches
    //double a_cumul=0;
    //dout(cout  << "in do_catch(): after: CATCH PER NBSZGROUP" << endl);
    //for(int i = 0; i < catch_pop_at_szgroup.size(); i++)
    //{
    //    for(int j = 0; j < catch_pop_at_szgroup[i].size(); j++)
    //   {
    //        dout(cout  << catch_pop_at_szgroup[i][j] << " ");
    //        a_cumul +=catch_pop_at_szgroup[i][j];
    //    }
    //    dout(cout  << endl);
    //}

    // add the metier for this ping
    this->idx_used_metiers_this_trip.push_back(this->get_metier()->get_name());

    // make a pause and check...
    //if(a_cumul=0){
    //  int a;
    //  dout(cout << "Pause: type a number to continue");
    //  cin >> a;
    //  }

    // check the name of the active metier (ground specific)
    //int met = this->get_metier()->get_name();
    //dout(cout  << "the active metier is " << met << endl);

    dout(cout  << "END do_catch()" << endl);

    unlock();
}


void Vessel::clear_catch_pop_at_szgroup()
{
    dout(cout  << "clear catches..." << endl);
    for(unsigned int i = 0; i < catch_pop_at_szgroup.size(); i++)
    {
        for(unsigned int j = 0; j < catch_pop_at_szgroup[i].size(); j++)
        {
            catch_pop_at_szgroup[i][j] = 0;
        }
    }
}

void Vessel::clear_ping_catch_pop_at_szgroup()
{
    dout(cout  << "clear catches..." << endl);
    for(unsigned int i = 0; i < ping_catch_pop_at_szgroup.size(); i++)
    {
        for(unsigned int j = 0; j < ping_catch_pop_at_szgroup[i].size(); j++)
        {
            ping_catch_pop_at_szgroup[i][j] = 0;
        }
    }
}



void Vessel::clear_discards_pop_at_szgroup()
{
    dout(cout  << "clear catches..." << endl);
    for(unsigned int i = 0; i < discards_pop_at_szgroup.size(); i++)
    {
        for(unsigned int j = 0; j < discards_pop_at_szgroup[i].size(); j++)
        {
            discards_pop_at_szgroup[i][j] = 0;
        }
    }
}

//------------------------------------------------------------//
//------------------------------------------------------------//
// related to individual choice of grounds...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::compute_experiencedcpue_fgrounds()
{
    double cum_cpue=0;

    // note that, at the tstep=0, no one single node has been visited yet, so experiencedcpue_fgrounds is full a guesses !
    // but there are qualified guesses: actually cpue from the frequency given by the input data...

    outc(cout  << "compute experienced cpue on grounds and clear cum effort and catch..." << endl);
    for(unsigned int a_node = 0; a_node < experiencedcpue_fgrounds.size(); a_node++)
    {
        // change cpue only if the node have been visited...otherwise the initial guess for cpue is kept
        if(cumeffort_fgrounds.at(a_node)!=0)
        {
            //outc(cout  << "on the grounds of this vessel cumcatch is " << cumcatch_fgrounds.at(a_node) << endl);
            //outc(cout  << "on the grounds of this vessel cumeffort is " << cumeffort_fgrounds.at(a_node) << endl);
            experiencedcpue_fgrounds.at(a_node)= cumcatch_fgrounds.at(a_node) / cumeffort_fgrounds.at(a_node);
            //outc(cout  << "on the grounds of this vessel experienced cpue is then " << experiencedcpue_fgrounds.at(a_node) << endl);
        }
        // cumul to scale to 1 (just below)
        cum_cpue +=experiencedcpue_fgrounds.at(a_node);

    }

    // caution: if the expected_cpue have been badly estimated, then the first ground chosen will highly influence
    // the frequency of visit for the following steps...
    // because if experienced_cpue >> expected_cpue then freq on first ground will be close to 1
    // because if experienced_cpue << expected_cpue then freq on first ground will be close to 0

    //  scale to 1 for use in do_sample() => freq_experiencedcpue_fgrounds
    if(cum_cpue!=0)
    {
        for(unsigned int a_node = 0; a_node < experiencedcpue_fgrounds.size(); a_node++)
        {
            freq_experiencedcpue_fgrounds.at(a_node)= experiencedcpue_fgrounds.at(a_node) / cum_cpue;
            //outc(cout  << "scaled experienced cpue is then " << freq_experiencedcpue_fgrounds.at(a_node) << endl);
        }
    }
    outc(cout  << "compute experienced cpue on grounds...OK" << endl);
}


void Vessel::compute_experiencedcpue_fgrounds_per_pop()
{
    vector<double> cum_cpue_over_pop;

    // note that, at the tstep=0, no one single node has been visited yet, so experiencedcpue_fgrounds is full of guesses!
    // but there are qualified guesses: actually cpue from the frequency given by the input data...

    outc(cout  << "compute experienced cpue on grounds per pop and clear cum effort and catch..." << endl);
    for(unsigned int a_node = 0; a_node < experiencedcpue_fgrounds_per_pop.size(); a_node++)
    {
        cum_cpue_over_pop.push_back(0);

        for(unsigned int pop = 0; pop < experiencedcpue_fgrounds_per_pop[a_node].size(); pop++)
        {
            // change cpue only if the node have been visited...otherwise the initial guess for cpue is kept
            if(cumeffort_fgrounds.at(a_node)!=0)
            {
                //dout(cout  << "on the grounds of this vessel cumcatch is " << cumcatch_fgrounds_per_pop.at(a_node).at(pop) << endl);
                //dout(cout  << "on the grounds of this vessel cumeffort is " << cumeffort_fgrounds.at(a_node) << endl);
                experiencedcpue_fgrounds_per_pop.at(a_node).at(pop)= cumcatch_fgrounds_per_pop.at(a_node).at(pop) / cumeffort_fgrounds.at(a_node);
                //dout(cout  << "on this ground, this vessel experienced a cpue of " << experiencedcpue_fgrounds_per_pop.at(a_node).at(pop) << endl);
            }
            // cumul to scale to 1 (just below)
            cum_cpue_over_pop.at(a_node) +=experiencedcpue_fgrounds_per_pop.at(a_node).at(pop);

            //  scale to 1 for use in do_sample() => freq_experiencedcpue_fgrounds_per_pop
            if(cum_cpue_over_pop.at(a_node)!=0)
            {
                for(unsigned int pop = 0; pop < experiencedcpue_fgrounds_per_pop[a_node].size(); pop++)
                {
                    freq_experiencedcpue_fgrounds_per_pop.at(a_node).at(pop)= experiencedcpue_fgrounds_per_pop.at(a_node).at(pop) / cum_cpue_over_pop.at(a_node);
                    //dout(cout  << "scaled experienced cpue this pop is then " << freq_experiencedcpue_fgrounds_per_pop.at(a_node).at(pop) << endl);
                }
            }

        }

    }
    outc(cout  << "experienced cpue on grounds per pop...OK" << endl);

}


void Vessel::clear_cumcatch_and_cumeffort()
{
    auto the_grds = this->get_fgrounds();
    for(unsigned int n=0; n<the_grds.size(); n++)
    {
        for(unsigned int pop=0; pop< cumcatch_fgrounds_per_pop.at(n).size(); pop++)
        {
            //clear
            cumcatch_fgrounds_per_pop.at(n).at(pop) =0;
            cumdiscard_fgrounds_per_pop.at(n).at(pop) =0;
        }
        //clear
        cumcatch_fgrounds.at(n) =0;
        cumdiscard_fgrounds.at(n) =0;
        //clear
        cumeffort_fgrounds.at(n) =0;
    }
}


void Vessel::alter_freq_fgrounds_for_nodes_in_polygons(multimap <int, int> nodes_in_polygons)
{

    dout(cout  << "BEGIN alter_freq_fgrounds_for_nodes_in_polygons" << endl);

    vector<int> polygons;
    vector<int> polygon_nodes;

    for (multimap<int, int>::iterator pos=nodes_in_polygons.begin(); pos != nodes_in_polygons.end(); pos++)
    {
        polygons.push_back(pos->first);
        polygon_nodes.push_back(pos->second);
        dout(cout  << " a polygon node is " << pos->second << endl);
    }
    // essential to be used with binary.search!!
    sort (polygon_nodes.begin(), polygon_nodes.end());

    // remember that the nodes_in_polygons will not be used for traveling (because of the distance penalty)
    // UNLESS the node is a chosen final destination...
    // note that the vessel will not be trapped by the algo because real distances between connected nodes are
    // calculated in do_move() (i.e. the dist.km in the graph object is actually not used during the simulation
    // so we dont care about the penalty)

    auto the_grds = this->get_fgrounds();
    vector <double> the_freq_grds = this->get_freq_fgrounds();

    // here a code to cancel (or at least lower down) the visit into the regulated polygons:
    // screen and put very low proba of visit eg 0.0001 if grounds inside the polygons
    // (if >0 then not assuming a total compliance here...)
    // (this is also convenient to avoid dividing  0 by something!!)
    double cumul=0;
    unsigned int count=0;
    // should be equal...otherwise check the input files!
    dout(cout  << "the_grds.size() " << the_grds.size() << "the_freq_grds.size() " << the_freq_grds.size() << endl);

    for(unsigned int n=0; n<the_grds.size(); n++)
    {
        dout(cout  << "the ground? " << the_grds.at(n) << endl);

        if (binary_search (polygon_nodes.begin(), polygon_nodes.end(), the_grds.at(n).toIndex()))
        {
            the_freq_grds.at(n)=0.00000000000001;
            dout(cout  << "change frequency to 0.0001 for the ground " << the_grds.at(n) << endl);
            count=count+1;
        }
        cumul += the_freq_grds.at(n);

    }

    if(count==the_grds.size()) {
        dout(cout << "all grounds are included in the restricted polygons (!) for this vessel " << this->get_name() << endl);
    }
    // => then this vessel will still fish on those grounds (with equal proba by the way), assuming non-compliance...
    // TO DO: need to think to an alternative model here....

    // then re-scale to 1
    for(unsigned int n=0; n<the_grds.size(); n++)
    {
        the_freq_grds.at(n)= the_freq_grds.at(n)/cumul;
    }

    // update the the_freq_grds
    // ....output
    this-> set_spe_freq_fgrounds (the_freq_grds);

    dout(cout  << "change the frequencies...done" << endl);
}


void Vessel::alloc_on_high_previous_cpue(int tstep,
                                         ofstream& freq_cpue)
{

    // input...
    vector <double> freq_grds = this->get_freq_fgrounds();
    // get_experiencedcpue_fgrounds is scaled to 1
    vector <double> past_freq_cpue_grds = this-> get_freq_experiencedcpue_fgrounds();

    double a_sum =0;
    double a_sum2=0;
    int idx_v = this->get_idx();
    freq_cpue << setprecision(3) << fixed;
    freq_cpue << tstep << " " << idx_v << " ";
    for(unsigned int ii = 0; ii < past_freq_cpue_grds.size(); ii++)
    {
        dout(cout   << "observed frequency:" << past_freq_cpue_grds.at(ii) << " " << endl);
        a_sum+= (past_freq_cpue_grds.at(ii)*freq_grds.at(ii));
        freq_cpue << past_freq_cpue_grds.at(ii) << " " ;
    }

    freq_cpue << 100 << " " ;	 // arbitrary column limit

    for(unsigned int ii = 0; ii < freq_grds.size(); ii++)
    {
        dout(cout   << "prior frequency:" << freq_grds.at(ii) << " " << endl);
        freq_cpue << freq_grds.at(ii) << " " ;
    }

    freq_cpue << 100 << " " ;	 // arbitrary column limit

    for(unsigned int ii = 0; ii < past_freq_cpue_grds.size(); ii++)
    {
        // update freq with the bayes'formula P(H1/D)=P(D/H1)*P(H1) / (P(D/H1)*P(H1)  +  P(D/H2)*P(H2) + ....)
        past_freq_cpue_grds.at(ii)= (past_freq_cpue_grds.at(ii)*freq_grds.at(ii)) / a_sum;
        dout(cout   << "updated frequency:" << past_freq_cpue_grds.at(ii) << " " << endl);
        a_sum2+=past_freq_cpue_grds.at(ii);
        freq_cpue << past_freq_cpue_grds.at(ii) << " " ;

    }
    dout(cout   << "a_sum2:" << a_sum2 << " " << endl);
    freq_cpue << endl;

    // update the prior for the next time (iterative bayesian process)
    // ...output
    this-> set_spe_freq_fgrounds(past_freq_cpue_grds);

}


vector<double> Vessel::expected_profit_on_grounds(int use_static_paths,
                                                  vector<Node* > &nodes,
                                                  const std::vector<types::NodeId> &relevant_nodes,
                                                  const std::vector<PathShop> &pathshops)
{

    outc(cout << "compute expected profit on grounds " << endl);
//cout << "compute expected profit on grounds " << endl;
    double price_multiplier=1.0;

    vector <double> freq_grds = this->get_freq_fgrounds();
    // get_experiencedcpue_fgrounds_per_pop is scaled to 1
    vector <vector<double> > past_freq_cpue_grds_pops = this-> get_freq_experiencedcpue_fgrounds_per_pop();
    vector <vector<double> > past_freq_cpue_grds_targts =past_freq_cpue_grds_pops;

    vector <double> revenue_per_fgrounds(freq_grds.size());
    vector <double> fcost_per_fgrounds(freq_grds.size());
    vector <double> scost_per_fgrounds(freq_grds.size());
    vector <double> profit_per_fgrounds(freq_grds.size());

    // distance to all grounds (through the graph...)
    auto from = this->get_loc()->get_idx_node();
    auto the_grounds = this->get_fgrounds();

    vector <double> distance_fgrounds;
    if(use_static_paths)
    {
         distance_fgrounds= compute_distance_fgrounds(relevant_nodes, pathshops,
                                                                      from, the_grounds);

    }
    else
    {
        // cartesian approx. actually...
        distance_fgrounds= compute_distance_fgrounds_on_the_fly(nodes, from, the_grounds);
    }


    // vsize
    int length_class =this->get_length_class();


    vector<int>  trgts =this->get_metier()->get_metier_target_stocks();
    vector<double> cum_cpue_over_trgt_pop(freq_grds.size());
    for(unsigned int gr=0; gr<freq_grds.size(); gr++)
    {

        // note that cpues sum to 1 over ALL pops...
        // ...then before all, given that the revenue is computed from the targeted pops only,
        // we need to rescale the past cpues to sum to 1 over the TARGT pop only.
        for(unsigned int i=0; i<trgts.size(); ++i)
        {
            int a_trgt=trgts.at(i);
            dout(if(past_freq_cpue_grds_pops.at(gr).size()<trgts.size()) cout << "error: inconsistent nb of pops in config.dat vs. nb of targets in metier files")
            cum_cpue_over_trgt_pop.at(gr) +=past_freq_cpue_grds_pops.at(gr).at(a_trgt);
        }
        //  scale to 1
        if(cum_cpue_over_trgt_pop.at(gr)!=0)
        {
            for(unsigned int i=0; i<trgts.size(); ++i)
            {
                int a_trgt=trgts.at(i);
                past_freq_cpue_grds_targts.at(gr).at(a_trgt)= past_freq_cpue_grds_pops.at(gr).at(a_trgt) / cum_cpue_over_trgt_pop.at(gr);
            }
        }


        //1. first, compute the expected revenue for full vessel load on this ground knowing the experienced cpues
        // and the fish price on the departure harbour. (caution, the vessel is assumed to currently be at the port)
        // .....only looking at the targeted stocks

        double tot_revenue=0.0;
        for(unsigned int i=0; i<trgts.size(); ++i)
        {
            int a_trgt=trgts.at(i);
            revenue_per_fgrounds.at(gr)+= past_freq_cpue_grds_targts.at(gr).at(a_trgt) * // weighted average of cpues
                    this->get_carrycapacity() *
                    // choose the most valuable cat (but actually currently the first one is returned: to do)
                    this->get_loc()->get_prices_per_cat(a_trgt, 0) * price_multiplier;

            tot_revenue+=revenue_per_fgrounds.at(gr);
        }

        if(tot_revenue==0) // we shouldn´t expect this...
        {
 // cout << this->get_name() << ": Pblm in metier definition vs. targets (past cpues on tgrt pops from gscale gshape likely to be 0s)...then expand the search to all pops! "  << endl;
            outc(cout << this->get_name() << ": Pblm in metier definition vs. targets (past cpues on tgrt pops from gscale gshape likely to be 0s)...then expand the search to all pops! "  << endl);
            for(unsigned int pop=0; pop<past_freq_cpue_grds_pops.at(gr).size(); ++pop)
            {
                revenue_per_fgrounds.at(gr)+= past_freq_cpue_grds_pops.at(gr).at(pop) * // weighted average of cpues
                        this->get_carrycapacity() *
                        // choose the most valuable cat (but actually currently the first one is returned: to do)
                        this->get_loc()->get_prices_per_cat(pop, 0)* price_multiplier;

                tot_revenue+=revenue_per_fgrounds.at(gr);
            }

        }


 //cout << "given the capacity "   << this->get_carrycapacity() << ", the expected revenue on this ground is " << revenue_per_fgrounds.at(gr) << endl;
 //cout << "ground " << gr << endl;

        //2. compute the expected cost when steaming
        // time given the shortest distance divided by the speed...
        double time_for_steaming=0;
        // *2 because WE NEED TO GO BACK TO PORT!
 //cout << "size of distance_fgrounds " << distance_fgrounds.size() << endl;
        time_for_steaming= (distance_fgrounds.at(gr)/this->get_speed())*2;
 //cout << "the expected time to reach this ground is " << time_for_steaming << endl;
 //cout << "fuel price " << this->get_loc()->get_fuelprices(length_class) << endl;
 //cout << "this->get_mult_fuelcons_when_steaming() " << this->get_mult_fuelcons_when_steaming()<< endl;
 //cout << "fuel cons " <<  this->get_fuelcons() << endl;

 scost_per_fgrounds.at(gr)=   time_for_steaming * this->get_loc()->get_fuelprices(length_class) * this->get_fuelcons() *  this->get_mult_fuelcons_when_steaming();
 //cout << "the expected scost on this ground is " << scost_per_fgrounds.at(gr) << endl;

        //3. compute the expected cost when fishing
        double time_to_be_full_of_catches_if_infinite_fuel_tank=0;
        double cpue_this_node = this->experiencedcpue_fgrounds.at(gr);
        if(cpue_this_node!=0)
        {
            time_to_be_full_of_catches_if_infinite_fuel_tank = this->get_carrycapacity() / cpue_this_node;
//cout << "the expected time to fill in the capacity on this ground is " << time_to_be_full_of_catches_if_infinite_fuel_tank << endl;
        }
        else
        {
            time_to_be_full_of_catches_if_infinite_fuel_tank = 100;
        }

        double time_for_fishing_given_fuel_tank= (this->get_tankcapacity() -
                                                  ( time_for_steaming * this->get_fuelcons() *  this->get_mult_fuelcons_when_steaming())) /
                (this->get_fuelcons()) ;// (tank - expected tot fuelcons when steaming) / conso per hour when fishing
//cout << "the expected time to empty the fuel tank on this ground is " << time_for_fishing_given_fuel_tank << endl;

        double time_for_fishing= min(time_to_be_full_of_catches_if_infinite_fuel_tank, time_for_fishing_given_fuel_tank);
//cout << "then, the expected time for fishing on this ground is " << time_for_fishing << endl;

        fcost_per_fgrounds.at(gr)=time_for_fishing * this->get_loc()->get_fuelprices(length_class) * this->get_fuelcons()  *  this->get_mult_fuelcons_when_fishing();
//cout << "the expected fcost on this ground is " << fcost_per_fgrounds.at(gr) << endl;

        //4. then compute the expected profit for this ground
        profit_per_fgrounds.at(gr)= revenue_per_fgrounds.at(gr) -
                scost_per_fgrounds.at(gr) -
                fcost_per_fgrounds.at(gr);
 //cout << "the expected profit on this ground is " << profit_per_fgrounds.at(gr) << endl;

    }


 //cout << "compute expected profit on grounds...OK " << endl;
    outc(cout << "compute expected profit on grounds...OK " << endl);

    return(profit_per_fgrounds);

}

void Vessel::alloc_on_high_profit_grounds(int tstep,
                                          int use_static_paths,
                                          vector<Node* >&nodes,
                                          const std::vector<types::NodeId> &relevant_nodes,
                                          const std::vector<PathShop> &pathshops,
                                          ofstream& freq_profit)
{


    vector<double> profit_per_fgrounds = expected_profit_on_grounds(use_static_paths,
                                                                    nodes,
                                                                    relevant_nodes,
                                                                    pathshops);


    vector <double> freq_grds = this->get_freq_fgrounds();
    // get_experiencedcpue_fgrounds_per_pop is scaled to 1

    // if(tstep>1) dout(cout << "an expected profit per ground has been estimated..." << endl);

    //  finally, scale to 1
    double cum_profit=0;
    for(unsigned int a_node = 0; a_node < profit_per_fgrounds.size(); a_node++)
    {
        cum_profit+=profit_per_fgrounds.at(a_node);
    }
    for(unsigned int a_node = 0; a_node < profit_per_fgrounds.size(); a_node++)
    {
        profit_per_fgrounds.at(a_node)= profit_per_fgrounds.at(a_node) / cum_profit;
        //if(tstep>1) dout(cout << "scaled expected profit per ground is then " << profit_per_fgrounds.at(a_node) << endl);
    }

    // the update procedure...
    double a_sum =0;
    double a_sum2=0;
    //	int idx_v = this->get_idx();
    //freq_profit << setprecision(3) << fixed;
    //freq_profit << tstep << " " << idx_v << " ";
    for(unsigned int ii = 0; ii < profit_per_fgrounds.size(); ii++)
    {
        //if(tstep>1) dout(cout  << "observed frequency:" << profit_per_fgrounds.at(ii) << " " << endl);
        a_sum+= (profit_per_fgrounds.at(ii)*freq_grds.at(ii));
        //freq_profit << profit_per_fgrounds.at(ii) << " " ;
    }

    //freq_profit << 100 << " " ; // arbitrary column limit

    for(unsigned int ii = 0; ii < freq_grds.size(); ii++)
    {
        //if(tstep>1) dout(cout  << "prior frequency:" << freq_grds.at(ii) << " " << endl);
        //freq_profit << freq_grds.at(ii) << " " ;
    }

    //freq_profit << 100 << " " ;  // arbitrary column limit

    for(unsigned int ii = 0; ii < profit_per_fgrounds.size(); ii++)
    {
        // update freq with the bayes'formula P(H1/D)=P(D/H1)*P(H1) / (P(D/H1)*P(H1)  +  P(D/H2)*P(H2) + ....)
        profit_per_fgrounds.at(ii)= (profit_per_fgrounds.at(ii)*freq_grds.at(ii)) / a_sum;
        //if(tstep>1) dout(cout  << "updated frequency:" << profit_per_fgrounds.at(ii) << " " << endl);
        a_sum2+=profit_per_fgrounds.at(ii);
        //freq_profit << profit_per_fgrounds.at(ii) << " " ;

    }
    // should return 1...
    dout(cout   << "a_sum2:" << a_sum2 << " " << endl);
    //freq_profit << endl;

    // update the prior for the next time (iterative bayesian process)
    // ...output
    this-> set_spe_freq_fgrounds(profit_per_fgrounds);

}


void Vessel::alloc_while_saving_fuel(int tstep,
                                     int use_static_paths,
                                     vector<Node*>& nodes,
                                     const vector <types::NodeId>& relevant_nodes,
                                     const std::vector<PathShop> &pathshops
                                     )
{
    UNUSED(tstep);
    // note that the brute search used below is expected to be tractable for 3 grounds only.
    // 20 percent increase
    double percent_increase_fuel_price=20;

    // input...
    vector <double> freq_grds = this->get_freq_fgrounds();

    // vessel specific conso per nm
    double conso_per_nm = this->get_fuelcons() /  this->get_speed();

    if(freq_grds.size()>2)		 // otherwise, do nothing.
    {

        // distance to all grounds (through the graph...)
        auto from = this->get_loc()->get_idx_node();
        auto the_grounds = this->get_fgrounds();

        vector <double> distance_fgrounds;
        if(use_static_paths)
        {
             distance_fgrounds= compute_distance_fgrounds(relevant_nodes, pathshops,
                                                                          from, the_grounds);

        }
        else
        {
            // cartesian approx. actually...
            distance_fgrounds= compute_distance_fgrounds_on_the_fly(nodes, from, the_grounds);
        }


        // find the freq for the 3 most used grounds and track their idx.
        // the value
        double pg1= *max_element (freq_grds.begin(),freq_grds.end());
        // tricky!
        int id1 = max_element (freq_grds.begin(),freq_grds.end()) - freq_grds.begin();
        freq_grds.at(id1) =0;
        // the value
        double pg2= *max_element (freq_grds.begin(),freq_grds.end());
        // tricky!
        int id2 = max_element (freq_grds.begin(),freq_grds.end()) - freq_grds.begin();
        freq_grds.at(id2) =0;
        // the value
        double pg3= *max_element (freq_grds.begin(),freq_grds.end());
        // tricky!
        int id3 = max_element (freq_grds.begin(),freq_grds.end()) - freq_grds.begin();

        dout(cout  << "vessel " << this->get_name() << " ");
        dout(cout  << "before: pg1 is " <<pg1 << " pg2 is " << pg2 << " pg3 is " << pg3 << endl);

        // the distances
        double dg1= distance_fgrounds.at(id1);
        double dg2= distance_fgrounds.at(id2);
        double dg3= distance_fgrounds.at(id3);

        double departure=1000;	 // init
        // init
        double min_departure=1000;
        // give the portion of the total (i.e. 1) on which we play with.
        double max_proba = round((pg1+pg2+pg3)*100);
        double expected_conso= (pg1*dg1*conso_per_nm)+(pg2*dg2*conso_per_nm)+(pg3*dg3*conso_per_nm);

        // the targeted conso is:
        double new_conso = expected_conso * (1- (percent_increase_fuel_price/100));

        dout(cout  << "the expected_conso is " <<expected_conso << "the new_conso is " << new_conso  <<  " the max_proba is " << max_proba << endl);

        // brute search along range of probas.
        // using a varying threshold to account for larger (defined as 5% of the conso)
        double threshold_to_break=new_conso*0.01;
        double p1 = 0, p2 = 0, p3 = 0;
        int a_max = max_proba+1;
        for(int i=0; i< a_max; i++)
        {
            for(int j=0; j< a_max; j++)
            {
                for(int k=0; k< a_max; k++)
                {
                    p1 = i*0.01;
                    p2 = j*0.01;
                    p3 = k*0.01;
                    //cout << "(p1+p2+p3) " << (p1+p2+p3) << " and max_proba " << max_proba << endl;
                    if((p1+p2+p3)==(max_proba/100))
                    {
                        departure = abs( ((p1 * dg1 *conso_per_nm) + (p2 * dg2 *conso_per_nm) + (p3 * dg3 *conso_per_nm)) -  new_conso );
                        if(departure <min_departure) min_departure=departure;
                        dout(cout  << departure << endl);
                    }
                    // early stop
                    if(departure <threshold_to_break) break;
                }
                if(departure <threshold_to_break) break;
            }
            if(departure <threshold_to_break) break;
        }

        // repeat with less constraint if failed.
        // no solution found, so...
        if(departure >threshold_to_break)
        {
            // search for less saving by decreasing the saving to 20-10 = 10%
            threshold_to_break=new_conso*0.1;
            for(int i=0; i< a_max; i++)
            {
                for(int j=0; j< a_max; j++)
                {
                    for(int k=0; k< a_max; k++)
                    {
                        p1 = i*0.01;
                        p2 = j*0.01;
                        p3 = k*0.01;
                        //cout << "(p1+p2+p3) " << (p1+p2+p3) << " and max_proba " << max_proba << endl;
                        if((p1+p2+p3)==(max_proba/100))
                        {
                            departure = abs( ((p1 * dg1 *conso_per_nm) + (p2 * dg2 *conso_per_nm) + (p3 * dg3 *conso_per_nm)) -  new_conso );
                            if(departure <min_departure) min_departure=departure;
                            dout(cout  << departure << endl);
                        }
                        // early stop
                        if(departure <threshold_to_break) break;
                    }
                    if(departure <threshold_to_break) break;
                }
                if(departure <threshold_to_break) break;
            }
        }

        // then fill in with the new probas for these 3 grounds.
        vector <double>  new_freq_per_fgrounds=freq_grds;

        if(departure <threshold_to_break)
        {
            new_freq_per_fgrounds.at(id1)=p1;
            new_freq_per_fgrounds.at(id2)=p2;
            new_freq_per_fgrounds.at(id3)=p3;
            dout(cout  << "after: p1 is " <<p1 << " p2 is " << p2 << " p3 is " << p3 << endl);
        }
        else
        {
            dout(cout  << "fuel saving impossible for this occurrence! min_departure is " << min_departure << endl);
        }

        // then rescale to sum to 1 because of the rounding approx.
        double a_sum=0;
        for(unsigned int ii = 0; ii < new_freq_per_fgrounds.size(); ii++)
        {
            a_sum+= new_freq_per_fgrounds.at(ii);
        }

        for(unsigned int ii = 0; ii < new_freq_per_fgrounds.size(); ii++)
        {
            new_freq_per_fgrounds.at(ii)= (new_freq_per_fgrounds.at(ii)) / a_sum;
            // dout(cout << " " << new_freq_per_fgrounds.at(ii) );

        }
        // dout(cout << endl);

        // update the prior for the next time (iterative bayesian process)
        // ...output
        this-> set_spe_freq_fgrounds(new_freq_per_fgrounds);

    }

}


void Vessel::alloc_on_closer_grounds(int tstep, int use_static_paths, vector<Node*>&nodes, const vector <types::NodeId>& relevant_nodes,
                                     const std::vector<PathShop> &pathshops,
                                     ofstream& freq_distance)
{
    // this is implicitly minimizing the fuel cost i.e. redirect the
    // choice toward grounds that will be reached with less fuel use...
    // but actually the net effect can be an increase of the fuel use if  more time needed
    // to catch the same amount that ends a trip (when catch rate is lower for those closer grounds...)
    // so, again, the 'end of trip' trigger events is a crucial assumption......
    // simulation will then poorly mimick the reality if most of the vessels
    // have fixed duration of trips whatever the catches they perform....
    // unfortunately this should be asked via the questionnaire,
    // but could also be investigated by analysing patterns of trip duration vessel by vessel...

    // in the same tracks, one could imagine to compute the expected catches and then the revenue and gav
    // even before going visiting the ground (assuming the the vessel is ommiscient)....and base the choice of
    // the ground on this.

    // input...
    vector <double> freq_grds = this->get_freq_fgrounds();

    auto from = this->get_loc()->get_idx_node();
    auto the_grounds = this->get_fgrounds();

    vector <double> distance_fgrounds;
    if(use_static_paths)
    {
         distance_fgrounds= compute_distance_fgrounds(relevant_nodes, pathshops,
                                                                      from, the_grounds);

    }
    else
    {
        // cartesian approx. actually...
        distance_fgrounds= compute_distance_fgrounds_on_the_fly(nodes, from, the_grounds);
    }


    // we could have computed here as well the fuel to be used for reaching each ground...

    vector <double> freq_distance_fgrounds= scale_a_vector_to_1(distance_fgrounds);

    // caution: at this stage we have then the higher proba for the farther ground so need to inverse
    // the proba to get more chance for closer ground...
    for(unsigned int it = 0; it < freq_distance_fgrounds.size(); it++)
    {
        freq_distance_fgrounds.at(it)= 1 - freq_distance_fgrounds.at(it);
    }

    double a_sum3 =0;
    double a_sum4 =0;
    int idx_v = this->get_idx();
    freq_distance << setprecision(3) << fixed;
    freq_distance << tstep << " " << idx_v << " ";
    for(unsigned int ii = 0; ii < freq_distance_fgrounds.size(); ii++)
    {
        dout(cout   << "observed frequency:" << freq_distance_fgrounds.at(ii) << " " << endl);
        a_sum3+= (freq_distance_fgrounds.at(ii)*freq_grds.at(ii));
        freq_distance << freq_distance_fgrounds.at(ii) << " " ;
    }

    freq_distance << 100 << " " ;// arbitrary column limit

    for(unsigned int ii = 0; ii < freq_grds.size(); ii++)
    {
        dout(cout   << "prior frequency:" << freq_grds.at(ii) << " " << endl);
        freq_distance << freq_grds.at(ii) << " " ;
    }

    freq_distance << 100 << " " ;// arbitrary column limit

    for(unsigned int ii = 0; ii < freq_distance_fgrounds.size(); ii++)
    {
        // update freq with the bayes'formula P(H1/D)=P(D/H1)*P(H1) / (P(D/H1)*P(H1)  +  P(D/H2)*P(H2) + ....)
        freq_distance_fgrounds.at(ii)= (freq_distance_fgrounds.at(ii)*freq_grds.at(ii)) / a_sum3;
        dout(cout   << "updated frequency:" << freq_distance_fgrounds.at(ii) << " " << endl);
        a_sum4+=freq_distance_fgrounds.at(ii);
        freq_distance << freq_distance_fgrounds.at(ii) << " " ;
    }
    dout(cout   << "a_sum4:" << a_sum4 << " " << endl);
    freq_distance << endl;

    // update the prior for the next time (iterative bayesian process)
    // ...output
    this-> set_spe_freq_fgrounds(freq_distance_fgrounds);

}



void Vessel::which_metier_should_i_go_for(vector <Metier*>& metiers){

    // caution: the likely metier is deduced from the proba of fishing grounds
    // we might imagine to deduce it from market incentive, etc. instead

    //1. draw a ground according to freq
    const auto &grds = this->get_fgrounds();
    vector <double> freq_grds = this->get_freq_fgrounds();
    // need to convert in array, see myRutils.cpp

    // check
    assert(grds.size() > 0);

    if(grds.size()==0)
    {
        cout << "DEBUG !! " << this->get_name() << " is likely missing vessel in fgrounds.dat " << endl;
    }


    // check
    //cout << "which_metier_should_i_go_for says possible grds for " << this->get_name() << " are " << endl;

    //for (int gr=0; gr<grds.size(); ++gr) cout << grds.at(gr) << '\t';
    //cout << endl;

    // check
    //cout << "which_metier_should_i_go_for says possible freq_grds for " << this->get_name() << " are " << endl;
    //for (int gr=0; gr<freq_grds.size(); ++gr) cout << freq_grds.at(gr) << '\t';
    //cout << endl;

    //cout << "do_sample 2 " << freq_grds.size() << " " << grds.size() << " " << this->get_name() << endl;
    auto grounds = do_sample(1, grds.size(), grds, freq_grds);
    auto ground=grounds[0];
    //cout << "end do_sample 2" << endl;

    // check
    //cout << "which_metier_should_i_go_for says current looked ground for " << this->get_name() << " is " << ground << endl;

    //2. get possible metiers on this ground
    const auto &poss_met        = this->get_possible_metiers();
    const auto &freq_poss_met= this->get_freq_possible_metiers();
    auto metiers_on_grd      = find_entries( poss_met, ground );
    auto freq_metiers_on_grd = find_entries( freq_poss_met, ground );
    // need to convert in array, see myRutils.cpp

    // check
    //cout << "which_metier_should_i_go_for says possible metiers for " << this->get_name() << " are " << endl;
    //for ( std::multimap< types::NodeId, int, std::less< int > >::const_iterator iter =poss_met.begin();
    //      iter != poss_met.end(); ++iter )
    //      cout << iter->first << '\t' << iter->second << '\n';
    //cout << endl;

    // check
    //cout << "which_metier_should_i_go_for says freq possible metiers for " << this->get_name() << " are " << endl;
    //for ( std::multimap< types::NodeId, double, std::less< int > >::const_iterator iter =freq_poss_met.begin();
    //      iter != freq_poss_met.end(); ++iter )
    //      cout << iter->first << '\t' << iter->second << '\n';
    //cout << endl;

    // check
    //cout << "which_metier_should_i_go_for says possible metiers_on_grd for " << this->get_name() << " are " << endl;
    //for (int gr=0; gr<metiers_on_grd.size(); ++gr) cout << metiers_on_grd.at(gr) << '\t';
    //cout << endl;

    // check
    //cout << "which_metier_should_i_go_for says possible freq_metiers_on_grd for " << this->get_name() << " are " << endl;
    //for (int gr=0; gr<freq_metiers_on_grd.size(); ++gr) cout << freq_metiers_on_grd.at(gr) << '\t';
    //cout << endl;

    if(metiers_on_grd.size()!=0)
    {
        vector<int>    a_met = do_sample(1, metiers_on_grd.size(), metiers_on_grd, freq_metiers_on_grd);
        dout(cout << "a_met from do_sample is " << a_met.at(0) << " and size of metiers is " << metiers.size() << endl);
        this->set_metier(  metiers.at(a_met.at(0))  );
    }
    else
    {

        cout << "no metier found on that ground " << ground << " for vessel " << this->get_name() << "...apply a dangerous fix! (but please check input data)" << endl;
        this->set_metier(  metiers[ 0 ]  ); // dangerous fix
    }

    //cout << this->get_name() << ", which_metier_should_i_go_for says metier " << this->get_metier()->get_name() << endl;


}



bool Vessel::choose_a_ground_and_go_fishing(int tstep, const displace::commons::Scenario &scenario, bool use_the_tree,
                                            const DynAllocOptions& dyn_alloc_sce,
                                            int use_static_paths,
                                            const vector<PathShop> &pathshops,
                                            adjacency_map_t& adjacency_map,
                                            vector<types::NodeId> &relevant_nodes,
                                            multimap<int, int>& nodes_in_polygons,
                                            vector<Node* >& nodes,
                                            vector <Metier*>& metiers,
                                            ofstream& freq_cpue,
                                            ofstream& freq_profit,
                                            ofstream& freq_distance
                                            )
{

    outc(cout << " choose a ground and go fishing " << endl);

    this->set_tstep_dep(tstep);	 // store departure date

    // choose a fishing ground
    const auto &grds = this->get_fgrounds();

    // choose the ground, dyn sce. vs. baseline
    types::NodeId ground = types::special::InvalidNodeId;


    if(use_the_tree && dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::ChooseGround)){

        outc(cout << " should i choose this ground" << endl);
        ground=this->should_i_choose_this_ground(tstep,
                                                 use_static_paths,
                                                 nodes,
                                                 relevant_nodes,
                                                 pathshops,
                                                 dyn_alloc_sce
                                                 ); // use ChooseGround dtree along all possible grounds to define the next ground

        if(ground==types::special::InvalidNodeId)
        {
            dout(cout << "Bad probabilities defined in the ChooseGround dtree...need a revision, unless all grounds are actually closed for this vessel" << endl);
            //cout << "do_nothing i.e. stay on quayside for ..." << this->get_name() << endl;
            return (1); // do_nothing i.e. stay on quayside
        }
    } else{

        // ************focus_on_high_previous_cpue********************//
        if (dyn_alloc_sce.option(Options::focus_on_high_previous_cpue))
        {

            outc(cout << " alloc on high previous cpue" << endl);
            this->alloc_on_high_previous_cpue(tstep,
                                              freq_cpue);
        }

        // ************focus_on_high_profit_grounds********************//
        if (dyn_alloc_sce.option(Options::focus_on_high_profit_grounds))
        {

            outc(cout << " alloc on high profit grounds" << endl);
            this->alloc_on_high_profit_grounds(tstep,
                                               use_static_paths,
                                               nodes,
                                               relevant_nodes,
                                               pathshops,
                                               freq_profit);
        }

        // ********realloc among the 3 more used grounds to save 20%*******//
        if (dyn_alloc_sce.option(Options::fuelprice_plus20percent))
        {

            // update freq only if starting from a port different from the last one.
            auto from = this->get_loc()->get_idx_node();
            if(from!=this->get_previous_harbour_idx())
            {

                outc(cout << " alloc while saving fuel" << endl);
                this->alloc_while_saving_fuel(tstep,
                                              use_static_paths,
                                              nodes,
                                              relevant_nodes,
                                              pathshops);
            }
            else
            {
                dout(cout  << "not looking for more saving here..." << endl);
            }

        }


        // ****************closer_grounds**********************************//
        if (dyn_alloc_sce.option(Options::closer_grounds))		 // dyn sce.
        {
            outc(cout << " alloc on closer grounds" << endl);
            this->alloc_on_closer_grounds(tstep,
                                          use_static_paths,
                                          nodes,
                                          relevant_nodes,
                                          pathshops,
                                          freq_distance);
        }

        // ****************area_closure**********************************//
        if (dyn_alloc_sce.option(Options::area_closure)  || scenario.closure_opts.isMetierBanned(this->get_metier()->get_name()))
        {
            //this->alter_freq_fgrounds_for_nodes_in_polygons(nodes_in_polygons);
            // compliance => 0.0001
            // replaced by:
            outc(cout << " alloc accounting for area closures" << endl);
            const auto &grds = this->get_fgrounds();
            for (int i=0; i<grds.size();++i)
            {
                auto a_grd = grds.at(i);
                if (nodes.at(a_grd.toIndex())->isMetierBanned(this->get_metier()->get_name()))
                {
                    set_spe_freq_fground(i, 1e-8);
                }
            }
        }

        if (dyn_alloc_sce.option(Options::area_monthly_closure))
        {
            const auto &grds = this->get_fgrounds();
            outc(cout << " alloc accounting for monthly area closures" << endl);

            vector<double> init_freq = this->get_freq_fgrounds_init();

            int met_idx = this->get_metier()->get_name();

            outc(cout << "for "<< this->get_name() << ", metier " << met_idx << endl);

            double nbDaysSpent=0.0;
            double nbOpenedDays=0.0;
            nbDaysSpent = this->getDaysSpentInRestrictedAreaThisMonth(met_idx);
            outc(cout << "for "<< this->get_name() << ", metier " << met_idx << " nbDaysSpent: " << nbDaysSpent << endl);
            for (int i=0; i<grds.size();++i)
            {
                auto a_grd = grds.at(i);
                if (nodes.at(a_grd.toIndex())->isMetierBanned(met_idx) &&
                        nodes.at(a_grd.toIndex())->isVsizeBanned(this->get_length_class())
                        )

                {
                    nbOpenedDays = (31- nodes.at(a_grd.toIndex())->getNbOfDaysClosed(met_idx));

                    dout(cout << this->get_name() << " nbDaysSpent: " << nbDaysSpent << "; nbOpenedDays here: " << nbOpenedDays << endl);

                  if(nbDaysSpent >= nbOpenedDays)
                    {
                      //cout << this->get_name() << " CANNOT FISH HERE! " << endl;
                      set_spe_freq_fground(i, 1e-8);
                    }
                  else
                    {
                      set_spe_freq_fground(i, init_freq.at(i));
                    }
                }

                // check for myfish graph1
                //if(this->get_name()=="DNK000038349")
                // {
                //    cout << " isMetierBanned   "  << nodes.at(a_grd)->isMetierBanned(this->get_metier()->get_name()) << endl;
                //    cout << " isVsizeBanned   " << nodes.at(a_grd)->isVsizeBanned(this->get_length_class()) << endl;
                // }

            }
           //cout << " alloc accounting for monthly area closures.....ok" << endl;

        }


        // then, draw a ground from the frequencies (altered or not)...
        vector <double> freq_grds = this->get_freq_fgrounds();
        // need to convert in array, see myRutils.cpp

        // ...unless all probas at 0 (because all grounds are closed)
        double sum_probas=0.0;
        for (int i=0; i<freq_grds.size(); ++i) sum_probas+=freq_grds.at(i);
        if(sum_probas<1e-5)
        {
            //if(this->get_name()=="DNK000038349") cout << "all the grounds are closed for this vessel " << this->get_name() << endl;
            return(1); // do_nothing
        }


        //cout << "do_sample 3" << endl;
        auto grounds = do_sample(1, grds.size(), grds, freq_grds);
        ground= types::NodeId(grounds[0]);

    }


    // count nb of days spent in restricted areas
    if (dyn_alloc_sce.option(Options::area_monthly_closure))
    {
        int met_idx = this->get_metier()->get_name();
        if (nodes.at(ground.toIndex())->isMetierBanned(met_idx) &&
                nodes.at(ground.toIndex())->isVsizeBanned(this->get_length_class()) )
        {
        this->addADayPortionToDaysSpentInRestrictedAreaThisMonth(met_idx, 24/24);
        //cout << "ADDING 1 DAY HERE" << endl;
        //cout << this->get_name() << " SO HERE nbDaysSpent: " << this->getDaysSpentInRestrictedAreaThisMonth(met_idx) << endl;
        }
    }


    //random_shuffle(grds.begin(),grds.end()); // random permutation i.e. equal frequency of occurence
    //int ground=grds[0];
    outc(cout  << this->get_name() << " GO FISHING ON " << ground.toIndex() << endl);

    /*if(ground.toIndex()==12132) {
        cout  << this->get_name() << " GO FISHING ON " << ground.toIndex() << endl;
        cout << "nodes.at(ground.toIndex())->isMetierBanned(this->get_metier()->get_name()) is "<< nodes.at(ground.toIndex())->isMetierBanned(this->get_metier()->get_name()) << endl;
        cout << "nodes.at(ground.toIndex())->isVsizeBanned(this->get_length_class()) is "<< nodes.at(ground.toIndex())->isVsizeBanned(this->get_length_class()) << endl;
        cout << "nbOpenedDays on this ground this met is " <<  (31- nodes.at(ground.toIndex())->getNbOfDaysClosed(this->get_metier()->get_name())) << endl;
        const auto &somefreqgrds = this->get_freq_fgrounds();
        for (int i=0; i<somefreqgrds.size();++i)
        {
            cout << somefreqgrds.at(i) << " ";
        }
        cout << endl;
    }
    */

    // get the shortest path between source and destination
    // with the list of intermediate nodes
    auto from = this->get_loc()->get_idx_node();
    this->set_previous_harbour_idx(from);

    PathShop curr_path_shop;


    list<types::NodeId> path;

    if(!use_static_paths)
    {
        // use ASTAR on the fly
        aStarMutex.lock();
         path = aStarPathFinder.findShortestPath(geoGraph, from.toIndex(), ground.toIndex());
        aStarMutex.unlock();
        //cout << from.toIndex() << " test the a-star...ok" <<endl;

        /*
         * if(path.size()<=1)
        { // a edge case we should avoid!
            cout << this->get_name() <<" on "<< from.toIndex() << "...the a-star returning an empty path after having chosen a ground?" <<endl;
            list<types::NodeId>::iterator road_iter = path.begin();
            dout(cout << "path: ");
            for( ; road_iter != path.end(); road_iter++)
            {
               dout(cout << *road_iter << " " );
            }
            dout(cout << endl);
        }
        */

    }
    else
    {
        // instead, read into the static paths
        std::vector<types::NodeId>::iterator it = find (relevant_nodes.begin(), relevant_nodes.end(), from);
        int idx;
        if (it != relevant_nodes.end())
        {
           idx = it - relevant_nodes.begin();
           curr_path_shop = pathshops.at(idx);
        }
        else
        {
            cout << from.toIndex() << " not found in the relevant nodes!! find a path on the fly and add to the pathshops" <<endl;
            relevant_nodes.push_back(from);
            spp::sparse_hash_map <vertex_t, vertex_t> previous;
            spp::sparse_hash_map <vertex_t, weight_t> min_distance;
            DijkstraComputePaths(from.toIndex(), adjacency_map, min_distance, previous, relevant_nodes);
            cout << from.toIndex() << " add to the pathshops...ok" <<endl;
        }

        outc(cout  << "find path to fishing ground " << ground.toIndex() <<endl);
        outc(cout  << "from the current_path_shop of node " << relevant_nodes.at(idx).toIndex() <<endl);
        outc(cout  << "starting from the node " << from.toIndex() <<endl);

        path = DijkstraGetShortestPathTo(ground, curr_path_shop);
    }





    if(path.size()>1)			 // i.e no path has been found if path.size()==1...
    {
        path.pop_front();		 // delete the first node (departure) because we are lying in...
        this->set_roadmap(path);

       /* // show the roadmap
        list<types::NodeId> road= this->get_roadmap();

        // check path
        list<types::NodeId>::iterator road_iter = road.begin();
            dout(cout << "path: ");
            for( ; road_iter != road.end(); road_iter++)
            {
               dout(cout << *road_iter << " " );
            }
            dout(cout << endl);
        */

        // then, call to find.next.pt.on.the.graph()
        this-> find_next_point_on_the_graph_unlocked(nodes);

        // decide on the rest duration for the next time (drawn from a gamma law)
        double calib=1;

        double a_shape = this-> get_resttime_par1();
        double a_scale = this-> get_resttime_par2()  *calib;
        dout(cout  << "TIME FOR REST WHEN WE WILL BE AT PORT AFTER OUR TRIP"  << endl);

        this-> set_timeforrest( rgamma(a_shape, a_scale) );

        // set inactive
        this-> set_inactive(false);
        // set vessel nationality
        this-> set_natio(true);
        // for this vessel, select the metier specific to this particular fishing ground
        // according to the observed frequency in data
        // (NOTE THAT, FINALLY, THE METIER CAN BE DIFFERENT FROM THE ONE FROM which_metier_should_i_go_for() )
        const auto &poss_met        = this->get_possible_metiers();
        const auto &freq_poss_met= this->get_freq_possible_metiers();
        auto metiers_on_grd      = find_entries( poss_met, ground );
        auto freq_metiers_on_grd = find_entries( freq_poss_met, ground );
        // need to convert in array, see myRutils.cpp
        if(metiers_on_grd.size()!=0)
        {
            //cout << "do_sample 4" << endl;
            vector<int>    a_met = do_sample(1, metiers_on_grd.size(), metiers_on_grd, freq_metiers_on_grd);
            this->set_metier(  metiers[ a_met.at(0) ]  );
        }
        else
        {
            this->set_metier(  metiers[ 0 ]  ); // dangerous fix
        }
        //random_shuffle(metiers_on_grd.begin(),metiers_on_grd.end()); // random permutation i.e. equal frequency of occurence
        //this->set_metier(  metiers[ metiers_on_grd[0] ]  );

        // make a pause and check...
        // dout(cout <<  a_met[0] << " name: " << this->get_metier()->get_name() << endl);
        // int a;
        //dout(cout  << "Pause: type a number to continue");
        //cin >> a;

    }
    else
    {
        outc(cout << "pble calculating from " << from << " to " << ground << " in choose_a_ground_and_go_fishing()"<< endl);
        this->move_to(nodes.at(from.toIndex())) ;
        // no path found: assume the vessel stucks at its current location
    }

    return(0);
}


int Vessel::choose_another_ground_and_go_fishing(int tstep,
                                                  const DynAllocOptions &dyn_alloc_sce,
                                                  int use_static_paths,
                                                  const std::vector<PathShop> &pathshops,
                                                  adjacency_map_t& adjacency_map,
                                                  vector <types::NodeId>& relevant_nodes,
                                                  const multimap<int, int>& nodes_in_polygons,
                                                  vector<Node* > &nodes,
                                                  vector <Metier* > &metiers,
                                                  ofstream& freq_cpue,
                                                  ofstream& freq_distance
                                                  )
{
    UNUSED(tstep);
    UNUSED(freq_cpue);
    UNUSED(freq_distance);

    lock();

    bool finally_I_should_go_for_the_closest=false;
    // => in case of area_closure: provoke oscillation at the border if the 2nd closest is inside a closed area

    const auto &grds = this->get_fgrounds();
    //int current_grd = this->get_loc()->get_idx_node();
    //  if(grds.size()>2)
    //  {
    // get the shortest distance between source and destination
    // with the list of intermediate nodes
    vector <double> dist_to_others;
    auto from = this->get_loc()->get_idx_node();
    dout(cout  << "current node: " << from.toIndex() << endl);
    //min_distance.clear();
    //previous.clear();
    PathShop curr_path_shop;

    vector <double> distance_fgrounds;
    if(use_static_paths)
    {
        auto it = find (relevant_nodes.begin(), relevant_nodes.end(), from);
        int idx = it - relevant_nodes.begin();
        curr_path_shop = pathshops.at(idx);

         distance_fgrounds= compute_distance_fgrounds(relevant_nodes, pathshops,
                                                                      from, grds);

    }
    else
    {
        // cartesian approx. actually...
        distance_fgrounds= compute_distance_fgrounds_on_the_fly(nodes, from, grds);
    }


    for (unsigned int i =0; i< grds.size(); i++)
    {

        types::NodeId vx = types::NodeId(grds.at(i));// destination
        //dout(cout  << "test the other ground "<< vx.toIndex() << endl);


        // check for area_closure
        if (
                (dyn_alloc_sce.option(Options::area_closure) && nodes.at(from.toIndex())->isMetierBanned(this->get_metier()->get_name()))
                || (dyn_alloc_sce.option(Options::area_monthly_closure)  && nodes.at(from.toIndex())->isMetierBanned(this->get_metier()->get_name()) &&
                    nodes.at(from.toIndex())->isVsizeBanned(this->get_length_class()))
                )
        {

            //	if(binary_search (polygon_nodes.begin(), polygon_nodes.end(), from.toIndex()))
            // if(nodes.at(from)->evaluateAreaType()==1) // area closed?
            if (nodes.at(from.toIndex())->isMetierBanned(this->get_metier()->get_name()))
                  {
                  int idx_met= this->get_metier()->get_name();
                  this->addADayPortionToDaysSpentInRestrictedAreaThisMonth(idx_met, 1/24); // because ping_rate is one hour
                  //cout << "ADDING 1/24 DAY HERE" << endl;

                  if(this->getDaysSpentInRestrictedAreaThisMonth(idx_met) >=
                      (31- nodes.at(from.toIndex())->getNbOfDaysClosed(idx_met)))
                     {


                      dout(cout  << "gosh... I am fishing in a closed area there! " << from.toIndex() <<  endl);
                      double dist_to_this_node = dist( nodes.at(from.toIndex())->get_x(),
                                                 nodes.at(from.toIndex())->get_y(),
                                                 nodes.at(vx.toIndex())->get_x(),
                                                 nodes.at(vx.toIndex())->get_y()
                                                 );

                //	if(!binary_search (polygon_nodes.begin(), polygon_nodes.end(), vx)

                //if(nodes.at(vx)->evaluateAreaType()!=1
                if (!nodes.at(vx.toIndex())->isMetierBanned(this->get_metier()->get_name())
                        // looking around in a radius of 200 km among the grounds I know...
                        &&  dist_to_this_node < 200 )
                {
                    dout(cout  << "this node " << vx.toIndex() << " is actually outside the closed area: steam away!!!" << endl);
                    // force to steam away by assigning a very low distance
                    dist_to_others.push_back(1);
                }
                else
                {
                    dout(cout  << "this other ground is also part of the closed area!!!" << endl);
                    dist_to_others.push_back(950);

                }

             }
             else
             {
                    dist_to_others.push_back(distance_fgrounds[i]);

             }
            }
            else
            {

                dist_to_others.push_back(distance_fgrounds[i]);

            }

        }
        else
        {
            // this time, check for closure for the potential dest vx
            if(
                    (dyn_alloc_sce.option(Options::area_closure) && nodes.at(vx.toIndex())->isMetierBanned(this->get_metier()->get_name()))
                    || (dyn_alloc_sce.option(Options::area_monthly_closure)  && nodes.at(vx.toIndex())->isMetierBanned(this->get_metier()->get_name()) &&
                        nodes.at(vx.toIndex())->isVsizeBanned(this->get_length_class()))
              )
            {
               dout(cout  << "this other ground is also part of the closed area!!!" << endl);
               dist_to_others.push_back(950);
            }
            else
            {
               dist_to_others.push_back(distance_fgrounds[i]);
            }
        }

    }
    types::NodeId next_ground;
    double lowest =10000;		 // init
    int idx_lowest=0;
    for(unsigned int i=0; i<dist_to_others.size(); i++)
    {
        if(dist_to_others[i]>1 && dist_to_others[i]<lowest )
        {
            lowest =dist_to_others[i];
            idx_lowest =i;
        }
    }
    vector <bool> a_vect;
    a_vect.push_back(true);
    a_vect.push_back(false);
    random_shuffle(a_vect.begin(),a_vect.end());

    if(a_vect[0])
    {
        dist_to_others[idx_lowest]= 10000;
        double scdlowest =10000; // init
        int idx_scdlowest=0;
        for(unsigned int i=0; i<dist_to_others.size(); i++)
        {
            if(dist_to_others[i]>1 && dist_to_others[i]<scdlowest )
            {
                scdlowest =dist_to_others[i];
                idx_scdlowest =i;
            }
        }
        next_ground = types::NodeId(grds[idx_scdlowest]);
        dout(cout  << "GO FISHING ON THE 2nd CLOSEST: " << next_ground.toIndex() << endl);

        // check for area_closure
        if (
                (dyn_alloc_sce.option(Options::area_closure) && !nodes.at(next_ground.toIndex())->isMetierBanned(this->get_metier()->get_name())) ||
                (dyn_alloc_sce.option(Options::area_monthly_closure)  && !nodes.at(from.toIndex())->isMetierBanned(this->get_metier()->get_name()) &&
                 !nodes.at(from.toIndex())->isVsizeBanned(this->get_length_class()))
                )
        {

            //if(binary_search (polygon_nodes.begin(), polygon_nodes.end(), next_ground))
            //if(nodes.at(next_ground)->evaluateAreaType()!=1)
            if (nodes.at(next_ground.toIndex())->isMetierBanned(this->get_metier()->get_name()))
            {
                dout(cout  << "this NEXT node " << next_ground.toIndex() << " is actually within a closed area!!!" << endl);
                finally_I_should_go_for_the_closest = true;
                // the closest might also be inside the closed area but at least we will oscillate back to outside

            }
        }

    }

    if(!a_vect[0] || finally_I_should_go_for_the_closest)
    {
        next_ground = types::NodeId(grds[idx_lowest]);

        dout(cout  << "GO FISHING ON THE CLOSEST: " <<   next_ground.toIndex() << endl);
    }

    if (from.toIndex() == next_ground.toIndex() ||
            nodes.at(next_ground.toIndex())->isMetierBanned(this->get_metier()->get_name()))
    {
        dout(cout  << "WHAT? I CANNOT CHANGE FOR " <<   next_ground.toIndex() << " SO I STAY WHERE I AM... " << endl);
       /* if(next_ground.toIndex()==12132){
            cout  << "WHAT? I CANNOT CHANGE FOR " <<   next_ground.toIndex() << " SO I STAY WHERE I AM... " << endl;
            cout << "nodes.at(next_ground.toIndex())->isMetierBanned(this->get_metier()->get_name()) is "<< nodes.at(next_ground.toIndex())->isMetierBanned(this->get_metier()->get_name()) << endl;
            cout << "nodes.at(next_ground.toIndex())->isVsizeBanned(this->get_length_class()) is "<< nodes.at(next_ground.toIndex())->isVsizeBanned(this->get_length_class()) << endl;
         }
       */
        unlock();
        return 1; // actually, there is not found possibility for a change....
    }


    // in this case, get a time and fuel bonus for free! (i.e. note the MINUS sign!)
    // (in order to somehow correct for the discretisation creating jumps between sequential fgrounds)
    this->set_timeatsea(this->get_timeatsea() - PING_RATE);
    this->set_traveled_dist_this_trip(this->get_traveled_dist_this_trip() + this->get_speed() * PING_RATE * NAUTIC);
    double cumfuelcons = this->get_cumfuelcons() - this->get_fuelcons()*PING_RATE;
    this->set_cumfuelcons(cumfuelcons);
    this->set_consotogetthere( this->get_consotogetthere() - (this->get_fuelcons()*PING_RATE) ) ;


    list<types::NodeId> path;
    if(!use_static_paths)
    {
        // Use ASTAR on the fly
        aStarMutex.lock();
         path = aStarPathFinder.findShortestPath(geoGraph, from.toIndex(), next_ground.toIndex());
        aStarMutex.unlock();
        //cout << from.toIndex() << " test the a-star for antoher ground...ok" <<endl;

        /*
         * if(path.size()<=1)
        { // a edge case we should avoid!
            cout << this->get_name() <<" on "<< from.toIndex() << "...the a-star returning an empty path after having decided to change ground?" <<endl;
            list<types::NodeId>::iterator road_iter = path.begin();
            dout(cout << "path: ");
            for( ; road_iter != path.end(); road_iter++)
            {
               dout(cout << *road_iter << " " );
            }
            dout(cout << endl);
        }
        */

    }
    else
    {
        // instead, read static paths
        path = DijkstraGetShortestPathTo(next_ground, curr_path_shop);

    }

    if(!path.empty()) path.pop_front();			 // delete the first node (departure) because we are lying in...

    if(path.size()==0)
    {
        dout(cout << this->get_name() << " when changing from "<< from.toIndex() << " to this new ground: " << next_ground.toIndex() << " you should stop here because my path is empty!");
        // as we detected something wrong here, we try to recover(!):
        for(unsigned int i=0; i<grds.size(); i++)
        {
            next_ground =  types::NodeId(grds[i]);
            dout(cout << "then try to change for this new ground: " << next_ground.toIndex() << endl);

            if(!use_static_paths)
            {
                // Use ASTAR on the fly
                aStarMutex.lock();
                 path = aStarPathFinder.findShortestPath(geoGraph, from.toIndex(), next_ground.toIndex());
                aStarMutex.unlock();
                //cout << from.toIndex() << " test the a-star for antoher ground...ok" <<endl;
            }
            else
            {
                // instead, read static paths
                path = DijkstraGetShortestPathTo(next_ground, curr_path_shop);

            }

            if(!path.empty()) path.pop_front();	 // delete the first node (departure) because we are lying in...
            if(path.size()>1)
            {
                dout(cout << "this one is all right." << endl);
                break;
            }
        }
    }

    if(path.size()==0)
    {
        // still empty!!
        outc(cout << "pble calculating from " << from.toIndex() << " to " << next_ground.toIndex() << endl);
        this->move_to(nodes.at(from.toIndex())) ;
        // no path found: assume the vessel stucks at its current location
    } else{
        this->set_roadmap(path);
    }


    dout(cout  << "WELL...GO FISHING ON " << next_ground.toIndex() << endl);
    /*if(next_ground.toIndex()==12132){
        cout  << "WELL...GO FISHING ON " << next_ground.toIndex() << endl;
        cout  << "We change from "<< from.toIndex() << " to this new ground: " << next_ground.toIndex() << endl;
        cout << "nodes.at(next_ground.toIndex())->isMetierBanned(this->get_metier()->get_name()) is "<< nodes.at(next_ground.toIndex())->isMetierBanned(this->get_metier()->get_name()) << endl;
        cout << "nodes.at(next_ground.toIndex())->isVsizeBanned(this->get_length_class()) is "<< nodes.at(next_ground.toIndex())->isVsizeBanned(this->get_length_class()) << endl;
     }
     */

     /*if(path.size()==0) {
         cout << "Path is empty!!!" << endl;
     }
     else
     {
         cout << "Path is not empty! "<< path.size() << endl;
     }
     list<types::NodeId>::iterator road_iter = path.begin();
     dout(cout << "path: ");
     for( ; road_iter != path.end(); road_iter++)
     {
        dout(cout << *road_iter << " " );
     }
     dout(cout << endl);
    */


    dout(cout  << "We change from "<< from.toIndex() << " to this new ground: " << next_ground.toIndex() << endl);

    // for this vessel, select the metier specific to this particular fishing ground
    // according to the observed frequency in data
    const auto &poss_met        = this->get_possible_metiers();
    const auto &freq_poss_met= this->get_freq_possible_metiers();
    auto metiers_on_grd      = find_entries( poss_met, next_ground );
    auto freq_metiers_on_grd = find_entries( freq_poss_met, next_ground );
    // need to convert in array, see myRutils.cpp

    if(metiers_on_grd.size()!=0)
    {
        //cout << "do_sample 5" << endl;
        vector<int>    a_met = do_sample(1, metiers_on_grd.size(), metiers_on_grd, freq_metiers_on_grd);
        this->set_metier(  metiers[ a_met[0] ]  );
    }
    else
    {
        this->set_metier(  metiers[ 0 ]  );   // a dangerous fix
    }
    // find.next.pt.on.the.graph()
    this->find_next_point_on_the_graph_unlocked(nodes);
    unlock();
return 0;
}


//------------------------------------------------------------//
//------------------------------------------------------------//
// related to individual choice of harbours...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::choose_a_port_and_then_return(int tstep,
                                           const DynAllocOptions &dyn_alloc_sce,
                                           int use_static_paths,
                                           const std::vector<PathShop> &pathshops,
                                           adjacency_map_t& adjacency_map,
                                           vector <types::NodeId>& relevant_nodes,
                                           vector<Node* >& nodes,
                                           vector <Metier*>& metiers,
                                           ofstream& freq_cpue,
                                           ofstream& freq_distance,
                                           vector <double>& dist_to_ports
                                           )
{
    UNUSED(tstep);
    UNUSED(freq_cpue);
    UNUSED(freq_distance);
    UNUSED(metiers);

    auto harbs = this->get_harbours();
    auto from = this->get_loc()->get_idx_node();
    //dist_to_ports.clear();
    //min_distance.clear();
    //previous.clear();
    PathShop curr_path_shop;
    // get the shortest path between source and destination
    // with the list of intermediate nodes

    if(use_static_paths)
    {     
        auto it = find (relevant_nodes.begin(), relevant_nodes.end(), from);
        int idx = it - relevant_nodes.begin();
        curr_path_shop = pathshops.at(idx);
    }



    vector <double> distance_to_harb;
    if(use_static_paths)
    {
         distance_to_harb= compute_distance_fgrounds(relevant_nodes, pathshops,
                                                                      from, harbs);

    }
    else
    {
        // cartesian approx. actually...
        distance_to_harb= compute_distance_fgrounds_on_the_fly(nodes, from, harbs);
    }



    // the 'arr' node will either be the closest port, or the more frequent port:
    types::NodeId arr;

    //*************************closer_port**************************//
    if (dyn_alloc_sce.option(Options::closer_port))			 // dyn sce.
    {
        for (unsigned int i =0; i< harbs.size(); i++)
        {
            // destination
            types::NodeId vx = types::NodeId(harbs[i]);
            dout(cout  << "distance to harbour " << vx.toIndex() << ": " << distance_to_harb[vx.toIndex()] << endl);
            dist_to_ports.push_back(distance_to_harb[vx.toIndex()]);
        }
        // init
        double lowest =dist_to_ports[0];
        int idx_lowest=0;
        for(unsigned int i=0; i<dist_to_ports.size(); i++)
        {
            if(dist_to_ports[i]<lowest)
            {
                lowest =dist_to_ports[i];
                idx_lowest =i;
            }
        }
        arr = types::NodeId(harbs[idx_lowest]); // destination: nearest port
    }
    else						 // ....otherwise (e.g. baseline), choose a harbour according to its frequency in data...
    {
        vector <double> freq_harbs = this->get_freq_harbours();
        // need to convert in array, see myRutils.cpp
        //cout << "do_sample 6" << endl;
        auto harbours = do_sample(1, harbs.size(), harbs, freq_harbs);
        arr = types::NodeId(harbours[0]);
    }

    dout(cout  << "from " << from.toIndex() << endl);
    dout(cout  << "choose " << arr.toIndex() << endl);

    list<types::NodeId> path;
    if(!use_static_paths)
    {
        // Use ASTAR on the fly
        aStarMutex.lock();
         path = aStarPathFinder.findShortestPath(geoGraph, from.toIndex(), arr.toIndex());
        aStarMutex.unlock();
        //cout << from.toIndex() << " test the a-star for antoher ground...ok" <<endl;
    }
    else
    {
        // instead, read static paths
        path = DijkstraGetShortestPathTo(arr, curr_path_shop);

    }


    // check the path
    // list<int>::iterator pos;
    //for(pos=path.begin(); pos!=path.end(); pos++)
    //{
    //    dout(cout << (*pos).toIndex() << " ");
    //}
    //cout << endl;

    if(path.size()==1)			 // i.e no path has been found...
    {
        // fix the fact that, in very few cases, certain paths are not found in a given direction (likely to come from imcomplete adjacency matrix)
        // so tips: use the inversed path instead! i.e. 'from' begin 'arr' while 'arr' begin 'from'
        // and finally the path is inverted.



        list<types::NodeId> path;
        if(!use_static_paths)
        {
            // Use ASTAR on the fly
            aStarMutex.lock();
             path = aStarPathFinder.findShortestPath(geoGraph, from.toIndex(), arr.toIndex());
            aStarMutex.unlock();
            //cout << from.toIndex() << " test the a-star for this situation...ok" <<endl;
        }
        else
        {
            // instead, read static paths
            auto it = find (relevant_nodes.begin(), relevant_nodes.end(), arr);
            int idx = it - relevant_nodes.begin();
            curr_path_shop = pathshops.at(idx);

            path = DijkstraGetShortestPathTo(arr, curr_path_shop);

        }


        reverse(path.begin(), path.end());

        // check
        //list<int>::iterator pos;
        //for(pos=path.begin(); pos!=path.end(); pos++)
        //{c
        //    dout(cout << *pos << " ");
        //}
        //cout << endl;
        // CAUTION => THIS IS ONLY A FIX!
    }
    bool print_out=false;
    if(path.size()==1)
    {
        // still no path!....
        dout (cout << "still no path found in shop for vessel " << this->get_name() <<
              " going to arr " << arr.toIndex() << " from " << from.toIndex() << " then compute it..." << endl);


        this->move_to(nodes.at(from.toIndex())) ;
        // no path found: assume the vessel stucks at its current location


    }

    if(!path.empty()) path.pop_front();			 // delete the first node (departure) because we are lying in...
    // (NOTE: if the 'arr' node is not in the 'previous' object of 'from',
    // then the path is only consistited of 'arr' and then will be blank here because removed by pop_front!!!! => bug)
    this->set_roadmap(path);
    if(print_out)
    {
        dout(cout << "new roadmap to port is: ");
        // check
        list<types::NodeId> lst = this->get_roadmap();
        for(auto pos=lst.begin(); pos!=lst.end(); pos++)
        {
            dout(cout << (*pos).toIndex() << " ");
        }
        dout(cout << endl);
    }

    // update
    this->set_timeatsea(this->get_timeatsea()+ PING_RATE);
    this->set_traveled_dist_this_trip(this->get_traveled_dist_this_trip() + this->get_speed() * PING_RATE * NAUTIC);

}


//------------------------------------------------------------//
//------------------------------------------------------------//
// reinit...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::reinit_after_a_trip()
{
    outc(cout << "reinit after a trip..." << endl);
    // clear for this vessel
    this-> clear_catch_pop_at_szgroup();
    this-> clear_discards_pop_at_szgroup();
    this-> set_inactive(true);
    // re-init some other stuffs after the trip
    this-> set_cumfuelcons(0);
    this-> set_sweptareathistrip(0);
    this-> set_subsurfacesweptareathistrip(0);
    this-> set_consotogetthere(0);
    this-> set_cumsteaming(0);
    this-> set_cumcatches(0);
    this-> set_cumdiscards(0);
    this-> clear_idx_used_metiers_this_trip();
    this-> compute_experiencedcpue_fgrounds();
    this-> compute_experiencedcpue_fgrounds_per_pop();
    this-> clear_cumcatch_and_cumeffort();
    this-> set_distprevpos(0);
    this-> set_state(3);
    this-> set_timeatsea(0);
    this-> set_traveled_dist_this_trip(0);
    this-> set_natio(true);
    this->set_tstep_dep(0);
    this->set_reason_to_go_back(0);
    outc(cout << "reinit after a trip...OK" << endl);
}


//------------------------------------------------------------//
//------------------------------------------------------------//
// export...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::export_loglike_prop_met(ofstream& loglike_prop_met, int tstep, int nbpops)
{
    UNUSED(nbpops);

    dout(cout  << "write down complementary infos...");
    loglike_prop_met << setprecision(2) << fixed;
    //date dep / date arr /  vessel / used metiers frequency
    loglike_prop_met << this->get_tstep_dep() << " " << tstep << " "
                     << this->get_name() << " ";
    vector<int> vec = this->get_idx_used_metiers_this_trip();
    typedef map<int, unsigned> counts_t;

    // Create the histogram
    counts_t histogram;
    for (vector<int>::const_iterator i = vec.begin(); i != vec.end(); i++)
    {
        ++histogram[*i];
    }
    // ... and display it.
    loglike_prop_met << "X" ;
    for (counts_t::const_iterator i = histogram.begin(); i != histogram.end(); i++)
    {
        loglike_prop_met << "_" ;
        double freq = static_cast<double>(i->second) / vec.size();
        loglike_prop_met  << i->first << ":" << freq ;

    }
    loglike_prop_met << " " << endl;

}


//------------------------------------------------------------//
//------------------------------------------------------------//
// individual decision, including reading of decision trees...
//------------------------------------------------------------//
//------------------------------------------------------------//

int Vessel::should_i_go_fishing(int tstep, std::vector<Population* >& populations,
                                bool use_the_tree, const DynAllocOptions& dyn_alloc_sce, vector<int>& implicit_pops,
                                int is_individual_vessel_quotas, int check_all_stocks_before_going_fishing)
{

    lock();

    // first of all, check if some remaining quotas (either on explicit stocks only or on ALL stocks)
    // (if the check is done on all the stocks then it is likely that some discards will occur because
    // vessel are continuing going fishing until no one quota is remaining...on the contrary, if only explicit stocks
    // that are checked then these stocks will act as "choke" species and the vessels will stay on quayside as soon as
    // the quotas are exhausted on explicit stocks.)
    // note that, by default, only pops with informed global_tac_this_pop have non-zero individual_tac
    // which means that the vessel will stop going fishing if all the pops with informed tac have their tac exhausted
    // (while they might actually target other species?? caution!)

    // NOTE THAT IDEALLY check_all_stocks_before_going_fishing SHOULD BE 1 IF ALL POPS ARE IMPLICIT!
    // OTHERWISE MOST OF THE VESSELS WILL STAY ON QUAYSIDE....

    dout(cout << "is_individual_vessel_quotas is" <<is_individual_vessel_quotas << endl);
    dout(cout << "check_all_stocks_before_going_fishing is" <<check_all_stocks_before_going_fishing << endl);

    int still_some_quotas;
    if(is_individual_vessel_quotas)
    {
        still_some_quotas=0; // init
        int nbpops = this->get_percent_tac_per_pop().size();
        int nbimplicitpops = implicit_pops.size();
        for (int pop=0; pop < nbpops; pop++)
        {
            // check
            if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  pop  )  // check explicit pops...
                    || check_all_stocks_before_going_fishing                         // ...and other pops if specified...
                    || nbimplicitpops==nbpops)                                 // ...and force check all pops if all pops are implicit (see config.dat)
            {
                // return the annual individual quota (!= the quota left), should be 0 if erased because no quota left
                double indiv_quota= this->get_individual_tac(pop);
                if(indiv_quota!=0)
                {
                    still_some_quotas=1;
                    dout(cout  << "this vessel " << this->get_name() << " have (still) quota for pop " << pop << ": " << indiv_quota << endl);
                     // => by default, continue if not all stks quotas are exhausted.....
                }
                if(dyn_alloc_sce.option(Options::stopGoingFishingOnFirstChokedStock))
                {
                    vector<int>  trgts =this->get_metier()->get_metier_target_stocks();
                    for(unsigned int tg=0; tg<trgts.size(); ++tg)
                        {
                            if(pop==trgts.at(tg) && indiv_quota==0)  still_some_quotas=0;
                             // => will stay on quayside because exhausted tac on at least one targeted stock
                            this->set_is_choked(pop, 1);
                        }

                }
            }
        }


        // to avoid the vessel staying quayside while the vessel is actually not targeting the tac-informed species
        // we need a small fix:
        if(this->get_targeting_non_tac_pop_only())
            still_some_quotas=1;

        dout(cout << "this->get_targeting_non_tac_pop_only() is" <<this->get_targeting_non_tac_pop_only() << endl);

    }
    else
    {
         still_some_quotas=1;
        if(dyn_alloc_sce.option(Options::stopGoingFishingOnFirstChokedStock))
         {
             for (int pop=0; pop < populations.size(); pop++)
             {
              if (populations.at(pop)->get_tac()->get_is_tac_exhausted())  still_some_quotas=0;
             }
         }
         else
         {
            if(dyn_alloc_sce.option(Options::fishing_credits))
            {
                tout(cout << "What this vessel " << this->get_name() <<" has for remaining credits ? " << this->get_fishing_credits().at(0) << endl);
                if(this->get_fishing_credits().at(0) <=0) still_some_quotas=0; // here, quota means credits
            }
            else
            {
               still_some_quotas=1; // init
            }
         }
   }

    dout(cout << "still_some_quotas is" <<still_some_quotas << endl);


    if( still_some_quotas)
    {

        // PBLE HERE FOR THE USE OF THE DECISION TREE: WHICH TIMING SHOULD WE USE?
        // take the decision to go once a day and at 7 a.m.

        // read the vessel-specific decision tree
        // (binary tree only, no/low/bad is 0 and yes/high/good is 1)
        if(use_the_tree && dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::GoFishing))
        {

            if((tstep % 24)==this->getWorkDayStartHour())
            {


                std::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::GoFishing);
                double the_value = traverseDtree(tstep, tree.get());
                //cout <<"the value returned by traverseDtree is "<< the_value << endl;

                // draw a random number [0,1) and compare with the value

                if(unif_rand()<the_value) {
                    unlock();     // GO!
                    return(1);
                } else {
                    unlock();
                    return(0);	  // DON'T GO!
                }

            }
            else
            {
                unlock();
                return(0);		  // DON'T DECIDE NOW!
            }


        }
        else
        {

            // DEFAULT-------------------------
            // by default, if rest time is over...GO!
            // (assumption: note that this rest time, which is drawn from a gamma distrib,
            //    do not change whatever the scenario, which can in some cases be weird given potentials for temporal effort reallocation...)
            if(this->get_timeforrest()<1) {
                unlock();
                return(1);
            } else {
                unlock();
                return(0);
            }
        }

    }
    else
    {
        dout(cout  << "no quota or credit left for this vessel " << this->get_name() << "...stay on quayside!" << endl);
        unlock();
        return(0);
    }

}






types::NodeId Vessel::should_i_choose_this_ground(int tstep,
                                                  int use_static_paths,
                                                  vector<Node *> &nodes,
                                                  const vector<types::NodeId> &relevant_nodes,
                                                  const std::vector<PathShop> &pathshops,
                                                  const DynAllocOptions& dyn_alloc_sce
                                                )
{
    lock();


    std::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::ChooseGround);


//  TO DO: MAKE THE ORDER OF RELEVANT NODES (smartCatch notThatFar etc.) FLEXIBLE!!!



    int idx=0; // idx of the relevant ground

    // keep tracks relevant nodes to evaluate
    vector <types::NodeId> relevant_grounds_to_evaluate;
    types::NodeId smartCatchGround = types::special::InvalidNodeId;
    types::NodeId highPotentialCatchGround = types::special::InvalidNodeId;
    types::NodeId notThatFarGround = types::special::InvalidNodeId;
    types::NodeId lowestTariffGround = types::special::InvalidNodeId;
    types::NodeId knowledgeOfThisGround = types::special::InvalidNodeId;

    // 1. grounds of that vessel
    auto grds= this->get_fgrounds();
    vector <double> freq_grds = this->get_freq_fgrounds();

    vector<double>  freq_grds_in_closure(0);
    if(dyn_alloc_sce.option(Options::area_monthly_closure))
    {
        // fill in in list of closed fgrounds
        vector<types::NodeId>  grds_in_closure(0);
        for (int i=0; i<grds.size();++i)
        {
            if (nodes.at(grds.at(i).toIndex())->isMetierBanned(this->get_metier()->get_name()) &&
                    nodes.at(grds.at(i).toIndex())->isVsizeBanned(this->get_length_class())
                    )
            {
                freq_grds_in_closure.push_back(freq_grds.at(i));
                grds_in_closure.push_back(grds.at(i));
            }

            // check for myfish graph1
            /*if(this->get_name()=="DNK000038349")
               {
                  if(grds_in_closure.size()>0)
                  {
                      cout << " with ChooseGround tree, isMetierBanned   "  << nodes.at(grds.at(i))->isMetierBanned(this->get_metier()->get_name()) << endl;
                      cout << " with ChooseGround tree,  isVsizeBanned   " << nodes.at(grds.at(i))->isVsizeBanned(this->get_length_class()) << endl;
                      cout << " so the list of fgrounds in closure is updated to : " << endl;
                      for(int j=0; j <grds_in_closure.size(); ++j)
                      {
                         cout << " " << grds_in_closure.at(j);
                      }
                      cout << endl;
                  }

              }
              */

        }
        this->set_fgrounds_in_closed_areas(grds_in_closure);
    }



    // 1. test for closure and keep a node within if any
    auto grds_in_closure = this->get_fgrounds_in_closed_areas();  // for the isInAreaClosure tree evaluation
    if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::isInAreaClosure) == true &&
            grds_in_closure.size()>0)
    {
        auto theground = do_sample(1, grds_in_closure.size(), grds_in_closure, freq_grds_in_closure);
        types::NodeId a_random_ground_inside_closed_area= types::NodeId(theground[0]);
        relevant_grounds_to_evaluate.push_back(a_random_ground_inside_closed_area); // the first tested ground
        // for the isInAreaClosure tree evaluation, knowing that isInAreaClosure should be the first tree node
    }


    // 2. search in the tree for the other relevant grounds
    // (the trick is to avoid screening the ChooseGround tree with all the grounds randomly)
    // check
    /*
           cout << this->get_name() << " has ground in closure ? " << endl;
          for (unsigned int i=0; i<grds_in_closure.size();++i)
             {
             cout << grds_in_closure.at(i) << " ";
             }
          cout << endl;
         */




    if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::smartCatch) == true)
    {
        outc(cout << "compute smartCatchGround"  << endl);

        vector<double> expected_profit_per_ground = this->expected_profit_on_grounds(use_static_paths,
                                                                                     nodes,
                                                                                     relevant_nodes,
                                                                                     pathshops);

        // a check

        // (c++14)
        //if ( all_of(expected_profit_per_ground.begin(), expected_profit_per_ground.end(), [](int i){return i<0;} ) )
        //   {

        bool positive_found = false;
        for(unsigned int i=0; i<expected_profit_per_ground.size();++i)
        {
            if(expected_profit_per_ground.at(i) > 0)
            {
                positive_found = true;
                break;
            }
        }
        if (!positive_found)
        {

            // all negative expected revenue: a TRIGGER EVENT for the vessel to start exploring other horizons...
            cout << this->get_name() << ": NO PROFIT EXPECTED ON ALL GROUNDS FROM TARGET SPECIES!" << endl;
            // => Then, imagine a mean to expand the range of these vessels....

            // e.g. look at what use to do some vessels sharing the same departure harbour!
            auto a_node         = this->get_loc()->get_idx_node(); // cause the decision is taken in harbour...
            int current_metier = this->get_metier()->get_name();
            int nbpops         = nodes.at(a_node.toIndex())->get_nbpops();
            const auto &grounds_from_harbours        = nodes.at(a_node.toIndex())->get_usual_fgrounds();
            const vector <double>         &freq_grounds_from_harbours   = nodes.at(a_node.toIndex())->get_freq_usual_fgrounds();
            vector<vector<double> > experiencedcpue_fgrounds_per_pop (grounds_from_harbours.size(), vector<double>(nbpops));
            multimap  <types::NodeId,int>     possible_metiers_from_harbours;     // = nodes.at(a_node)->get_usual_metiers();
            multimap  <types::NodeId,double>  freq_possible_metiers_from_harbours; //= nodes.at(a_node)->get_freq_usual_metiers();
            for(unsigned int gr=0; gr<grounds_from_harbours.size();++gr)
            { // rebuild assuming deploying one metier spread over the entire range from this harbour...
                possible_metiers_from_harbours.insert(std::make_pair(grounds_from_harbours.at(gr),current_metier));
                freq_possible_metiers_from_harbours.insert(std::make_pair(grounds_from_harbours.at(gr), 1.0));
            }

            if(grounds_from_harbours.at(0)!=a_node)
            {
                //cout << this->get_name() << "check size for do_sample() " << grounds_from_harbours.size() << " " << freq_grounds_from_harbours.size() << endl;
                this->set_spe_fgrounds(grounds_from_harbours); // CHANGED
                this->set_spe_freq_fgrounds(freq_grounds_from_harbours); // CHANGED
                this->set_experienced_bycatch_prop_on_fgrounds(freq_grounds_from_harbours);// re-dimensioned
                this->set_experienced_avoided_stks_bycatch_prop_on_fgrounds(freq_grounds_from_harbours);// re-dimensioned
                this->set_cumcatch_fgrounds(freq_grounds_from_harbours);// re-dimensioned
                this->set_cumdiscard_fgrounds(freq_grounds_from_harbours);// re-dimensioned
                this->set_cumcatch_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop);// re-dimensioned
                this->set_cumdiscard_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop);// re-dimensioned
                this->set_cumeffort_fgrounds(freq_grounds_from_harbours);// re-dimensioned
                this->set_experiencedcpue_fgrounds(freq_grounds_from_harbours); // re-dimensioned
                this->set_experiencedcpue_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop); // re-dimensioned
                this->set_freq_experiencedcpue_fgrounds(freq_grounds_from_harbours); // re-dimensioned
                this->set_freq_experiencedcpue_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop); // re-dimensioned
                this->set_fgrounds_in_closed_areas(vector <types::NodeId> ()); // TO DO
                this->set_spe_possible_metiers(possible_metiers_from_harbours); // CREATED
                this->set_spe_freq_possible_metiers(freq_possible_metiers_from_harbours); // CREATED

            }
            else
            {
                cout << this->get_name() << " says: no info from the harbour...hopeless! " << endl;
            }
            cout << this->get_name() << "...look at knowledge from " << nodes.at(a_node.toIndex())->get_name() << endl;
            // then, assume:
            expected_profit_per_ground = freq_grounds_from_harbours;

            // ...and caution, need for redefining grds.
            grds= this->get_fgrounds();
            freq_grds= this->get_freq_fgrounds();
            grds_in_closure = this->get_fgrounds_in_closed_areas();

        }




        // keep only the grds out the closed areas...
        vector <types::NodeId> grds_out;
        vector <double> expected_profit_per_ground_out;
        if(grds_in_closure.size()>0)
        {
            for (unsigned int i=0; i<grds.size();++i)
            {
                auto it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
                if(it == grds_in_closure.end()) // not found in closure....so keep it
                {
                    expected_profit_per_ground_out.push_back(expected_profit_per_ground.at(i));
                    grds_out.push_back(grds.at(i));
                }
            }
        } else{
            grds_out=grds;
            expected_profit_per_ground_out=expected_profit_per_ground;
        }
        if(grds_out.size()>0){

            // ...and find the max
            idx = distance(expected_profit_per_ground_out.begin(),
                           max_element(expected_profit_per_ground_out.begin(), expected_profit_per_ground_out.end()));
            smartCatchGround = grds_out.at(idx);
            this->set_smartcatch(smartCatchGround);

            relevant_grounds_to_evaluate.push_back(smartCatchGround); // use it to limit the search time...

        } else{
            this->set_smartcatch(types::special::InvalidNodeId);  // grounds are all included in closed areas...
        }
        outc(cout << "smartCatchGround is " << smartCatchGround.toIndex() << endl);
    }





    if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::highPotentialCatch) == true)
    {
        outc(cout << "compute highPotentialCatchGround"  << endl);

        vector <vector<double> > past_freq_cpue_grds_pops = this-> get_freq_experiencedcpue_fgrounds_per_pop(); // (experiencedcpue is computed after each trip)
        vector <double> past_freq_cpue_grds (grds.size());

        vector<int>  trgts =this->get_metier()->get_metier_target_stocks();

        // only look at the targeted stocks
        //cout << "there are xx grounds: .." << grds.size() << endl;
        for(unsigned int gr=0; gr<grds.size(); ++gr)
        {
            //cout << "gr is: .." << gr << endl;
            for(unsigned int i=0; i<trgts.size(); ++i)
            {
                int a_trgt=trgts.at(i);
                //cout << "pop target is: .." << a_trgt << endl;
                past_freq_cpue_grds.at(gr)+= past_freq_cpue_grds_pops.at(gr).at(a_trgt);
            }
        }


        // keep only the grds out the closed areas...
        vector <types::NodeId> grds_out2;
        vector <double> past_freq_cpue_grds_out;
        grds_in_closure = this->get_fgrounds_in_closed_areas();
        if(grds_in_closure.size()>0)
        {
            for (unsigned int i=0; i<grds.size();++i)
            {
                auto it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
                if(it == grds_in_closure.end()) // not found
                {
                    past_freq_cpue_grds_out.push_back(past_freq_cpue_grds.at(i));
                    grds_out2.push_back(grds.at(i));
                }
            }
        } else{
            grds_out2=grds;
            past_freq_cpue_grds_out=past_freq_cpue_grds;
        }
        if(grds_out2.size()>0){

            // ...and find the max
            idx = distance(past_freq_cpue_grds_out.begin(),
                           max_element(past_freq_cpue_grds_out.begin(), past_freq_cpue_grds_out.end()));
            highPotentialCatchGround = grds_out2.at(idx);
            this->set_highpotentialcatch(highPotentialCatchGround);

            relevant_grounds_to_evaluate.push_back(highPotentialCatchGround); // use it to limit the search time...

        } else{
            this->set_highpotentialcatch(types::special::InvalidNodeId);  // grounds are all included in closed areas...
        }
        outc(cout << "highPotentialCatchGround is " << highPotentialCatchGround.toIndex() << endl);

    }

    if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::knowledgeOfThisGround) == true)
    {
        outc(cout << "compute knowledgeOfThisGround" << endl);
        vector <double> freq_grounds = this->get_freq_fgrounds();

        // keep only the grds out the closed areas...
        vector <types::NodeId> grds_out4;
        vector <double> freq_grounds_out;
        grds_in_closure = this->get_fgrounds_in_closed_areas();
        if(grds_in_closure.size()>0)
        {
            for (unsigned int i=0; i<grds.size();++i)
            {
                auto it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
                if(it == grds_in_closure.end()) // not found
                {
                    freq_grounds_out.push_back(freq_grounds.at(i));
                    grds_out4.push_back(grds.at(i));
                }
            }
        } else{
            grds_out4=grds;
            freq_grounds_out=freq_grounds;
        }
        if(grds_out4.size()>0){

            // ...and find the max
            idx = distance(freq_grounds_out.begin(),
                           max_element(freq_grounds_out.begin(), freq_grounds_out.end()));
            knowledgeOfThisGround = grds_out4.at(idx);
            this->set_mosthistoricallyused(knowledgeOfThisGround);

            relevant_grounds_to_evaluate.push_back(knowledgeOfThisGround); // use it to limit the search time...

        } else{
            this->set_mosthistoricallyused(types::special::InvalidNodeId);  // grounds are all included in closed areas...
        }
        outc(cout << "knowledgeOfThisGround is " << knowledgeOfThisGround.toIndex() << endl);

    }

    if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::notThatFar) == true)
    {
        outc(cout << "compute notThatFarGround"  << endl);

        auto from = this->get_loc()->get_idx_node();


        vector <double> distance_to_grounds;
        if(use_static_paths)
        {
             distance_to_grounds= compute_distance_fgrounds(relevant_nodes, pathshops,
                                                                          from, grds);

        }
        else
        {
            // cartesian approx. actually...
            distance_to_grounds= compute_distance_fgrounds_on_the_fly(nodes, from, grds);
        }


        // keep only the grds out the closed areas...
        vector <types::NodeId> grds_out3;
        vector <double> distance_to_grounds_out;
        grds_in_closure = this->get_fgrounds_in_closed_areas();
        if(grds_in_closure.size()>0)
        {
            for (unsigned int i=0; i<grds.size();++i)
            {
                auto it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
                if(it == grds_in_closure.end()) // not found
                {
                    distance_to_grounds_out.push_back(distance_to_grounds.at(i));
                    grds_out3.push_back(grds.at(i));
                }
            }
        } else{
            grds_out3=grds;
            distance_to_grounds_out=distance_to_grounds;
        }
        if(grds_out3.size()>0){

            // ...and find the max
            idx = distance(distance_to_grounds_out.begin(),
                           max_element(distance_to_grounds_out.begin(), distance_to_grounds_out.end()));
            notThatFarGround = grds_out3.at(idx);
            this->set_notthatfar(notThatFarGround);

            relevant_grounds_to_evaluate.push_back(notThatFarGround); // use it to limit the search time...

        } else{
            this->set_notthatfar(types::special::InvalidNodeId);  // grounds are all included in closed areas...
        }
        outc(cout << "notThatFarGround is " << notThatFarGround.toIndex() << endl);
    }


    if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::lowestTariff) == true)
    {
        outc(cout << "compute lowestTariff"  << endl);


        vector <double> tariff_per_ground;
        for (unsigned int i=0; i<grds.size();++i)
        {
           tariff_per_ground.push_back(nodes.at(grds.at(i).toIndex())->get_tariffs().at(0));
        }

        // keep only the grds out the closed areas...
        vector <types::NodeId> grds_out4;
        vector <double> tariff_per_ground_out;
        grds_in_closure = this->get_fgrounds_in_closed_areas();
        if(grds_in_closure.size()>0)
        {
            for (unsigned int i=0; i<grds.size();++i)
            {
                auto it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
                if(it == grds_in_closure.end()) // not found
                {
                    grds_out4.push_back(grds.at(i));
                    tariff_per_ground_out.push_back(tariff_per_ground.at(i));
                }
            }
        } else{
            grds_out4=grds;
            tariff_per_ground_out=tariff_per_ground;
        }
        if(grds_out4.size()>0){

            // ...and find the min
            idx = distance(tariff_per_ground_out.begin(),
                           min_element(tariff_per_ground_out.begin(), tariff_per_ground_out.end()));
            lowestTariffGround = grds_out4.at(idx);
            this->set_lowesttariff(lowestTariffGround);

            relevant_grounds_to_evaluate.push_back(lowestTariffGround); // use it to limit the search time...

        } else{
            this->set_lowesttariff(types::special::InvalidNodeId);  // grounds are all included in closed areas...
        }
        outc(cout << "lowestTariffGround is " << lowestTariffGround.toIndex() << endl);
    }







    // 3. traverseDTree for each possible relevant grounds
    types::NodeId ground= types::special::InvalidNodeId;
    //random_shuffle(grds.begin(),grds.end()); // random permutation i.e. equal frequency of occurence
    for (size_t it=0; it < relevant_grounds_to_evaluate.size(); ++it){
        ground=relevant_grounds_to_evaluate.at(it);
        outc(cout << "Evaluate for ground... "<< ground.toIndex() << endl);

        // Caution with a logical leaks: each ground is evaluated once and only once,
        // meanwhile the smartCatch ground can be likely the same than the highPotentialCatch, notThatFar, etc.
        // so most of the time the tree need to be COMPLETE or FULL to account for conditional probabilities...

        // evaluators should evaluate if yes/no the ground a smartCatch ground etc.:
        // e.g.

        //"smartCatch",          // ChooseGround             => find if that ground is where the highest expected profit is
        //"highPotentialCatch",          // ChooseGround     => find if that ground is where the highest experienced CPUE (all species) occurred
        //"notThatFar",          // ChooseGround             => find if that ground is the closest one
        //"knowledgeOfThisGround",          // ChooseGround  => look at the historic proba of visiting the grounds and pick up the most frequented ground
        //"riskOfBycatchAvoidedStksThisGroundIs"  // ChooseGround related to experienced_avoided_stks_bycatch_prop_on_fgrounds
        //"riskOfBycatchAllStksThisGroundIs"    // ChooseGround related to experienced_bycatch_prop_on_fgrounds
        //"individualQuotaLeftOnAvoidedStksIs"    // ChooseGround
        //"globalQuotaLeftOnAvoidedStksIs"    // ChooseGround
        //"saveFuel"                 // ChooseGround         => TO DO: find the highest expected profit among the XX closests
        //"isInAreaClosure"      // ChooseGround             => find if that ground is lying inside the closed polygons
        //"lowestTariff"   // ChooseGround               => relevant only if fishing_credits Option is active
        // avoidHighTariffAreas  // ChooseGround               => relevant only if fishing_credits Option is active
        //=> TO DO: add the corresponding dtree evaluators...

        // cout << "traverse tree for ground " << ground << endl;
        double the_value = traverseDtree(ground.toIndex(), tree.get());


        //CHOOSE THAT GROUND!
        if(unif_rand()<the_value) {
            unlock();
            //cout << "END1 should_i_choose_this_ground for ..." << this->get_name() << endl;
            return(ground);
        }
        //  else // CONTINUE SEARCHING AMONG RELEVANT GROUNDS
    }


    // if here, then no ground has actually been found within
    // smartCatch or highPotentialCatch or knowledgeOfThisGround or notThatFar.....
    // so we will rely on freq_grds to choose the ground.
    // i.e. MIXED APPROACH

    // (because of 1- a *non-complete* tree;
    // or 2- the node present into two or more relevant nodes at the mean time e.g smartCatch is also notThatFar)
    // or (e.g. 3- when all nodes are in closed areas)
    dout(cout << "no one among relevant grounds for " << this->get_name() << " last ground evaluated was... "<< ground.toIndex() << endl);

    //for the last node....caution. Check if rand>last_value if yes then go to freq_fgrounds use...otherwise do nothing

    double last_value = traverseDtree(0, tree.get()); // traverse up to the last value, assuming node 0 is no meaning for the vessel...
    // if 1 found in the very last leaf of the tree then we´ll go for sure for sampling into freq_grds..
    // if less than 1 then it is tested to know if the vessel do nothing or if the vessel will proceed further.


    if(unif_rand()>last_value || (relevant_grounds_to_evaluate.size()>0 && ground==types::special::InvalidNodeId)){
         unlock();
         //cout << "END2 should_i_choose_this_ground for ..." << this->get_name() << endl;
        return (types::special::InvalidNodeId); // do_nothing, likely because all grounds in closed areas and last leaf at 0
    }



    // but first we need to check for special cases e.g. reaction to potential for bycatch:
    if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::riskOfBycatchAllStksThisGroundIs) == true)
    {
       freq_grds= this->get_experienced_bycatch_prop_on_fgrounds();
       freq_grds= scale_a_vector_to_1(freq_grds);
       for(unsigned int it = 0; it < freq_grds.size(); it++)
       {
           freq_grds.at(it)= 1 - freq_grds.at(it);
       }

    }
    if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::riskOfBycatchAvoidedStksThisGroundIs) == true)
    {
       freq_grds= this->get_experienced_avoided_stks_bycatch_prop_on_fgrounds();
       freq_grds= scale_a_vector_to_1(freq_grds);
       for(unsigned int it = 0; it < freq_grds.size(); it++)
       {
           freq_grds.at(it)= 1 - freq_grds.at(it);
       }
   }

    if(dyn_alloc_sce.option(Options::fishing_credits))
    {
      if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::avoidHighTariffAreas) == true)
      {
        freq_grds= this->get_freq_fgrounds();
        for(int gr=0; gr < grds.size(); ++gr)
        {
            freq_grds.at(gr)= freq_grds.at(gr) * nodes.at(grds.at(gr).toIndex())->get_tariffs().at(0); // probas weigthed by the tariffs
        }
        freq_grds= scale_a_vector_to_1(freq_grds);
      }
    }



    // need to convert in array, see myRutils.cpp
    double cumul=0.0;
    //cout << "grds.size() is" << grds.size() << endl;
    //cout << "freq_grds.size() is" << freq_grds.size() << endl;
    grds_in_closure = this->get_fgrounds_in_closed_areas();
    for(unsigned int n=0; n<grds.size(); n++)
    {
        if (binary_search (grds_in_closure.begin(), grds_in_closure.end(), grds.at(n)))
        {
           // cout << " allo " << endl;
            freq_grds.at(n)=1e-8; // to avoid removing if nb of grounds outside is 0
            // but potential non-compliance if all grounds are in the closed areas....
            // therefore put 0.0 in the last leaf if this is not the wished behaviour...
        }
        cumul += freq_grds.at(n);
    }

    // then re-scale to 1
    for(unsigned int n=0; n<grds.size(); n++)
    {
        freq_grds.at(n)= freq_grds.at(n)/cumul;
    }

    // then sample...
    dout(cout << "Possible crash here if grounds.size() " << grds.size() << " is different from freq_grds.size() " << freq_grds.size() << endl);
    auto grounds = do_sample(1, grds.size(), grds, freq_grds); // caution: will return empty vector if something wrong in input....then make a crash
    ground= types::NodeId(grounds[0]);

    //cout << "ground is " << ground.toIndex() << endl;

    unlock();
    // cout << "END3 should_i_choose_this_ground for ..." << this->get_name() << endl;
    return(ground);

}



int Vessel::should_i_change_ground(map<string,int>& external_states, bool use_the_tree)
{
    UNUSED(external_states);

    bool shall_I_change_to_another_ground_because_of_StartFishing_dtree = false;
    bool shall_I_change_to_another_ground_because_of_ChangeGround_dtree = false;
    bool shall_I_change_to_another_ground = false;

    // StartFishing
    if(use_the_tree && dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::StartFishing))
    {

        lock();

        std::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::StartFishing);

        auto from = this->get_loc()->get_idx_node();
        dout(cout  << "current node: " << from.toIndex() << endl);

        double the_value = traverseDtree(from.toIndex(), tree.get());

       //SHALL I START FISHING FROM THE CURRENT GROUND?
        if(unif_rand()<the_value) {
            shall_I_change_to_another_ground_because_of_StartFishing_dtree=false;
        }
        else
        {
            shall_I_change_to_another_ground_because_of_StartFishing_dtree=true &&
                    this->get_fgrounds().size()>2 &&
                    this->get_nbfpingspertrip() > 1 &&
                    this->get_loc()->get_code_area()!=10;

        }

        unlock();

    }

    // ChangeGround
    if(use_the_tree && dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::ChangeGround))
    {

        lock();

        std::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::ChangeGround);

        auto from = this->get_loc()->get_idx_node();
        dout(cout  << "current node: " << from.toIndex() << endl);

        double the_value = traverseDtree(from.toIndex(), tree.get());

       //SHALL I CHANGE GROUND ?
        if(unif_rand()<the_value) {

            shall_I_change_to_another_ground_because_of_ChangeGround_dtree=true &&  // yes...
                    this->get_fgrounds().size()>2 &&  //...unless...
                    this->get_nbfpingspertrip() > 1 &&
                    this->get_loc()->get_code_area()!=10;

        }
        else
        {
            shall_I_change_to_another_ground_because_of_ChangeGround_dtree=false;
        }

        unlock();

    }


    // DEFAULT-------------------------
    if(!use_the_tree || (use_the_tree && ! (dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::StartFishing)||
                          (dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::ChangeGround)))))
    {
    lock();

        vector <bool> a_vect;
        a_vect.push_back(true);
        a_vect.push_back(false);
        a_vect.push_back(false);
        a_vect.push_back(false);
        random_shuffle(a_vect.begin(),a_vect.end());
        shall_I_change_to_another_ground = a_vect[0] &&
                this->get_fgrounds().size()>2 &&
                this->get_nbfpingspertrip() > 1 &&
                this->get_loc()->get_code_area()!=10; // do not change if outside the area of interest (where the nodes are likely to be spaced by large distance!) (see R code for code 10)

     unlock();
     }


return(shall_I_change_to_another_ground_because_of_StartFishing_dtree ||
           shall_I_change_to_another_ground_because_of_ChangeGround_dtree ||
           shall_I_change_to_another_ground);

}


int Vessel::should_i_stop_fishing(const map<string,int>& external_states, bool use_the_tree,
                                  int tstep,
                                  const DynAllocOptions& dyn_alloc_sce,
                                  int use_static_paths,
                                  const std::vector<PathShop> &pathshops,
                                  adjacency_map_t& adjacency_map,
                                  const vector <types::NodeId>& relevant_nodes,
                                  vector<Node* >& nodes,
                                  vector <Metier*>& metiers,
                                  ofstream& freq_cpue,
                                  ofstream& freq_distance,
                                  vector <double>& dist_to_ports
                                  )
{
    UNUSED(external_states);
    UNUSED(tstep);
    UNUSED(dyn_alloc_sce);
    UNUSED(nodes);
    UNUSED(metiers);
    UNUSED(freq_cpue);
    UNUSED(freq_distance);

    outc (cout<< "should I  stop fishing??" << endl);

    if(use_the_tree && dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::StopFishing))
    {

        std::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::StopFishing);
        double the_value = traverseDtree(tstep, tree.get());


        //STOP FISHING!
        if(unif_rand()<the_value) {
            return(1);
        } else {
            return(0);		 // DON´T STOP!
        }

    }
    else
    {
        lock();
        // DEFAULT-------------------------

        //assumptions:
        // the trip duration is VARIABLE in most cases (i.e. not pre-fixed by the skipper/schedule of the vessel owner)
        // and then change depending on the performance in terms of used fuel (i.e. go back if running out of fuel)
        // or in terms of catch volume (i.e. go back if carrying cap is full)
        // (exception: the detection of daily trips...)
        // we then assume that the skipper choose a flexible trip duration and do not
        // go back to port for other reasons (eg opening hours for delivery)
        // (i.e. if timeatsea>20 he will NOT go back with bad catches or if still some fuel left)
        // (caution: those assumptions would need a check with questionnaires...)
        // another important assumption is the carrying capacity which is actually
        // the best observed historic catches...which create a somehow artificial ceiling for tot catches....

        double a_min_dist=0;
        dist_to_ports.clear();

        // compute a_min_dist
        // (necessary to catch the daily trips or to jauge if we need to go back because of fuel)
        if( (this->get_cumfuelcons() > (this->get_tankcapacity()/2))
                || (( this->get_av_trip_duration() < 30)
                    && (this->get_timeatsea() >=12
                        // still in the day?
                        && this->get_timeatsea() < 20)) )
        {
            const auto &harbs = this->get_harbours();
            auto from = this->get_loc()->get_idx_node();


            vector <double> dist_to_ports;
            if(use_static_paths)
            {

                 dist_to_ports= compute_distance_fgrounds(relevant_nodes, pathshops,
                                                                              from, harbs);

            }
            else
            {
                // cartesian approx. actually...
                dist_to_ports= compute_distance_fgrounds_on_the_fly(nodes, from, harbs);
            }


            vector<double>::iterator where = min_element (dist_to_ports.begin(), dist_to_ports.end());
            a_min_dist = *where;
            dout(cout  << "minimum dist to port is " << *where << endl);

        }

        bool flag1=true;
        if((this->get_av_trip_duration() < 30) &&
                this->get_timeatsea() >=12 &&
                this->get_timeatsea() < 20)
        {
            //cout << "tstep with detection at "  << tstep << endl;
            // boolean for capturing the daily trips (3 hours at full speed of the nearest port)
            //MAGIC NUMBER HERE!
            flag1 =  !(a_min_dist < this->get_speed() * NAUTIC * 3);
        }
        // boolean for knowing if we have to go back because of fuel (continue action if true)
        //('conso to get there' give only the conso for steaming....)
        //bool flag2 = this->get_tankcapacity() - this->get_cumfuelcons()
        //             > this->get_consotogetthere();
        bool flag2 = this->get_tankcapacity() - this->get_cumfuelcons()
                > ( (a_min_dist /(this->get_speed() * NAUTIC)) * this->get_fuelcons()*this->get_mult_fuelcons_when_steaming() );
        // boolean for knowing if we have to go back because of full of catches (continue action if true)
        bool flag3 = this->get_cumcatches() < this->get_carrycapacity();
        // boolean for knowing if we may change of grounds
        // boolean for knowing if we will change of grounds
        dout(cout  << "flag 1 is " <<flag1 << endl);
        dout(cout  << "flag 2 is " <<flag2 << endl);
        dout(cout  << "flag 3 is " <<flag3 << endl);

        if(!flag1)
        {
            dout(cout  << "no more time..." << endl);
            this->set_reason_to_go_back(1);
        }
        if(!flag2)
        {
            dout(cout  << "no enough fuel to go on..." << endl);
            this->set_reason_to_go_back(2);
        }
        if(!flag3)
        {
            dout(cout  << "no enough room to fish more..." << endl);
            this->set_reason_to_go_back(3);
        }

        int stop_fishing =1;
        if(flag1 && flag2 && flag3) stop_fishing =0;

        unlock();
        return(stop_fishing);
    }
}


int Vessel::should_i_choose_this_port(map<string,int>& external_states, bool use_the_tree)
{
    UNUSED(external_states);

    if(use_the_tree)
    {
        // TO DO
    }
    else
    {

        // DEFAULT-------------------------
    }

    return 0;
}
