// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <dtree/dtnode.h>
#include <dtree/decisiontreemanager.h>
#include <dtree/externalstatemanager.h>
#include <dtree/commonstateevaluators.h>
#include <dtree/evaluators/timeseriesevaluator.h>
#include <dtree/variables.h>

#include <boost/make_shared.hpp>

#include <functional>
#include <stdexcept>

std::vector<boost::shared_ptr<dtree::StateEvaluator> > Vessel::mStateEvaluators;

namespace dtree {
namespace vessels {
class AverageProfitComparationsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    AverageProfitComparationsStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
        if (vessel->getNumTrips() > 2)
            return (vessel->getLastTripProfit() < vessel->getAvgTripProfit() ? 1.0 : 0.0);
        else
            return 0.0;
    }
};

class AverageRevenuesComparationStateEvaluator : public dtree::StateEvaluator {
private:
public:
    AverageRevenuesComparationStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
        if (vessel->getNumTrips() > 2)
            return (vessel->getLastTripRevenues() < vessel->getAvgTripRevenues() ? 1.0 : 0.0);
        else
            return 0.0;
    }
};

class VesselSizeStateEvaluator : public dtree::StateEvaluator {
public:
    VesselSizeStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
      //cout << vessel->get_name() << " with length " << vessel->get_length() << " is of length class " <<  (double)vessel->get_length_class() << endl;
        return vessel->get_length_class();
    }
};

class MetierStateEvaluator : public dtree::StateEvaluator {
public:
    MetierStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
        return static_cast<float>(vessel->get_metier()->get_name()) / 29.0;
    }
};


class VesselTodayIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselTodayIsStateEvaluator() {}
    double evaluate(int tstep, Vessel *) const {
          return (tstep % 168)>72  ? 1.0 : 0.0; // 24*7=168; 3 first days is 72h //0: "week start" node; 1: "week ending"
        }
};



class VesselFuelTankStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselFuelTankStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
          return vessel->get_cumfuelcons() > (vessel->get_tankcapacity()*0.5)  ? 1.0 : 0.0; //0: "still ok" node; 1: "low"
          //  a more complicated alternative require computing (all) distance to ports:
          //  return (	vessel->get_tankcapacity() - vessel->get_cumfuelcons()
          //          < ( (a_min_dist /(vessel->get_speed() * NAUTIC)) * vessel->get_fuelcons()*vessel->get_mult_fuelcons_when_steaming() );
          //   ? 1.0 : 0.0);
        }
};


class VesselCatchVolumeStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselCatchVolumeStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
          return vessel->get_cumcatches() > (vessel->get_carrycapacity() *0.5)  ? 1.0 : 0.0; //0: "still ok" node; 1: "fullfilled"
        }
};


class VesselNbOfDaysAtSeaSoFarIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselNbOfDaysAtSeaSoFarIsStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
          return vessel->get_timeatsea() > 5*24  ? 1.0 : 0.0; //0:<5days ; 1:><5days
        }
};


class VesselEndOfTheDayIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselEndOfTheDayIsStateEvaluator() {}
    double evaluate(int tstep, Vessel *) const {
          return (tstep % 24)<18  ? 1.0 : 0.0; // return for daily trip after 6 p.m. //0: "true" node; 1: "false"
        }
};



class VesselSmartCatchStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselSmartCatchStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isSmart= (fground==v->get_smartcatch());
        //cout << "smartcatch on this ground evaluated at " <<  isSmart << endl;
        return  isSmart ? 1.0 : 0.0; // Is yes or no the tested ground a smart catch?
        }
};


class VesselHighPotentialCatchStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselHighPotentialCatchStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isHighPotential=(fground==v->get_highpotentialcatch());
        //cout << "highpotentialcatch on this ground evaluated at " << isHighPotential<< endl;
        return   isHighPotential ? 1.0 : 0.0; // Is yes or no the tested ground the highest cpue ground?
        }
};

class VesselNotThatFarStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselNotThatFarStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isNotFar = (fground==v->get_notthatfar());
        //cout << "notthatfar on this ground evaluated at " << isNotFar << endl;
        return  isNotFar ? 1.0 : 0.0; // Is yes or no the closest ground?
        }
};

class VesselKnowledgeOfThisGroundStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselKnowledgeOfThisGroundStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isWellKnown = (fground==v->get_mosthistoricallyused());
        //cout << "mosthistoricallyused on this ground evaluated at " << isWellKnown << endl;
        return  isWellKnown ? 1.0 : 0.0; // Is yes or no the ground has been the most historically frequent?
        }
};

class VesselRiskOfBycatchIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselRiskOfBycatchIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        vector <int> the_grds = v->get_fgrounds();
        int idx_node_r= find(the_grds.begin(), the_grds.end(), fground) - the_grds.begin();    // relative node index to this vessel
        //cout << "risk of bycatch on this ground being evaluated..." << endl;
        vector <double> prop_bycatch = v->get_experienced_bycatch_prop_on_fgrounds();
        //cout << "...the discard ratio for that ground is: " << prop_bycatch.at(idx_node_r) << endl;
        return  prop_bycatch.at(idx_node_r) > 0.5 ? 1.0 : 0.0; // Is yes or no the vessel has experienced large bycatch (>50%) on this ground?
        }
};


class VesselIsInAreaClosureEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselIsInAreaClosureEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
          vector <int> lst_fgrounds_in_closed_areas=v->get_fgrounds_in_closed_areas();
          std::vector<int>::iterator it= find (lst_fgrounds_in_closed_areas.begin(), lst_fgrounds_in_closed_areas.end(), fground);
          bool isIt= (it != lst_fgrounds_in_closed_areas.end()); // found
          //cout << "isinareaclosure on this ground evaluated at "  << isIt << endl;
          return  isIt ? 1.0 : 0.0; // Is yes or no tin closed area?

        }
};



}
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
    pthread_mutex_init(&mutex,0);

	m_location = p_location;
	idx_vessel = idx;
	p_location->set_vid(idx_vessel);
	x =m_location->get_x();
	y =m_location->get_y();
	course=0;
	name = a_name;
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


Vessel::Vessel(Node* p_location,  int a_idx_vessel, string a_name, int nbpops, int nbszgroups,
const vector<int> &_harbours, const vector<int> &_fgrounds, const vector<int> &_fgrounds_init,
const vector<double> &_freq_harbours, const vector<double> &_freq_fgrounds, const vector<double> &_freq_fgrounds_init,
const vector<double> &_vessel_betas_per_pop,
const vector<double> &_percent_tac_per_pop,
const multimap<int, int> &_possible_metiers, const multimap<int, double> &_freq_possible_metiers,
double a_speed, double a_fuelcons, double a_length, double a_KW,
double  a_carrycapacity, double a_tankcapacity, double a_nbfpingspertrip,
double a_resttime_par1, double a_resttime_par2, double a_av_trip_duration,
double _mult_fuelcons_when_steaming, double _mult_fuelcons_when_fishing,
double _mult_fuelcons_when_returning, double _mult_fuelcons_when_inactive)
{
    pthread_mutex_init(&mutex,0);

	m_location = p_location;
	idx_vessel = a_idx_vessel;
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
	}

	// init at 0 the matrix of catches
    dout(cout  << "init matrix of catches" << endl);
	vector< vector<double> > init_catch_pop_at_szgroup(nbpops, vector<double>(nbszgroups));
	catch_pop_at_szgroup= init_catch_pop_at_szgroup;
	for(int i = 0; i < nbpops; i++)
	{

		for(int j = 0; j < nbszgroups; j++)
		{

			catch_pop_at_szgroup[i][j] = 0;
            dout(cout  << catch_pop_at_szgroup[i][j] << " ");
		}
        dout(cout  << endl);
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


    dout(cout <<"vessel creator...OK" << endl);
    init();
}


void Vessel::init()
{
    lastTrip_revenues = lastTrip_profit = avgRevenues = avgProfit = 0;
    numTrips = 0;

    nationality = nationalityFromName(get_name());

    // Lazy initialization of the global State Evaluator.
    if (mStateEvaluators.size() == 0) {
        for (int i = 0; i < dtree::VarLast; ++i) {
            mStateEvaluators.push_back(boost::shared_ptr<dtree::StateEvaluator>());
        }

        // Add here the variables associations
        // GoFishing
        mStateEvaluators[dtree::vesselMetierIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::MetierStateEvaluator);
        mStateEvaluators[dtree::vesselSizeIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselSizeStateEvaluator);
        mStateEvaluators[dtree::lastTripRevenueIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::AverageRevenuesComparationStateEvaluator());
        mStateEvaluators[dtree::lastTripProfitIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::AverageProfitComparationsStateEvaluator());
        mStateEvaluators[dtree::vesselMetierIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::MetierStateEvaluator());
        mStateEvaluators[dtree::fuelPriceIs] = boost::shared_ptr<dtree::StateEvaluator>(new displace::dtree::TimeSeriesEvaluator<displace::simulation::TimeSeriesManager::Fuelprice>());
        mStateEvaluators[dtree::fishPriceTargetStockIs] = boost::shared_ptr<dtree::StateEvaluator>(new displace::dtree::TimeSeriesEvaluator<displace::simulation::TimeSeriesManager::Fishprice>());
        mStateEvaluators[dtree::windSpeedIs] = boost::shared_ptr<dtree::StateEvaluator>(new displace::dtree::TimeSeriesEvaluator<displace::simulation::TimeSeriesManager::WSpeed>());
        mStateEvaluators[dtree::todayIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselTodayIsStateEvaluator);

        // StopFishing
        mStateEvaluators[dtree::fuelTankIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselFuelTankStateEvaluator);
        mStateEvaluators[dtree::catchVolumeIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselCatchVolumeStateEvaluator);
        mStateEvaluators[dtree::nbOfDaysAtSeaSoFarIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselNbOfDaysAtSeaSoFarIsStateEvaluator);
        mStateEvaluators[dtree::endOfTheDayIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselEndOfTheDayIsStateEvaluator);


        // chooseGround
        mStateEvaluators[dtree::smartCatch] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselSmartCatchStateEvaluator);
        mStateEvaluators[dtree::highPotentialCatch] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselHighPotentialCatchStateEvaluator);
        mStateEvaluators[dtree::notThatFar] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselNotThatFarStateEvaluator);
        mStateEvaluators[dtree::knowledgeOfThisGround] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselKnowledgeOfThisGroundStateEvaluator);
        mStateEvaluators[dtree::riskOfBycatchIs] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselRiskOfBycatchIsStateEvaluator);
        mStateEvaluators[dtree::isInAreaClosure] =
                boost::shared_ptr<dtree::StateEvaluator> (new dtree::vessels::VesselIsInAreaClosureEvaluator);



    }

}

Vessel::Vessel(string name, Node* a_location)
: name(name), m_location(a_location)
{
    pthread_mutex_init(&mutex,0);
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


int Vessel::read_message () const
{
	return(message);
}


string Vessel::get_name () const
{
	return(name);
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


const vector<int> &Vessel::get_harbours() const
{
	return(harbours);
}


const vector<int> &Vessel::get_fgrounds() const
{
	return(fgrounds);
}

const vector<int> &Vessel::get_fgrounds_init() const
{
    return(fgrounds_init);
}


vector<int> &Vessel::get_fgrounds_in_closed_areas()
{
    return(fgrounds_in_closed_areas);
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

const vector<double> &Vessel::get_experienced_bycatch_prop_on_fgrounds () const
{
    return(experienced_bycatch_prop_on_fgrounds);
}


int Vessel::get_previous_harbour_idx () const
{
	return(previous_harbour_idx);
}


const vector<vector<double> > &Vessel::get_cumcatch_fgrounds_per_pop() const
{
	return(cumcatch_fgrounds_per_pop);
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


const multimap<int, int> &Vessel::get_possible_metiers() const
{
	return(possible_metiers);
}


const multimap<int, double> &Vessel::get_freq_possible_metiers() const
{
	return(freq_possible_metiers);
}


const list<vertex_t> &Vessel::get_roadmap() const
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


double Vessel::get_cumsteaming () const
{
	return(cumsteaming);
}


const vector<vector<double> > &Vessel::get_catch_pop_at_szgroup() const
{
	return(catch_pop_at_szgroup);
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


int Vessel::get_targeting_non_tac_pop_only () const
{
	return(targeting_non_tac_pop_only);
}

int Vessel::get_smartcatch () const
{
    return(smartcatch);
}

int Vessel::get_highpotentialcatch () const
{
    return(highpotentialcatch);
}

int Vessel::get_notthatfar () const
{
    return(notthatfar);
}

int Vessel::get_mosthistoricallyused () const
{
    return(mosthistoricallyused);
}



//------------------------------------------------------------//
//------------------------------------------------------------//
// setters...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::set_previous_harbour_idx(int _previous_harbour_idx)
{
	previous_harbour_idx= _previous_harbour_idx;
}


void Vessel::set_speed(double _speed)
{
	speed= _speed;
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


void Vessel::set_spe_harbours (vector<int> _harbours)
{
	harbours= _harbours;
}


void Vessel::set_spe_fgrounds (vector<int> _fgrounds)
{
	fgrounds=_fgrounds;
}

void Vessel::set_spe_fgrounds_init (vector<int> _fgrounds_init)
{
    fgrounds_init=_fgrounds_init;
}


void Vessel::set_fgrounds_in_closed_areas (vector<int> _fgrounds)
{
    fgrounds_in_closed_areas=_fgrounds;
}



void Vessel::set_spe_freq_harbours (vector<double> _freq_harbours)
{
	freq_harbours= _freq_harbours;
}


void Vessel::set_spe_freq_fgrounds (vector<double> _freq_fgrounds)
{
	freq_fgrounds=_freq_fgrounds;
}

void Vessel::set_spe_freq_fgrounds_init (vector<double> _freq_fgrounds_init)
{
    freq_fgrounds=_freq_fgrounds_init;
}


void Vessel::set_spe_cumcatch_fgrounds (vector<double> _cumcatch_fgrounds)
{
	cumcatch_fgrounds=_cumcatch_fgrounds;
}

void Vessel::set_spe_experienced_bycatch_prop_on_fgrounds (vector<double> _experienced_bycatch_prop_on_fgrounds)
{
    experienced_bycatch_prop_on_fgrounds=_experienced_bycatch_prop_on_fgrounds;
}



void Vessel::set_spe_cumeffort_fgrounds (vector<double> _cumeffort_fgrounds)
{
	cumeffort_fgrounds=_cumeffort_fgrounds;
}


void Vessel::set_spe_experiencedcpue_fgrounds (vector<double> _experiencedcpue_fgrounds)
{
	experiencedcpue_fgrounds=_experiencedcpue_fgrounds;
}


void Vessel::set_spe_betas_per_pop (vector<double> _betas_per_pop)
{
	vessel_betas_per_pop=_betas_per_pop;
}


void Vessel::set_spe_percent_tac_per_pop (vector<double> _tacs_per_pop)
{
	percent_tac_per_pop=_tacs_per_pop;
}


void Vessel::set_spe_possible_metiers (multimap <int, int>  _possible_metiers)
{
	possible_metiers=_possible_metiers;
}


void Vessel::set_spe_freq_possible_metiers  (multimap <int, double>  _freq_possible_metiers)
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
            dout(cout  << cpue_nodes_species[i][j] << " ");
		}
        dout(cout  << endl);
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
            dout(cout  << gshape_cpue_nodes_species[i][j] << " ");
		}
        dout(cout  << endl);
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
            dout(cout  << gscale_cpue_nodes_species[i][j] << " ");
		}
        dout(cout  << endl);
	}

}


void Vessel::set_cpue_nodes_species(int idx_node, vector<double> newval)
{
    for(unsigned int sp = 0; sp < cpue_nodes_species[idx_node].size(); sp++)
	{
		cpue_nodes_species[idx_node][sp]= newval[sp];
	}

}


void Vessel::set_gshape_cpue_nodes_species(int idx_node, vector<double> newval)
{
    for(unsigned int sp = 0; sp < gshape_cpue_nodes_species[idx_node].size(); sp++)
	{
		gshape_cpue_nodes_species[idx_node][sp]= newval[sp];
	}

}


void Vessel::set_gscale_cpue_nodes_species(int idx_node, vector<double> newval)
{
    for(unsigned int sp = 0; sp < gscale_cpue_nodes_species[idx_node].size(); sp++)
	{
		gscale_cpue_nodes_species[idx_node][sp]= newval[sp];
	}

}


void Vessel::set_cumcatch_fgrounds (vector<double>  _cumcatch_fgrounds)
{
	cumcatch_fgrounds=_cumcatch_fgrounds;
}


void Vessel::set_experienced_bycatch_prop_on_fgrounds (vector<double>  _experienced_bycatch_prop_on_fgrounds)
{
    experienced_bycatch_prop_on_fgrounds=_experienced_bycatch_prop_on_fgrounds;
}




void Vessel::set_cumcatch_fgrounds_per_pop (vector<vector<double> >  _cumcatch_fgrounds_per_pop)
{
	cumcatch_fgrounds_per_pop=_cumcatch_fgrounds_per_pop;
}


void Vessel::set_cumeffort_fgrounds (vector<double>  _cumeffort_fgrounds)
{
	cumeffort_fgrounds=_cumeffort_fgrounds;
}


void Vessel::set_experiencedcpue_fgrounds (vector<double>  _experiencedcpue_fgrounds)
{
	experiencedcpue_fgrounds=_experiencedcpue_fgrounds;
}


void Vessel::set_experiencedcpue_fgrounds_per_pop (vector<vector<double> >  _experiencedcpue_fgrounds_per_pop)
{
	experiencedcpue_fgrounds_per_pop=_experiencedcpue_fgrounds_per_pop;
}


void Vessel::set_freq_experiencedcpue_fgrounds (vector<double>  _freq_experiencedcpue_fgrounds)
{
	freq_experiencedcpue_fgrounds=_freq_experiencedcpue_fgrounds;
}


void Vessel::set_freq_experiencedcpue_fgrounds_per_pop (vector<vector<double> > _freq_experiencedcpue_fgrounds_per_pop)
{
	freq_experiencedcpue_fgrounds_per_pop=_freq_experiencedcpue_fgrounds_per_pop;
}


void Vessel::set_roadmap (const list<vertex_t> &_roadmap)
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


void Vessel::clear_idx_used_metiers_this_trip()
{
	idx_used_metiers_this_trip.clear();
}


void Vessel::set_nbfpingspertrip(int _nbfpingpertrip)
{
	nbfpingspertrip=_nbfpingpertrip;
}


void Vessel::set_individual_tac_this_pop(ofstream& export_individual_tacs, int tstep, vector<Population* >& populations, int pop, int init, double a_tac)
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
        dout(cout << "this vessel: individual TAC set to " << individual_tac_per_pop.at(pop) <<
			" for this pop " << pop <<
			" from global tac " << global_tac_this_pop <<
			" percent_simulated_tac_this_pop " << percent_simulated_tac_this_pop <<
			" percent_tac_per_pop " << percent_tac_per_pop.at(pop) <<
            " relative key " << relative_key[this->get_nationality()] << endl);

		// inform if this vessel is actually linked with the management under investigation...
		// (if not then the vessel will not be forced to stay on quayside when TACs exhausted)
		if(individual_tac_per_pop.at(pop)!=0) this->set_targeting_non_tac_pop_only (0);

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
            a_tac << " " <<
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

void Vessel::updateTripsStatistics(const std::vector<Population* >& populations)
{
    double cumProfit = avgProfit * numTrips;
    double cumRevenues = avgRevenues * numTrips;

    if (numTrips > 0) {
        avgRevenues += (cumRevenues + lastTrip_revenues) / numTrips;
        avgProfit += (cumProfit + lastTrip_profit) / numTrips;
    } else {
        avgRevenues = avgProfit = 0.0;
    }

    lastTrip_revenues = 0.0;
    lastTrip_profit = 0.0;
    const vector< vector<double> > &a_catch_pop_at_szgroup = get_catch_pop_at_szgroup();
    for(unsigned int pop = 0; pop < a_catch_pop_at_szgroup.size(); pop++)
    {
        vector<int> comcat_at_szgroup =   populations[pop]->get_comcat_at_szgroup();

        for(unsigned int sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
        {
            int comcat_this_size =comcat_at_szgroup.at(sz);
            lastTrip_revenues += a_catch_pop_at_szgroup[pop][sz] * get_loc()->get_prices_per_cat(pop, comcat_this_size);
        }
    }

    double fuelcost = get_cumfuelcons() * get_loc()->get_fuelprices(length_class);
    lastTrip_profit = lastTrip_revenues - fuelcost;

    ++numTrips;
}

/** \brief Starting from the dtree root, traverse it evaluating any node and the relative Variable.
 * The return value from StateEvaluator::evaluate() is rounded and casted to int to define the next node
 * */
double Vessel::traverseDtree(int tstep, dtree::DecisionTree *tree)
{
    int bin = -1;
    double value = 0.0f;

    boost::shared_ptr<dtree::Node> prevnode;
    boost::shared_ptr<dtree::Node> node = tree->root();
    while (node.get()) {
        if (node->getChildrenCount() == 0) { // is a leaf node
//            std::cout << "Node Value= " << node->value() << std::endl;
            return node->value();
        }

        value = 0.0;
        if (mStateEvaluators[static_cast<int>(node->variable())] != 0) {
            value = mStateEvaluators[static_cast<int>(node->variable())]->evaluate(tstep, this);
            //cout << "vessel " << this->get_name() << " evaluation gets back " << value << endl;
        } else {
            throw std::runtime_error("Unsupported variable evaulation requested.");
        }

        bin = static_cast<int>(std::floor(value*node->getChildrenCount() + 0.5));

        //std::cout << "value=" << value << " bin=" << bin << std::endl;
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


void Vessel::set_smartcatch(int _smartcatch)
{
     smartcatch=_smartcatch;
}

void Vessel::set_highpotentialcatch(int _highpotentialcatch)
{
     highpotentialcatch=_highpotentialcatch;
}

void Vessel::set_notthatfar(int _notthatfar)
{
     notthatfar=_notthatfar;
}

void Vessel::set_mosthistoricallyused(int _mosthistoricallyused)
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


//void Vessel::move_to(boost::shared_ptr<Node> pnext_node)
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
    }

    assert(roadmap.size() > 0);

	list<vertex_t>::iterator pos = roadmap.begin();
	list<vertex_t>::iterator pos2 = roadmap.end();

	bool returning_to_harbour= nodes[*(--pos2)]->get_is_harbour();
    dout(cout  <<  " returning_to_harbour " << returning_to_harbour  << endl);

	if(roadmap.size()==0)
	{
		cout << "DEBUG !! " << endl;
	}

	if(roadmap.size()==1)
	{

		//do a discrete jump on the final node
        dout (cout << "JUMP?? FROM NODE " << this->get_loc()->get_idx_node()
			<< " " << this->get_loc()->get_x() << " " << this->get_loc()->get_y()
			<<  " TO NODE " << nodes[*pos]->get_idx_node()
            << " " << nodes[*pos]->get_x() << " " << nodes[*pos]->get_y()  << endl);

		double dist_for_one_ping = this->get_speed() * PING_RATE * NAUTIC;
		double dist_next_node = dist(this->get_x(), this->get_y(), nodes[*pos]->get_x(), nodes[*pos]->get_y());
		if(dist_for_one_ping > dist_next_node)
		{
            dout(cout  << "YES: JUMP!!" << endl);
			this->move_to(nodes[*pos]);
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
			double b = bearing (this->get_x(), this->get_y(), nodes[*pos]->get_x(), nodes[*pos]->get_y());
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
        dout(cout  << "SEMI-CONTINUOUS MOVE...from here " << this->get_loc()->get_idx_node() << endl);
        dout(cout  << " to NODE " << nodes[*pos]->get_idx_node() << endl);
		bool flag = false;
		// travel on the graph as long as dist_traveled is reached.
        dout(cout << "SEMI-CONTINUOUS MOVE...FROM NODE " << this->get_loc()->get_idx_node()
            << " TO NODE " << nodes[*pos]->get_idx_node() << endl);
		next_x= nodes[*pos]->get_x();
		next_y= nodes[*pos]->get_y();
        dout(cout  << "x " << x << " y " << y  << " next_x " << next_x << " next_y " << next_y << endl);
		double dist_next_node = dist(x, y, next_x, next_y);
		double dist_traveled = this->get_speed() * PING_RATE * NAUTIC;
		this->set_distprevpos(dist_next_node) ;
		double b = bearing (x, y, next_x, next_y);
		this->set_course(b);
        dout(cout  << "bearing between " << this->get_loc()->get_idx_node() << " and " <<nodes[*pos]->get_idx_node() <<" " << b << endl);
        dout(cout  << "START BEARING " << this->get_course() <<endl);

        dout(cout << "distance to next node (before moving): " << dist_next_node
            << ", dist to be traveled: " << dist_traveled  << endl);

		double dist_sauv;		 // required to get the remaining dist
		while(dist_traveled > dist_next_node)
		{
			next_x= nodes[*pos]->get_x();
			next_y= nodes[*pos]->get_y();
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
            // dout(cout  << "pass through node " << nodes[*pos]->get_idx_node() << endl);
			//  x = next_x;
			// y = next_y;
			if(roadmap.size()>1)
			{
				this->move_to(nodes[*pos]);
                dout(cout  << "pass through node " << nodes[*pos]->get_idx_node() << endl);
				x = next_x;
				y = next_y;
								 // erode
				roadmap.pop_front();
				pos= roadmap.begin();
			}
			else
			{
				double dist_for_one_ping = this->get_speed() * PING_RATE * NAUTIC;
				double dist_next_node = dist(this->get_x(), this->get_y(), nodes[*pos]->get_x(), nodes[*pos]->get_y());
				if(dist_for_one_ping > dist_next_node)
				{

                    dout(cout  << "YES: JUMP TO THE FINAL NODE" << endl);
								 // JUMP
					this->move_to(nodes[*pos]);
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
					b = bearing (this->get_x(), this->get_y(), nodes[*pos]->get_x(), nodes[*pos]->get_y());
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
			b = bearing (this->get_loc()->get_x(), this->get_loc()->get_y(), nodes[*pos]->get_x(), nodes[*pos]->get_y());
            dout(cout  << "bearing between " << this->get_loc()->get_idx_node() << " and " <<nodes[*pos]->get_idx_node() <<" " << b << endl);
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

void Vessel::do_catch(ofstream& export_individual_tacs, vector<Population* >& populations, vector<Node* >& nodes,
                      vector<int>& implicit_pops, int& tstep, double& graph_res, int& is_individual_vessel_quotas)
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
	vector<double> dis_ogive        = this->get_metier()->get_discards_ogive();
	vector<double> sel_ogive        = this->get_metier()->get_selectivity_ogive();
	vector<double> m_betas_per_pop  = this->get_metier()->get_betas_per_pop();
    vector<int> m_mls_cat_per_pop  = this->get_metier()->get_mls_cat_per_pop();
    double fspeed                   = this->get_metier()->get_fspeed();
    double gear_width_a             = this->get_metier()->get_gear_width_a();
	double gear_width_b             = this->get_metier()->get_gear_width_b();
	string gear_width_model         = this->get_metier()->get_gear_width_model();

	// SWEPT AREA
	double gear_width=0.0;
	double swept_area=0.0;
	if(gear_width_model=="a*(LOA^b)")  gear_width=gear_width_a* pow(v_vsize,gear_width_b);
	if(gear_width_model=="a*(kW^b)")   gear_width=gear_width_a* pow(v_kw,gear_width_b);
	if(gear_width_model=="(a*LOA)+b")  gear_width=(gear_width_a*v_vsize) +gear_width_b;
	if(gear_width_model=="(a*kW)+b")   gear_width=(gear_width_a*v_kw) +gear_width_b;
                                 // converted to the right unit i.e. km2 and assuming fishing at fspeed knots
    if(this->get_metier()->get_metier_type()==1){
    swept_area = gear_width/1000 * PING_RATE * fspeed*NAUTIC; // for trawlers
    } else{
    swept_area =  PI*pow(((gear_width/1000)/(2*PI)),2);  // seiners and gillnetters
    }

    dout( cout << " for this model " << gear_width_model << " the gear width is " << gear_width
		<< "from KW "<<  v_kw << " and vessel size "<< v_vsize << " and param a " << gear_width_a << " param b " <<gear_width_b
		<< ", swept area this fishing event is then:" << swept_area
        << " compared to the cell area which is " << graph_res*graph_res << endl);
    this->get_loc()->add_to_cumsweptarea(swept_area);


	// FIND OUT THE DECREASE FACTOR AFTER THE PASSAGE
    int a_landscape =           this->get_loc()->get_marine_landscape();
    multimap<int,double> loss=  this->get_metier()->get_loss_after_1_passage();
	vector<double> loss_after_1_passage_per_func_group=find_entries_i_d (loss,  a_landscape);


    // THEN, DEPLETE THE UNDERLYING BENTHOS ON THIS NODE...
    double decrease_factor_on_benthos_funcgroup;
    for (unsigned int funcid=0; funcid< this->get_loc()->get_benthos_tot_biomass().size(); funcid++)
    {
         dout(cout  << "before " << this->get_loc()->get_benthos_tot_biomass(funcid));
         decrease_factor_on_benthos_funcgroup= 1-(1-(loss_after_1_passage_per_func_group.at(funcid)*(swept_area/(graph_res*graph_res)) ) );
         outc (cout << "for this func " << funcid << " the loss_after_1_passage_per_func_group is "
                    << loss_after_1_passage_per_func_group.at(funcid) << " for this landscape "<< a_landscape
                    << ", then decrease_factor_on_benthos_funcgroup this fishing event is: "
                    << decrease_factor_on_benthos_funcgroup << endl);
         this->get_loc()->set_benthos_tot_biomass(funcid, this->get_loc()->get_benthos_tot_biomass(funcid)*(1-decrease_factor_on_benthos_funcgroup));
         dout(cout  << "after " << this->get_loc()->get_benthos_tot_biomass(funcid));
    }

	// NODE ATTRIBUTES
	vector <int> pop_names = this->get_loc()->get_pop_names_on_node();
	sort (pop_names.begin(), pop_names.end());
	int idx_node = this->get_loc()->get_idx_node();
	int code_area = this->get_loc()->get_code_area();
    vector <int> the_grds = this->get_fgrounds();
                     // relative node index to this vessel
    int idx_node_r= find(the_grds.begin(), the_grds.end(), idx_node) - the_grds.begin();

    // VARIABLES VALID FOR THIS FISHING EVENT ONLY
    double totLandThisEvent=1;
    double totDiscThisEvent=0.0001;


    // OUTPUTS
								 // declare with length nbpops
	vector<double> tot_catch_per_pop(  catch_pop_at_szgroup.size() );

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
                double tot                          = 0;
                double tot_avai_for_land            = 0;
                double tot_avai_for_disc            = 0;
                int    MLS_cat                      = m_mls_cat_per_pop[pop];

				// compute available biomass via selectivity
                for(int szgroup=0; szgroup < (int)avail_biomass.size(); szgroup++)
				{
                    all_biomass[szgroup]   =  Ns_at_szgroup_pop[szgroup]*wsz[szgroup];
                    avail_biomass[szgroup] =  all_biomass[szgroup]      *sel_ogive[szgroup]; // available for landings only
								 // cumul
					tot = tot+avail_biomass[szgroup];
                    if(szgroup >=MLS_cat) {
                        tot_avai_for_land= tot_avai_for_land+avail_biomass[szgroup];
                    } else{
                        tot_avai_for_disc= tot_avai_for_disc+all_biomass[szgroup];
                    }
                    dout(cout  << "wsz[szgroup] " <<wsz[szgroup] << endl);
                    dout(cout  << "sel_ogive[szgroup] " <<sel_ogive[szgroup] << endl);
                    dout(cout  << "avail_biomass[szgroup] " <<avail_biomass[szgroup] << endl);
				}

                dout(cout  << "tot biomass available on this node " << tot << endl);

				//if(tot!=tot)  // c++ trick for like testing for is.nan
				//{
				//    int a;
				//    cout << "1: nan detected" << endl;
				//    cout << "1: nan detected...Pause: type a number to continue";
				//    cin >> a;
				//}

				if(tot!=0)
				{
                    // remember that metier at NA in parameterisation (see the Rcode) have been set to -20
					// in order to avoid (potentially high!) catches with the wrong metier

					// 2. COMPUTE TOTAL CATCH IN WEIGHT as the response from the glm model on cpue
					// (kg per hour i.e. PING:RATE at 1, for a calibration year, poisson regression, see the R code)
					// rescaling multiplying by 1000, see R code.
					vector<double> avai_pops_at_selected_szgroup = nodes.at(idx_node)->get_avai_pops_at_selected_szgroup(pop);

								 // vessel effect
					tot_catch_per_pop[pop] = min(tot, exp(v_betas_per_pop[pop]*1 +
								 // metier effect
						m_betas_per_pop[pop]*1 +
								 // avai effect szgroup 0
						populations[pop]->get_avai0_beta()* avai_pops_at_selected_szgroup[0] *1000 *sel_ogive[selected_szgroups.at(0)] +
								 //szgroup 2
						populations[pop]->get_avai2_beta()* avai_pops_at_selected_szgroup[1] *1000 *sel_ogive[selected_szgroups.at(1)]+
								 //szgroup 3
						populations[pop]->get_avai3_beta()* avai_pops_at_selected_szgroup[2] *1000 *sel_ogive[selected_szgroups.at(2)]+
								 //szgroup 5
						populations[pop]->get_avai5_beta()* avai_pops_at_selected_szgroup[3] *1000 *sel_ogive[selected_szgroups.at(3)]+
								 //szgroup 7
						populations[pop]->get_avai7_beta()* avai_pops_at_selected_szgroup[4] *1000 *sel_ogive[selected_szgroups.at(4)]
								 // poisson regression, see the R code
						)*populations[pop]->get_cpue_multiplier() );
					// 'min' is there for not allowing catching more than available!

                    dout(cout  << "cpue_multiplier is " << populations[pop]->get_cpue_multiplier() << endl);

					// REMENBER THAT THE IDEAL WOULD BE DO THE THE GLM ON ABSOLUTE NUMBER OF INDIVIDUAL ON EACH NODE......
					// BUT THIS INFO IS NOT AVAILABLE OF COURSE (WE ONLY HAVE THE N FOR THE FIRST OF JANUARY)
					// THATS WHY WE USE THE RELATIVE AVAILABILITY AS A PROXY.

					//for(int i = 0; i < cumcatch_fgrounds.size(); i++){
					//cout << "on the grounds of this vessel " << the_grds.at(i) << " cumcatch is " << cumcatch_fgrounds.at(i) << endl;
					//}


                    // compute the landings vs. discard part
                    // in function of MLS and gear selectivity ogive and the N
                    vector<double> Ns_at_szgroup_pop_scaled = Ns_at_szgroup_pop; // init
                    for(int sizgroup=0; sizgroup<(int)Ns_at_szgroup_pop.size(); sizgroup++) {
                        Ns_at_szgroup_pop_scaled.at(sizgroup)=Ns_at_szgroup_pop_scaled.at(sizgroup)/
                                 *(max_element(Ns_at_szgroup_pop.begin(), Ns_at_szgroup_pop.end()));
                    }
                    int inter=0;
                    int a_szgroup=0;
                    while(a_szgroup<=MLS_cat){
                       inter = a_szgroup;
                       a_szgroup+=1;
                    }
                    double left_to_MLS=0;
                    double right_to_MLS=0;
                    if(sel_ogive.at(inter)>Ns_at_szgroup_pop_scaled.at(inter)){
                        left_to_MLS  = trapezoidal(0, inter, sel_ogive) + trapezoidal(inter, MLS_cat, Ns_at_szgroup_pop_scaled); // discards
                        right_to_MLS = trapezoidal(MLS_cat, NBSZGROUP-1, Ns_at_szgroup_pop_scaled); // landings

                      } else{
                        left_to_MLS  = trapezoidal(0, MLS_cat, sel_ogive); // discards
                        right_to_MLS = trapezoidal(MLS_cat, inter, sel_ogive)+trapezoidal(inter, NBSZGROUP-1, Ns_at_szgroup_pop_scaled); // landings

                    }

                     double tot_landings_this_pop=tot_catch_per_pop[pop];
                     double tot_discards_this_pop=tot_catch_per_pop[pop]* left_to_MLS/right_to_MLS;
                    // then disagregate per szgroup....


                    // 3. DISAGREGATE TOTAL LANDINGS IN WEIGHT INTO SZGROUP
					//  AND CONVERT INTO REMOVALS IN NUMBER
					vector <double> totN = populations[pop]->get_tot_N_at_szgroup_just_after_redistribution();
								 // init
					vector<double> new_avai_pops_at_selected_szgroup=avai_pops_at_selected_szgroup;



                    for(int szgroup=0; szgroup <(int)avail_biomass.size(); szgroup++)
					{
                        if(all_biomass[szgroup]!=0)
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
								if(szgroup==0 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
								{
									val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
									new_avai_pops_at_selected_szgroup.at(0)=val;
								}
								if(szgroup==2 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
								{
									val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
									new_avai_pops_at_selected_szgroup.at(1)=val;
								}
								if(szgroup==3 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
								{
									val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
									new_avai_pops_at_selected_szgroup.at(2)=val;
								}
								if(szgroup==5 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
								{
									val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
									new_avai_pops_at_selected_szgroup.at(3)=val;
								}
								if(szgroup==7 && totN[szgroup]!=0 && (removals_per_szgroup[szgroup]<Ns_at_szgroup_pop[szgroup]))
								{
									val= (new_Ns_at_szgroup_pop[szgroup])/(totN[szgroup]) ;
									new_avai_pops_at_selected_szgroup.at(4)=val;
								}
								nodes.at(idx_node)->set_avai_pops_at_selected_szgroup(pop, new_avai_pops_at_selected_szgroup);

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
                            catch_pop_at_szgroup[pop][szgroup] += landings_per_szgroup[szgroup];

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
						}

                    } // end szgroup

   //if(tot_landings_this_pop>1000){
   //int aa;
   //cin >> aa;
   //}


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
                    for(unsigned int a_szgroup = 0; a_szgroup < catch_per_szgroup.size(); a_szgroup++)
					{
						a_cumul_weight_this_pop_this_vessel +=catch_per_szgroup[a_szgroup];
					}

                    // the management is not applied the first year, because the first year is the calibration year.
                    if(tstep>8761 && is_individual_vessel_quotas)
					{
						// MANAGEMENT MEASURE:
						// THE TAC: check against first the individual quota, then the global tac...
						// 1. get the individual vessel tac for this pop
                        int remaining_individual_tac_this_pop = this->get_individual_tac(pop);
						// 2. compare (AT THE VESSEL SCALE)
                        if(a_cumul_weight_this_pop_this_vessel>remaining_individual_tac_this_pop)
						{
                            dout(cout  << this->get_name()  << ": individual quota is "<< remaining_individual_tac_this_pop << " for this pop: overshooted" << endl);

							// reaction = discard
                            dout (cout << "...then this vessel (with quota " << remaining_individual_tac_this_pop << ") now discards all (" <<
                                a_cumul_weight_this_pop_this_vessel <<") for this pop " << pop << "!!! " << endl);

								 // force a zero quota on this pop. (and export discards)
                            this->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, pop, 0, a_cumul_weight_this_pop_this_vessel);
							// => if all quotas at 0 then the vessel will stay on quayside in should_i_go_fishing()...
								 // what a waste !!...
							a_cumul_weight_this_pop_this_vessel=0.0;

						}
						else
						{
                            dout(cout  << "individual quota this pop still ok...but now decrease the amount by the last catches." << endl);
                            this->set_individual_tac_this_pop(export_individual_tacs, tstep, populations, pop, 0,
                                                              remaining_individual_tac_this_pop- a_cumul_weight_this_pop_this_vessel);
                        }
					}
					// 3. get the global tac for this pop
                    dout(cout  << "BEFORE " << populations.at(pop)->get_landings_so_far());

					double so_far = (populations.at(pop)->get_landings_so_far()) +
						a_cumul_weight_this_pop_this_vessel;
								 // "real-time" update, accounting for this last bit of catches
					populations.at(pop)->set_landings_so_far(so_far);
					// note that oth_land (per node) are also added to landings_so_far but at the start of each month.

                    dout(cout  << "the new catch is  " << tot_catch_per_pop[pop] << endl);
                    dout(cout  << "landings so far for pop " << pop << ", AFTER: " << populations.at(pop)->get_landings_so_far());

								 // because the first year is the calibration year.
					if(tstep>8761)
					{
						// 4. compare in tons (AT THE GLOBAL SCALE)
						if( (so_far/1000) > (populations.at(pop)->get_tac()->get_current_tac()))
						{
                            dout (cout << "used " <<
								(so_far/1000) / (populations.at(pop)->get_tac()->get_current_tac())*100  <<
                                " % global quota of " << populations.at(pop)->get_tac()->get_current_tac() << " this pop: overshoot..." << endl);

							// reaction
                            dout(cout  << "Global TAC reached...then discard all for this pop " << pop << "!!! " << endl);
							//populations.at(pop)->set_landings_so_far(so_far -a_cumul_weight_this_pop_this_vessel);
							// => back correction (disable if you want to know the discarded part in annual_indic.
							// ...i.e. discarded = so_far - current_tac)
								 // what a waste !!...
							a_cumul_weight_this_pop_this_vessel=0;

						}
						else
						{
                            dout (cout << "used " <<
								(so_far/1000) / (populations.at(pop)->get_tac()->get_current_tac())*100  <<
                                " % global quota of " << populations.at(pop)->get_tac()->get_current_tac() << " this pop: ok." << endl);
						}

					}			 // end individual TAC management

					// update dynamic trip-based cumul for this node
								 // CUMUL FOR THE TRIP (all species confounded)
					this->cumcatches+= a_cumul_weight_this_pop_this_vessel;
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



        }
        else  // implicit pop:
		{
            dout(cout  << "this pop " << populations.at(pop)->get_name() << " is implicit (or outside the range)...catch knowing cpue only! " << endl);
            // tips: put tot catch into the first bin...we don´t care which szgroup here...
			//vector<double> cpues = find_entries_i_d (cpue_per_stk_on_nodes, idx_node);
			vector <int> grds = this->get_fgrounds();
								 // relative node index to this vessel
			int idx_node_v= find(grds.begin(), grds.end(), idx_node) - grds.begin();
			//if((this->get_name())=="DNK000007718")cout <<this->get_name() << ": the cpue for this pop " << populations.at(pop)->get_name()
			//        << " on this node "<< idx_node << endl;

			//double cpue = cpue_nodes_species.at(idx_node_v).at(pop); // look into the vector of vector....
			// replaced by:
								 // look into the vector of vector....
			double a_shape = gshape_cpue_nodes_species.at(idx_node_v).at(pop);
								 // look into the vector of vector....
			double a_scale = gscale_cpue_nodes_species.at(idx_node_v).at(pop);
			double cpue = rgamma(a_shape, a_scale);

            dout (cout<<this->get_name() << ": the cpue for this pop " << populations.at(pop)->get_name()
                << " on this node "<< idx_node << " is " << cpue << endl);

			if(cpue!=0)
			{
								 // ON THIS NODE, cpue per hour, see the R code
				catch_pop_at_szgroup[pop][0] = cpue*PING_RATE;
								 // CUMUL
				this->cumcatches+= catch_pop_at_szgroup[pop][0];

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
								 // catches per pop
				cumcatch_fgrounds_per_pop.at(idx_node_r).at(pop) += cpue*PING_RATE;
								 // effort
				cumeffort_fgrounds.at(idx_node_r) += PING_RATE;

			}
		}


    }


    // compute the proportion of discard of this event on that ground
    // (of all explicit species) to potentially influence future decision-making (see ChooseGround dtree)
    experienced_bycatch_prop_on_fgrounds.at(idx_node_r)= totDiscThisEvent/(totLandThisEvent+totDiscThisEvent);



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

    dout(cout  << "compute experienced cpue on grounds and clear cum effort and catch..." << endl);
    for(unsigned int a_node = 0; a_node < experiencedcpue_fgrounds.size(); a_node++)
	{
								 // change cpue only if the node have been visited...otherwise the initial guess for cpue is kept
		if(cumeffort_fgrounds.at(a_node)!=0)
		{
            dout(cout  << "on the grounds of this vessel cumcatch is " << cumcatch_fgrounds.at(a_node) << endl);
            dout(cout  << "on the grounds of this vessel cumeffort is " << cumeffort_fgrounds.at(a_node) << endl);
			experiencedcpue_fgrounds.at(a_node)= cumcatch_fgrounds.at(a_node) / cumeffort_fgrounds.at(a_node);
            dout(cout  << "on the grounds of this vessel experienced cpue is then " << experiencedcpue_fgrounds.at(a_node) << endl);
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
            dout(cout  << "scaled experienced cpue is then " << freq_experiencedcpue_fgrounds.at(a_node) << endl);
		}
	}
}


void Vessel::compute_experiencedcpue_fgrounds_per_pop()
{
	vector<double> cum_cpue_over_pop;

    // note that, at the tstep=0, no one single node has been visited yet, so experiencedcpue_fgrounds is full of guesses!
	// but there are qualified guesses: actually cpue from the frequency given by the input data...

    dout(cout  << "compute experienced cpue on grounds and clear cum effort and catch..." << endl);
    for(unsigned int a_node = 0; a_node < experiencedcpue_fgrounds_per_pop.size(); a_node++)
	{
		cum_cpue_over_pop.push_back(0);

        for(unsigned int pop = 0; pop < experiencedcpue_fgrounds_per_pop[a_node].size(); pop++)
		{
								 // change cpue only if the node have been visited...otherwise the initial guess for cpue is kept
			if(cumeffort_fgrounds.at(a_node)!=0)
			{
                dout(cout  << "on the grounds of this vessel cumcatch is " << cumcatch_fgrounds_per_pop.at(a_node).at(pop) << endl);
                dout(cout  << "on the grounds of this vessel cumeffort is " << cumeffort_fgrounds.at(a_node) << endl);
				experiencedcpue_fgrounds_per_pop.at(a_node).at(pop)= cumcatch_fgrounds_per_pop.at(a_node).at(pop) / cumeffort_fgrounds.at(a_node);
                dout(cout  << "on this ground, this vessel experienced a cpue of " << experiencedcpue_fgrounds_per_pop.at(a_node).at(pop) << endl);
			}
								 // cumul to scale to 1 (just below)
			cum_cpue_over_pop.at(a_node) +=experiencedcpue_fgrounds_per_pop.at(a_node).at(pop);

			//  scale to 1 for use in do_sample() => freq_experiencedcpue_fgrounds_per_pop
			if(cum_cpue_over_pop.at(a_node)!=0)
			{
                for(unsigned int pop = 0; pop < experiencedcpue_fgrounds_per_pop[a_node].size(); pop++)
				{
					freq_experiencedcpue_fgrounds_per_pop.at(a_node).at(pop)= experiencedcpue_fgrounds_per_pop.at(a_node).at(pop) / cum_cpue_over_pop.at(a_node);
                    dout(cout  << "scaled experienced cpue this pop is then " << freq_experiencedcpue_fgrounds_per_pop.at(a_node).at(pop) << endl);
				}
			}

		}

	}

}


void Vessel::clear_cumcatch_and_cumeffort()
{
	vector <int> the_grds = this->get_fgrounds();
    for(unsigned int n=0; n<the_grds.size(); n++)
	{
        for(unsigned int pop=0; pop< cumcatch_fgrounds_per_pop.at(n).size(); pop++)
		{
								 //clear
			cumcatch_fgrounds_per_pop.at(n).at(pop) =0;
		}
								 //clear
		cumcatch_fgrounds.at(n) =0;
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

	vector <int> the_grds = this->get_fgrounds();
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

		if (binary_search (polygon_nodes.begin(), polygon_nodes.end(), the_grds.at(n)))
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


vector<double> Vessel::expected_profit_on_grounds(const vector <int>& idx_path_shop,
                                        const deque <map<vertex_t, vertex_t> >& path_shop,
                                        const deque <map<vertex_t, weight_t> >& min_distance_shop)
{
    vector <double> freq_grds = this->get_freq_fgrounds();
                                 // get_experiencedcpue_fgrounds_per_pop is scaled to 1
    vector <vector<double> > past_freq_cpue_grds_pops = this-> get_freq_experiencedcpue_fgrounds_per_pop();

    vector <double> revenue_per_fgrounds(freq_grds.size());
    vector <double> fcost_per_fgrounds(freq_grds.size());
    vector <double> scost_per_fgrounds(freq_grds.size());
    vector <double> profit_per_fgrounds(freq_grds.size());

    // distance to all grounds (through the graph...)
    int from = this->get_loc()->get_idx_node();
    vector<int> the_grounds = this->get_fgrounds();
    vector <double> distance_fgrounds = compute_distance_fgrounds(idx_path_shop,
        path_shop, min_distance_shop, from, the_grounds);

                                 // vsize
    int length_class =this->get_length_class();

    //if(tstep>1) dout(cout << "the vessel "<< this->get_name() << " ask for a good guess..." << endl);

    vector<int>  trgts =this->get_metier()->get_metier_target_stocks();
    for(unsigned int gr=0; gr<freq_grds.size(); gr++)
    {
        //if(tstep>1) dout(cout << "...on this ground " << gr << endl);

        //1. first, compute the expected revenue for full vessel load on this ground knowing the experienced cpues
        // and the fish price on the departure harbour. (caution, the vessel is assumed to currently be at the port)
       // .....only looking at the targeted stocks

        for(unsigned int i=0; i<trgts.size(); ++i)
           {
            int a_trgt=trgts.at(i);
            //cout << "pop target is: .." << a_trgt << endl;
            revenue_per_fgrounds.at(gr)+= past_freq_cpue_grds_pops.at(gr).at(a_trgt) * // weighted average of cpues
                                              this->get_carrycapacity() *
                                               // choose the most valuable cat (but actually currently the first one is returned: to do)
                                              this->get_loc()->get_prices_per_cat(a_trgt, 0);

           }


        //if(tstep>1) dout(cout << "the expected revenue on this ground is " << revenue_per_fgrounds.at(gr) << endl);

        //2. compute the expected cost when steaming
                                 // time given the shortest distance divided by the speed...
        double time_for_steaming=0;
                                 // *2 because WE NEED TO GO BACK TO PORT!
        time_for_steaming= (distance_fgrounds.at(gr)/this->get_speed())*2;
        //if(tstep>1) dout(cout << "the expected time to reach this ground is " << time_for_steaming << endl);
        scost_per_fgrounds.at(gr)=   time_for_steaming * this->get_loc()->get_fuelprices(length_class) * this->get_fuelcons() *  this->get_mult_fuelcons_when_steaming();
        //if(tstep>1) dout(cout << "the expected scost on this ground is " << scost_per_fgrounds.at(gr) << endl);

        //3. compute the expected cost when fishing
        double time_to_be_full_of_catches_if_infinite_fuel_tank=0;
        double cpue_this_node = this->experiencedcpue_fgrounds.at(gr);
        if(cpue_this_node!=0)
        {
            time_to_be_full_of_catches_if_infinite_fuel_tank = this->get_carrycapacity() / cpue_this_node;
            //if(tstep>1) dout(cout << "the expected time to fill in the capacity on this ground is " << time_to_be_full_of_catches_if_infinite_fuel_tank << endl);
        }
        else
        {
            time_to_be_full_of_catches_if_infinite_fuel_tank = 100;
        }

        double time_for_fishing_given_fuel_tank= (this->get_tankcapacity() -
            ( time_for_steaming * this->get_fuelcons() *  this->get_mult_fuelcons_when_steaming())) /
            (this->get_fuelcons()) ;// (tank - expected tot fuelcons when steaming) / conso per hour when fishing
        //if(tstep>1) dout(cout << "the expected time to empty the fuel tank on this ground is " << time_for_fishing_given_fuel_tank << endl);

        double time_for_fishing= min(time_to_be_full_of_catches_if_infinite_fuel_tank, time_for_fishing_given_fuel_tank);
        //if(tstep>1) dout(cout << "then, the expected time for fishing on this ground is " << time_for_fishing << endl);

        fcost_per_fgrounds.at(gr)=time_for_fishing * this->get_loc()->get_fuelprices(length_class) * this->get_fuelcons()  *  this->get_mult_fuelcons_when_fishing();
        //if(tstep>1) dout(cout << "the expected fcost on this ground is " << fcost_per_fgrounds.at(gr) << endl);

        //4. then compute the expected profit for this ground
        profit_per_fgrounds.at(gr)= revenue_per_fgrounds.at(gr) -
            scost_per_fgrounds.at(gr) -
            fcost_per_fgrounds.at(gr);
        //if(tstep>1) dout(cout << "the expected profit on this ground is " << profit_per_fgrounds.at(gr) << endl);

    }



    return(profit_per_fgrounds);

}

void Vessel::alloc_on_high_profit_grounds(int tstep,
                                          const vector <int>& idx_path_shop,
                                          const deque <map<vertex_t, vertex_t> >& path_shop,
                                          const deque <map<vertex_t, weight_t> >& min_distance_shop,
                                          ofstream& freq_profit)
{


    vector<double> profit_per_fgrounds = expected_profit_on_grounds(
                                            idx_path_shop,
                                            path_shop,
                                            min_distance_shop);


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
                                     const vector <int>& idx_path_shop,
                                     const deque <map<vertex_t, vertex_t> >& path_shop,
                                     const deque <map<vertex_t, weight_t> >& min_distance_shop
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
		int from = this->get_loc()->get_idx_node();
		vector<int> the_grounds = this->get_fgrounds();
		vector <double> distance_fgrounds = compute_distance_fgrounds(idx_path_shop,
			path_shop, min_distance_shop, from, the_grounds);

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


void Vessel::alloc_on_closer_grounds(int tstep, const vector <int>& idx_path_shop,
const deque<map<vertex_t, vertex_t> >& path_shop,
const deque<map<vertex_t, weight_t> >& min_distance_shop,
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

	int from = this->get_loc()->get_idx_node();
	vector<int> the_grounds = this->get_fgrounds();
	vector <double> distance_fgrounds = compute_distance_fgrounds(idx_path_shop,
		path_shop, min_distance_shop, from, the_grounds);
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
    const vector <int> &grds = this->get_fgrounds();
    vector <double> freq_grds = this->get_freq_fgrounds();
                                 // need to convert in array, see myRutils.cpp
    vector<int> grounds = do_sample(1, grds.size(), &grds[0], &freq_grds[0]);
    int ground=grounds[0];


    //2. get possible metiers on this ground
    const multimap<int, int> &poss_met        = this->get_possible_metiers();
    const multimap<int, double> &freq_poss_met= this->get_freq_possible_metiers();
    vector<int>    metiers_on_grd      = find_entries_i_i( poss_met, ground );
    vector<double> freq_metiers_on_grd = find_entries_i_d( freq_poss_met, ground );
                             // need to convert in array, see myRutils.cpp
    vector<int>    a_met = do_sample(1, metiers_on_grd.size(), &metiers_on_grd[0], &freq_metiers_on_grd[0]);
    this->set_metier(  metiers[ a_met[0] ]  );



}



void Vessel::choose_a_ground_and_go_fishing(int tstep, bool use_the_tree,
        const DynAllocOptions& dyn_alloc_sce,
        int create_a_path_shop,
        const vector<int> &idx_path_shop,
        const deque<map<vertex_t, vertex_t> > &path_shop,
        const deque<map<vertex_t, weight_t> > &min_distance_shop,
        adjacency_map_t& adjacency_map,
        map<vertex_t, weight_t>& min_distance,
        map<vertex_t, vertex_t>& previous,
        vector <int>& relevant_nodes,
        multimap<int, int>& nodes_in_polygons,
        vector<string>& vertex_names,
        vector<Node* >& nodes,
        vector <Metier*>& metiers,
        ofstream& freq_cpue,
        ofstream& freq_profit,
        ofstream& freq_distance
        )
{
    UNUSED(vertex_names);

	this->set_tstep_dep(tstep);	 // store departure date

	// choose a fishing ground
    const vector <int> &grds = this->get_fgrounds();

	// choose the ground, dyn sce. vs. baseline
	int ground;


    if(use_the_tree && dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::ChooseGround)){

        ground=this->should_i_choose_this_ground(tstep,
                                                 nodes,
                                                 idx_path_shop,
                                                 path_shop,
                                                 min_distance_shop); // use ChooseGround dtree along all possible grounds to define the next ground
        if(ground==-1) cout << "Bad probabilities defined in the ChooseGround dtree...need a revision" << endl;

    } else{

       // ************focus_on_high_previous_cpue********************//
       if (dyn_alloc_sce.option(Options::focus_on_high_previous_cpue))
       {

		this->alloc_on_high_previous_cpue(tstep,
			freq_cpue);
       }

       // ************focus_on_high_profit_grounds********************//
       if (dyn_alloc_sce.option(Options::focus_on_high_profit_grounds))
       {

		this->alloc_on_high_profit_grounds(tstep,
			idx_path_shop,
			path_shop,
			min_distance_shop,
			freq_profit);
       }

       // ********realloc among the 3 more used grounds to save 20%*******//
       if (dyn_alloc_sce.option(Options::fuelprice_plus20percent))
       {

           // update freq only if starting from a port different from the last one.
           int from = this->get_loc()->get_idx_node();
           if(from!=this->get_previous_harbour_idx())
           {

			this->alloc_while_saving_fuel(tstep,
				idx_path_shop,
				path_shop,
				min_distance_shop);
           }
           else
           {
            dout(cout  << "not looking for more saving here..." << endl);
           }

       }


       // ****************closer_grounds**********************************//
       if (dyn_alloc_sce.option(Options::closer_grounds))		 // dyn sce.
       {
		this->alloc_on_closer_grounds(tstep,
			idx_path_shop,
			path_shop,
			min_distance_shop,
			freq_distance);
       }

       // ****************area_closure**********************************//
       if (dyn_alloc_sce.option(Options::area_closure))		 // area-based sce
       {
		this->alter_freq_fgrounds_for_nodes_in_polygons(nodes_in_polygons);
		// compliance => 0.0001
       }

       // then, draw a ground from the frequencies (altered or not)...
       vector <double> freq_grds = this->get_freq_fgrounds();
								 // need to convert in array, see myRutils.cpp
       vector<int> grounds = do_sample(1, grds.size(), &grds[0], &freq_grds[0]);
       ground=grounds[0];
       }



       //random_shuffle(grds.begin(),grds.end()); // random permutation i.e. equal frequency of occurence
       //int ground=grds[0];
       dout(cout  << this->get_name() << "GO FISHING ON " << ground << endl);

       // get the shortest path between source and destination
       // with the list of intermediate nodes
       int from = this->get_loc()->get_idx_node();
       this->set_previous_harbour_idx(from);

       if(!create_a_path_shop)
       {
		min_distance.clear();
		previous.clear();
								 // from the source to all nodes
		DijkstraComputePaths(from, adjacency_map, min_distance, previous, relevant_nodes);
       }
       else						 // replaced by:
       {
        vector<int>::const_iterator it = find (idx_path_shop.begin(), idx_path_shop.end(), from);
								 // tricky!
		int idx = it - idx_path_shop.begin();

		previous=path_shop.at(idx);
		min_distance=min_distance_shop.at(idx);
       }
       vertex_t vx = ground;		 // destination
       dout(cout  << "distance to fishing ground " << vertex_names[vx] << ": " << min_distance[vx] << endl);

       // check for area_closure
       if (dyn_alloc_sce.option(Options::area_closure) )
       {
		vector<int> polygons;
		vector<int> polygon_nodes;
		for (multimap<int, int>::iterator pos=nodes_in_polygons.begin(); pos != nodes_in_polygons.end(); pos++)
           {
			// all values across the keys
			polygons.push_back(pos->first);
			polygon_nodes.push_back(pos->second);
            dout(cout  << " a polygon node is " << pos->second << endl);
           }
		sort (polygon_nodes.begin(), polygon_nodes.end());

		if(binary_search (polygon_nodes.begin(), polygon_nodes.end(), ground))
           {
            dout(cout  << "this node " << ground << " is actually within a closed area!!!" << endl);
           }
       }

       list<vertex_t> path = DijkstraGetShortestPathTo(vx, previous);
       if(path.size()>1)			 // i.e no path has been found if path.size()==1...
       {
           path.pop_front();		 // delete the first node (departure) because we are lying in...
           this->set_roadmap(path);
           min_distance.clear();
           previous.clear();
           // show the roadmap
           list<vertex_t> road= this->get_roadmap();
           list<vertex_t>::iterator road_iter = road.begin();

           // check path
           //    dout(cout << "path: ");
           //    for( ; road_iter != road.end(); road_iter++)
           //    {
           //       dout(cout << vertex_names[*road_iter] << " " );
           //    }
           //    dout(cout << endl);

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
           const multimap<int, int> &poss_met        = this->get_possible_metiers();
           const multimap<int, double> &freq_poss_met= this->get_freq_possible_metiers();
           vector<int>    metiers_on_grd      = find_entries_i_i( poss_met, ground );
           vector<double> freq_metiers_on_grd = find_entries_i_d( freq_poss_met, ground );
								 // need to convert in array, see myRutils.cpp
           vector<int>    a_met = do_sample(1, metiers_on_grd.size(), &metiers_on_grd[0], &freq_metiers_on_grd[0]);
           this->set_metier(  metiers[ a_met[0] ]  );
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
           outc(cout << "pble calculating from " << from << " to " << ground << endl);
           this->move_to(nodes.at(from)) ;
           // no path found: assume the vessel stucks at its current location
       }

}


void Vessel::choose_another_ground_and_go_fishing(int tstep,
        const DynAllocOptions &dyn_alloc_sce,
        int create_a_path_shop,
        const vector<int> &idx_path_shop,
        const deque<map<vertex_t, vertex_t> > &path_shop,
        const deque<map<vertex_t, weight_t> > &min_distance_shop,
        adjacency_map_t& adjacency_map,
        map<vertex_t, weight_t>& min_distance,
        map<vertex_t, vertex_t>& previous,
        vector <int>& relevant_nodes,
        const multimap<int, int>& nodes_in_polygons,
        vector<string>& vertex_names,
        vector<Node* >& nodes,
        vector <Metier*>& metiers,
        ofstream& freq_cpue,
        ofstream& freq_distance
        )
{
    UNUSED(tstep);
    UNUSED(freq_cpue);
    UNUSED(freq_distance);
    UNUSED(vertex_names);

    lock();

	bool finally_I_should_go_for_the_closest=false;
	// => in case of area_closure: provoke oscillation at the border if the 2nd closest is inside a closed area

    const vector <int> &grds = this->get_fgrounds();
	//int current_grd = this->get_loc()->get_idx_node();
	//  if(grds.size()>2)
	//  {
	// get the shortest distance between source and destination
	// with the list of intermediate nodes
	vector <double> dist_to_others;
	int from = this->get_loc()->get_idx_node();
    dout(cout  << "current node: " << from << endl);
	min_distance.clear();
	previous.clear();

	if(!create_a_path_shop)
	{
								 // from the source to all nodes
		DijkstraComputePaths(from, adjacency_map, min_distance, previous, relevant_nodes);
	}
	else						 // replaced by:
	{
        vector<int>::const_iterator it = find (idx_path_shop.begin(), idx_path_shop.end(), from);
								 // tricky!
		int idx = it - idx_path_shop.begin();

		previous=path_shop.at(idx);
		min_distance=min_distance_shop.at(idx);

	}

	// check for area_closure
	vector<int> polygons;
	vector<int> polygon_nodes;
    if (dyn_alloc_sce.option(Options::area_closure))
	{

        for (multimap<int, int>::const_iterator pos=nodes_in_polygons.begin(); pos != nodes_in_polygons.end(); pos++)
		{
			// get all values across the keys
			polygons.push_back(pos->first);
			polygon_nodes.push_back(pos->second);
            dout(cout  << " a polygon node is " << pos->second << endl);
		}
		sort (polygon_nodes.begin(), polygon_nodes.end());

	}

	for (unsigned int i =0; i< grds.size(); i++)
	{

		vertex_t vx = grds.at(i);// destination
        dout(cout  << "distance to other grounds " << vertex_names[vx] << ": " << min_distance[vx] << endl);

		// check for area_closure
        if (dyn_alloc_sce.option(Options::area_closure))
		{

			if(binary_search (polygon_nodes.begin(), polygon_nodes.end(), from))
			{
                dout(cout  << "gosh... I am fishing in a closed area there! " << from <<  endl);
				double dist_to_this_node = dist( nodes.at(from)->get_x(),
					nodes.at(from)->get_y(),
					nodes.at(vx)->get_x(),
					nodes.at(vx)->get_y()
					);

				if(!binary_search (polygon_nodes.begin(), polygon_nodes.end(), vx)
								 // looking around in a radius of 200 km among the grounds I know...
					&&  dist_to_this_node < 200)
				{
                    dout(cout  << "this node " << vx << " is actually outside the closed area: steam away!!!" << endl);
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

				dist_to_others.push_back(min_distance[vx]);

			}

		}
		else
		{

			dist_to_others.push_back(min_distance[vx]);

		}

	}
	vertex_t next_ground;
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
		next_ground = grds[idx_scdlowest];
        dout(cout  << "GO FISHING ON THE 2nd CLOSEST: " << vertex_names[next_ground] << endl);

		// check for area_closure
        if (dyn_alloc_sce.option(Options::area_closure))
		{

			vector<int> polygons;
			vector<int> polygon_nodes;
            for (multimap<int, int>::const_iterator pos=nodes_in_polygons.begin(); pos != nodes_in_polygons.end(); pos++)
			{
				// get all values across the keys
				polygons.push_back(pos->first);
				polygon_nodes.push_back(pos->second);
                dout(cout  << " a polygon node is " << pos->second << endl);
			}

			if(binary_search (polygon_nodes.begin(), polygon_nodes.end(), next_ground))
			{
                dout(cout  << "this NEXT node " << next_ground << " is actually within a closed area!!!" << endl);
				finally_I_should_go_for_the_closest = true;
				// the closest might also be inside the closed area but at least we will oscillate back to outside

			}
		}

	}

	if(!a_vect[0] || finally_I_should_go_for_the_closest)
	{
		next_ground = grds[idx_lowest];
        dout(cout  << "GO FISHING ON THE CLOSEST: " <<   vertex_names[next_ground] << endl);
		// in this case, get a time and fuel bonus for free! (i.e. note the MINUS sign!)
		// (in order to somehow correct for the discretisation creating jumps between sequential fgrounds)
		this->set_timeatsea(this->get_timeatsea() - PING_RATE);
		this->set_traveled_dist_this_trip(this->get_traveled_dist_this_trip() + this->get_speed() * PING_RATE * NAUTIC);
		double cumfuelcons = this->get_cumfuelcons() - this->get_fuelcons()*PING_RATE;
		this->set_cumfuelcons(cumfuelcons);
		this->set_consotogetthere( this->get_consotogetthere() - (this->get_fuelcons()*PING_RATE) ) ;
	}
	list<vertex_t> path = DijkstraGetShortestPathTo(next_ground, previous);
	path.pop_front();			 // delete the first node (departure) because we are lying in...

	if(path.empty())
	{
        dout(cout << this->get_name() << " when changing from "<< from << " to this new ground: " << next_ground << " you should stop here because my path is empty!");
		// as we detected something wrong here, we try to recover(!):
        for(unsigned int i=0; i<grds.size(); i++)
		{
			next_ground = grds[i];
            dout(cout << "then try to change for this new ground: " << next_ground << endl);
			path = DijkstraGetShortestPathTo(next_ground, previous);
			path.pop_front();	 // delete the first node (departure) because we are lying in...
			if(!path.empty())
			{
                dout(cout << "this one is all right." << endl);
				break;
			}
		}
	}

    if(path.empty())
    {
    // still empty!!
    outc(cout << "pble calculating from " << from << " to " << next_ground << endl);
    this->move_to(nodes.at(from)) ;
    // no path found: assume the vessel stucks at its current location
    } else{
        this->set_roadmap(path);
    }

    min_distance.clear();
	previous.clear();

    dout(cout  << "WELL...GO FISHING ON " << next_ground << endl);

    dout(cout  << "We change from "<< from << " to this new ground: " << next_ground << endl);
	//list<vertex_t>::iterator pos;
	//for(pos=path.begin(); pos!=path.end(); pos++)
	//{
    //    dout(cout << *pos << " ");
    //    dout(cout << endl);
	//}

	// for this vessel, select the metier specific to this particular fishing ground
	// according to the observed frequency in data
    const multimap<int, int> &poss_met        = this->get_possible_metiers();
    const multimap<int, double> &freq_poss_met= this->get_freq_possible_metiers();
	vector<int>    metiers_on_grd      = find_entries_i_i( poss_met, next_ground );
	vector<double> freq_metiers_on_grd = find_entries_i_d( freq_poss_met, next_ground );
								 // need to convert in array, see myRutils.cpp
	vector<int>    a_met = do_sample(1, metiers_on_grd.size(), &metiers_on_grd[0], &freq_metiers_on_grd[0]);
	this->set_metier(  metiers[ a_met[0] ]  );
	// find.next.pt.on.the.graph()
    this->find_next_point_on_the_graph_unlocked(nodes);
    unlock();
}


//------------------------------------------------------------//
//------------------------------------------------------------//
// related to individual choice of harbours...
//------------------------------------------------------------//
//------------------------------------------------------------//

void Vessel::choose_a_port_and_then_return(int tstep,
        const DynAllocOptions &dyn_alloc_sce,
        int create_a_path_shop,
        const vector<int> &idx_path_shop,
        const deque<map<vertex_t, vertex_t> > &path_shop,
        const deque<map<vertex_t, weight_t> > &min_distance_shop,
        adjacency_map_t& adjacency_map,
        map<vertex_t, weight_t>& min_distance,
        map<vertex_t, vertex_t>& previous,
        vector <int>& relevant_nodes,
        vector<string>& vertex_names,
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
    UNUSED(vertex_names);

    vector <int> harbs = this->get_harbours();
	int from = this->get_loc()->get_idx_node();
	dist_to_ports.clear();
	min_distance.clear();
	previous.clear();
	// get the shortest path between source and destination
	// with the list of intermediate nodes

	if(!create_a_path_shop)
	{
								 // from the source to all nodes
		DijkstraComputePaths(from, adjacency_map, min_distance, previous, relevant_nodes);
	}
	else						 // replaced by:
	{
        vector<int>::const_iterator it = find (idx_path_shop.begin(), idx_path_shop.end(), from);
								 // tricky!
		int idx = it - idx_path_shop.begin();

		previous=path_shop.at(idx);
		min_distance=min_distance_shop.at(idx);

	}

	// the 'arr' node will either be the closest port, or the more frequent port:
	vertex_t arr;

	//*************************closer_port**************************//
    if (dyn_alloc_sce.option(Options::closer_port))			 // dyn sce.
	{
		for (unsigned int i =0; i< harbs.size(); i++)
		{
								 // destination
			vertex_t vx = harbs[i];
            dout(cout  << "distance to harbour " << vertex_names[vx] << ": " << min_distance[vx] << endl);
			dist_to_ports.push_back(min_distance[vx]);
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
		arr = harbs[idx_lowest]; // destination: nearest port
	}
	else						 // ....otherwise (e.g. baseline), choose a harbour according to its frequency in data...
	{
		vector <double> freq_harbs = this->get_freq_harbours();
								 // need to convert in array, see myRutils.cpp
		vector<int> harbours = do_sample(1, harbs.size(), &harbs[0], &freq_harbs[0]);
		arr = harbours[0];
	}

    dout(cout  << "from " << from << endl);
    dout(cout  << "choose " << arr << endl);
	list<vertex_t> path = DijkstraGetShortestPathTo(arr, previous);

	// check the path
	// list<int>::iterator pos;
	//for(pos=path.begin(); pos!=path.end(); pos++)
	//{
    //    dout(cout << *pos << " ");
	//}
	//cout << endl;

	if(path.size()==1)			 // i.e no path has been found...
	{
        // fix the fact that, in very few cases, certain paths are not found in a given direction (likely to come from imcomplete adjacency matrix)
		// so tips: use the inversed path instead! i.e. 'from' begin 'arr' while 'arr' begin 'from'
		// and finally the path is inverted.

		if(!create_a_path_shop)
		{
			min_distance.clear();
			previous.clear();
								 // from the source to all nodes
			DijkstraComputePaths(arr, adjacency_map, min_distance, previous, relevant_nodes);
		}
		else					 // replaced by:
		{
            vector<int>::const_iterator it = find (idx_path_shop.begin(), idx_path_shop.end(), arr);
								 // tricky!
			int idx = it - idx_path_shop.begin();

			previous=path_shop.at(idx);
			min_distance=min_distance_shop.at(idx);

		}

		path = DijkstraGetShortestPathTo(from, previous);
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
            " going to arr " << arr << " from " << from << " then compute it..." << endl);

		//compute a path!  (24Feb14 but disabled because too time consuming!)
		//system("PAUSE");
		//min_distance.clear();
		//previous.clear();
		//DijkstraComputePaths(arr, adjacency_map, min_distance, previous, relevant_nodes); // from the source to all nodes
		//print_out=true;

        this->move_to(nodes.at(from)) ;
        // no path found: assume the vessel stucks at its current location


	}

	path.pop_front();			 // delete the first node (departure) because we are lying in...
	// (NOTE: if the 'arr' node is not in the 'previous' object of 'from',
	// then the path is only consistited of 'arr' and then will be blank here because removed by pop_front!!!! => bug)
	this->set_roadmap(path);
	min_distance.clear();
	previous.clear();
	if(print_out)
	{
        dout(cout << "new roadmap to port is: ");
		// check
		list<int>::iterator pos;
		list<vertex_t> lst = this->get_roadmap();
		for(pos=lst.begin(); pos!=lst.end(); pos++)
		{
            dout(cout << *pos << " ");
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
	// clear for this vessel
	this-> clear_catch_pop_at_szgroup();
	this-> set_inactive(true);
	// re-init some other stuffs after the trip
	this-> set_cumfuelcons(0);
	this-> set_consotogetthere(0);
	this-> set_cumsteaming(0);
	this-> set_cumcatches(0);
	this-> set_cumcatches(0);
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

int Vessel::should_i_go_fishing(int tstep, bool use_the_tree)
{

    lock();

	// first of all, check if some remaining quotas
	// note that, by default, only pops with informed global_tac_this_pop have non-zero individual_tac
	// which means that the vessel will stop going fishing if all the pops with informed tac have their tac exhausted
	// (while they might actually target other species?? caution!)
	int still_some_quotas=0;
	int nbpops = this->get_percent_tac_per_pop().size();
	for (int pop=0; pop < nbpops; pop++)
	{
								 // return the annual individual quota (!= the quota left), should be 0 if erased because no quota left
		double indiv_quota= this->get_individual_tac(pop);
		if(indiv_quota!=0)
		{
			still_some_quotas=1;
            dout(cout  << "this vessel " << this->get_name() << " have (still) quota for pop " << pop << ": " << indiv_quota << endl);
		}
	}

	// to avoid the vessel staying quayside while the vessel is actually not targeting the tac-informed species
	// we need a small fix:
    if(this->get_targeting_non_tac_pop_only())
        still_some_quotas=1;

    if( still_some_quotas)
	{

		// PBLE HERE FOR THE USE OF THE DECISION TREE: WHICH TIMING SHOULD WE USE?
        // take the decision to go once a day and at 7 a.m.

		// read the vessel-specific decision tree
		// (binary tree only, no/low/bad is 0 and yes/high/good is 1)
        if(use_the_tree && dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::GoFishing))
		{

            if((tstep % 24)==7)
            {
               boost::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::GoFishing);
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
			//    do not change whatever the scenario, which can in some cases be weird?)
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
        dout(cout  << "no quota left for this vessel " << this->get_name() << "...stay on quayside!" << endl);
        unlock();
		return(0);
	}

}






int Vessel::should_i_choose_this_ground(int tstep, vector<Node *> &nodes, const vector<int> &idx_path_shop,
                                        const deque<map<vertex_t, vertex_t> > &path_shop,
                                        const deque<map<vertex_t, weight_t> > &min_distance_shop)
{

        boost::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::ChooseGround);

        int idx=0; // idx of the relevant ground

        // keep tracks relevant nodes to evaluate
        vector <int> relevant_grounds_to_evaluate;
        int smartCatchGround;
        int highPotentialCatchGround;
        int notThatFarGround;
        int knowledgeOfThisGround;


        // 1. grounds of that vessel
        vector <int> grds= this->get_fgrounds();

        // 2. Pre-computing of the variable all grds together
        // (if the variable in the tree, and if this variable need computing from all grounds altogether)
        // e.g. for smartCatch or highPotentialCatch

          vector<int>  grds_in_closure = this->get_fgrounds_in_closed_areas();
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
           vector<double> expected_profit_per_ground = this->expected_profit_on_grounds(idx_path_shop,
                                                                                      path_shop,
                                                                                      min_distance_shop);

           // a check
           double expected_profit=0;
           for(unsigned int gr=0; gr<grds.size(); gr++)
               {
               expected_profit+=expected_profit_per_ground.at(gr);
               }
           //cout << "_The expected revenue for this vessel " <<
           //         this->get_name() << " is " << expected_profit << endl;
           if(expected_profit<0) cout << this->get_name() << ": NO PROFIT EXPECTED ON ALL GROUNDS FROM TARGET SPECIES!" << endl;
           // => IMAGINE A MEAN TO EXPAND THEIR RANGE....
           if(expected_profit<0)
              {
               // e.g. look at what use to do some vessels sharing the same departure harbour!
               //int a_node = this->get_previous_harbour_idx();
               //vector <int> grounds_from_harbours =node.at(a_node->get_grounds_from_harbours());
               //this->set_fgounds(grounds_from_harbours);
               //this->set_freq_fgounds(freq_grounds_from_harbours);
               // then, do it again:
                 expected_profit_per_ground = this->expected_profit_on_grounds(idx_path_shop,
                                                                             path_shop,
                                                                             min_distance_shop);
              }





           // keep only the grds out the closed areas...
            vector <int> grds_out;
            vector <double> expected_profit_per_ground_out;
            if(grds_in_closure.size()>0)
            {
                for (unsigned int i=0; i<grds.size();++i)
                {
                std::vector<int>::iterator it;
                it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
                if(it == grds_in_closure.end()) // not found
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
            this->set_smartcatch(-1);  // grounds are all included in closed areas...
            }
            dout(cout << "smartCatchGround is " << smartCatchGround << endl);

        }





        if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::highPotentialCatch) == true)
        {
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
           vector <int> grds_out2;
           vector <double> past_freq_cpue_grds_out;
           if(grds_in_closure.size()>0)
           {
               for (unsigned int i=0; i<grds.size();++i)
               {
               std::vector<int>::iterator it;
               it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
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
           this->set_highpotentialcatch(-1);  // grounds are all included in closed areas...
           }
           dout(cout << "highPotentialCatchGround is " << highPotentialCatchGround << endl);

        }

        if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::knowledgeOfThisGround) == true)
        {
          vector <double> freq_grounds = this->get_freq_fgrounds();

          // keep only the grds out the closed areas...
          vector <int> grds_out4;
          vector <double> freq_grounds_out;
          if(grds_in_closure.size()>0)
          {
              for (unsigned int i=0; i<grds.size();++i)
              {
              std::vector<int>::iterator it;
              it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
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
          this->set_mosthistoricallyused(-1);  // grounds are all included in closed areas...
          }
          dout(cout << "knowledgeOfThisGround is " << knowledgeOfThisGround << endl);

         }

        if(dtree::DecisionTreeManager::manager()->hasTreeVariable(dtree::DecisionTreeManager::ChooseGround, dtree::notThatFar) == true)
        {
          int from = this->get_loc()->get_idx_node();
          vector <double> distance_to_grounds = compute_distance_fgrounds
                                                   (idx_path_shop,
                                                   path_shop, min_distance_shop, from, grds);

          // keep only the grds out the closed areas...
          vector <int> grds_out3;
          vector <double> distance_to_grounds_out;
          if(grds_in_closure.size()>0)
          {
              for (unsigned int i=0; i<grds.size();++i)
              {
              std::vector<int>::iterator it;
              it=find (grds_in_closure.begin(), grds_in_closure.end(), grds.at(i));
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
          this->set_notthatfar(-1);  // grounds are all included in closed areas...
          }
          dout(cout << "notThatFarGround is " << notThatFarGround << endl);
          }




        // 3. traverseDTree for each possible ground (??: is this realistic??)
        int ground=-1;
        //random_shuffle(grds.begin(),grds.end()); // random permutation i.e. equal frequency of occurence
        for (int it=0; it < relevant_grounds_to_evaluate.size(); ++it){
            ground=relevant_grounds_to_evaluate.at(it);
            dout(cout << "Evaluate for ground... "<< ground << endl);

            // Caution with a logical leaks: each ground is evaluated once and only once,
            // meanwhile the smartCatch ground can be likely the same than the highPotentialCatch, notThatFar, etc.
            // so most of the time the tree need to be COMPLETE or FULL to account for conditional probabilities...

            // evaluators should evaluate if yes/no the ground a smartCatch ground etc.:
         // e.g.

            //"smartCatch",          // ChooseGround             => find if that ground is where the highest expected profit is
            //"highPotentialCatch",          // ChooseGround     => find if that ground is where the highest experienced CPUE (all species) occurred
            //"notThatFar",          // ChooseGround             => find if that ground is the closest one
            //"knowledgeOfThisGround",          // ChooseGround  => look at the historic proba of visiting the grounds and pick up the most frequented ground
            //"riskOfBycatchIs",          // ChooseGround        => find proportion on sites of juveniles or other non-targeted species and pick up the lowest
            //"saveFuel"                 // ChooseGround         => TO DO: find the highest expected profit among the XX closests
            //"isInAreaClosure"      // ChooseGround             => find if that ground is lying inside the closed polygons
            //=> TO DO: add the corresponding dtree evaluators...

            // cout << "traverse tree for ground " << ground << endl;
            double the_value = traverseDtree(ground, tree.get());


            //CHOOSE THAT GROUND!
            if(unif_rand()<the_value) {
                unlock();
                return(ground);
            }
            //  else // CONTINUE SEARCHING AMONG RELEVANT GROUNDS

        }

 // if here, then no ground has actually been found
        // (because of 1- a *non-complete* tree;
        // or 2- the node present into two or more relevant nodes at the mean time e.g smartCatch is also notThatFar)
        cout << "no one among relevant grounds......take the last ground evaluated... "<< ground << endl;
        if(relevant_grounds_to_evaluate.size()>0) return (ground);

        // ultimately, use the freq_fgrounds...(e.g. when all nodes are in closed areas)
        vector <double> freq_grds = this->get_freq_fgrounds();
                                  // need to convert in array, see myRutils.cpp
        double cumul=0.0;
        for(unsigned int n=0; n<grds.size(); n++)
        {
            if (binary_search (grds_in_closure.begin(), grds_in_closure.end(), grds.at(n)))
            {
                freq_grds.at(n)=0.00000000000001; // to avoid removing if nb of grounds outside is 0
            }
            cumul += freq_grds.at(n);
        }
            // then re-scale to 1
        for(unsigned int n=0; n<grds.size(); n++)
        {
            freq_grds.at(n)= freq_grds.at(n)/cumul;
        }
        // then sample...
        vector<int> grounds = do_sample(1, grds.size(), &grds[0], &freq_grds[0]);
        ground=grounds[0];
  return(ground);

}


int Vessel::should_i_start_fishing(map<string,int>& external_states, bool use_the_tree)
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


int Vessel::should_i_change_ground(map<string,int>& external_states, bool use_the_tree)
{
    UNUSED(external_states);

	if(use_the_tree)
	{
        return -1;
	}
	else
	{
        lock();

		// DEFAULT-------------------------
		vector <bool> a_vect;
		a_vect.push_back(true);
		a_vect.push_back(false);
		a_vect.push_back(false);
		random_shuffle(a_vect.begin(),a_vect.end());
		bool another_ground = a_vect[0] &&
			this->get_fgrounds().size()>2 &&
			this->get_nbfpingspertrip() > 1;

        unlock();
		return(another_ground);
	}
}


int Vessel::should_i_stop_fishing(const map<string,int>& external_states, bool use_the_tree,
                                  int tstep,
                                  const DynAllocOptions& dyn_alloc_sce,
                                  int create_a_path_shop,
                                  const vector <int>& idx_path_shop,
                                  const deque<map<vertex_t, vertex_t> >& path_shop,
                                  const deque<map<vertex_t, weight_t> >& min_distance_shop,
                                  adjacency_map_t& adjacency_map,
                                  map<vertex_t, weight_t>& min_distance,
                                  map<vertex_t, vertex_t>& previous,
                                  const vector <int>& relevant_nodes,
                                  vector<string>& vertex_names,
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
    UNUSED(vertex_names);

    if(use_the_tree && dtree::DecisionTreeManager::manager()->hasTree(dtree::DecisionTreeManager::StopFishing))
        {

            boost::shared_ptr<dtree::DecisionTree> tree = dtree::DecisionTreeManager::manager()->tree(dtree::DecisionTreeManager::StopFishing);
            double the_value = traverseDtree(tstep, tree.get());


            //STOP FISHING!
            if(unif_rand()<the_value) {
                unlock();
                return(1);
            } else {
                unlock();
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
            const vector <int> &harbs = this->get_harbours();
			int from = this->get_loc()->get_idx_node();
			min_distance.clear();
			previous.clear();

			if(!create_a_path_shop)
			{
								 // from the source to all nodes
				DijkstraComputePaths(from, adjacency_map, min_distance, previous, relevant_nodes);
			}
			else				 // replaced by:
			{
                vector<int>::const_iterator it = find (idx_path_shop.begin(), idx_path_shop.end(), from);
								 // tricky!
				int idx = it - idx_path_shop.begin();

				previous=path_shop.at(idx);
				//std::list<map<int,int> >::iterator it_p = path_shop.begin();
				//advance(it_p, idx-1);
				//previous= *it_p;

				min_distance=min_distance_shop.at(idx);
				//std::list<map<int,int> >::iterator it_d = min_distance_shop.begin();
				//advance(it_d, idx-1);
				//min_distance= *it_d;

			}

			for (unsigned int i =0; i< harbs.size(); i++)
			{
				// get the shortest distance between source and destination
				// with the list of intermediate nodes
								 // destination
				vertex_t vx = harbs[i];
                dout(cout  << "distance to harbour " << vertex_names[vx] << ": " << min_distance[vx] << endl);
				dist_to_ports.push_back(min_distance[vx]);
			}
			min_distance.clear();
			previous.clear();
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
