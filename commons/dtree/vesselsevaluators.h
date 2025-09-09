#ifndef VESSELSEVALUATORS_H
#define VESSELSEVALUATORS_H

#include <Vessel.h>
#include <Node.h>
#include <helpers.h>
#include <numeric> // std::accumulate()

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
      //cout << vessel->get_name() << " with length " << vessel->get_length() << " is of length class " <<  (double)vessel->get_length_class() << "\n";
      return static_cast<float>(vessel->get_length_class()) / VariableNames::variableBinCount(Variable::vesselSizeIs);
    }
};

class VesselTargetEvaluator : public dtree::StateEvaluator {
public:
    VesselTargetEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
        vector<int> trgts= vessel->get_metier()->get_metier_target_stocks();
        return trgts.at(0)  / VariableNames::variableBinCount(Variable::stockTargetIs); //WIP: should account for all the targets!
    }
};


class MetierStateEvaluator : public dtree::StateEvaluator {
public:
    MetierStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
        return static_cast<float>(vessel->get_metier()->get_name()) / VariableNames::variableBinCount(Variable::vesselMetierIs);
    }
};


class VesselTodayIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    static constexpr double WorkDay = 0.0;
    static constexpr double WeekEndDay = 1.0;

    VesselTodayIsStateEvaluator() {}
    double evaluate(int tstep, Vessel *vessel) const {
        int wday = (tstep / 24) % 7;
        if (vessel->getWeekEndStartDay() > vessel->getWeekEndEndDay()) {
            if (vessel->getWeekEndEndDay() < wday && wday < vessel->getWeekEndStartDay())
                return WorkDay;
            return WeekEndDay;
        } else {
            if (vessel->getWeekEndEndDay() >= wday && wday >= vessel->getWeekEndStartDay())
                return WeekEndDay;
            return WorkDay;
        }
    }
};

class VesselDayIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    static constexpr double FirstWorkDay = 0.0;
    static constexpr double RestOfTheWeekDay = 1.0;

    VesselDayIsStateEvaluator() {}
    double evaluate(int tstep, Vessel* vessel) const {
        int wday = (tstep / 24) % 7;
            if (wday < 1)
                return FirstWorkDay;
            return RestOfTheWeekDay;
    }
};

class VesselFridayIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    static constexpr double Friday = 0.0;
    static constexpr double RestOfTheWeekDay = 1.0;

    VesselFridayIsStateEvaluator() {}
    double evaluate(int tstep, Vessel* vessel) const {
        int wday = (tstep / 24) % 7;
        if (wday ==4)
            return Friday;
        return RestOfTheWeekDay;
    }
};

class VesselMonthIsStateEvaluator : public dtree::StateEvaluator {
private:
public:

    VesselMonthIsStateEvaluator() {}
    double evaluate(int tstep, Vessel *vessel) const {
        UNUSED(vessel);
        return ((tstep/720) % 12) / 11.0;
    }
};


class VesselindividualQuotaLeftOnAvoidedStksNowIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselindividualQuotaLeftOnAvoidedStksNowIsStateEvaluator() {}
    double evaluate(int, Vessel *v) const {
        double min_quota_left_among_avoided_stks = v->get_min_prop_remaining_individual_quotas_on_avoided_stks();
        return  min_quota_left_among_avoided_stks < 0.1 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the individual quotas (for avoided species) left is low
        }
};

class VesselglobalQuotaLeftOnAvoidedStksNowIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselglobalQuotaLeftOnAvoidedStksNowIsStateEvaluator() {}
    double evaluate(int, Vessel *v) const {
        double min_quota_left_among_avoided_stks = v->get_min_prop_remaining_global_quotas_on_avoided_stks();
//        cout << "min_quota_left_among_avoided_stks is " << min_quota_left_among_avoided_stks;
        return  min_quota_left_among_avoided_stks < 0.1 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the global quotas (for avoided species) left is low
        }
};

class VesselRiskOfBycatchAvoidedStksNowIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselRiskOfBycatchAvoidedStksNowIsStateEvaluator() {}
    double evaluate(int, Vessel *v) const {
        //cout << "vessel specific risk of bycatch being evaluated before trip start..." << "\n";
        vector <double> prop_bycatch = v->get_experienced_avoided_stks_bycatch_prop_on_fgrounds();
        double average_prop_bycatch= std::accumulate( prop_bycatch.begin(), prop_bycatch.end(), 0.0)/prop_bycatch.size();
        //cout << "...the average discard ratio for this vessel is: " << average_prop_bycatch << "\n";
        return  average_prop_bycatch > 0.2 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the vessel has experienced large bycatch (>20%) on this ground?
        }
};



class VesselFuelTankStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselFuelTankStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
         int exit= vessel->get_cumfuelcons() > (vessel->get_tankcapacity() * 0.5); //0: "still ok" node; 1: "low"
         if (exit) 
         { 
             vessel->set_reason_to_go_back(2);
         }
        // else
        // {
        //     vessel->set_reason_to_go_back(-2);
        // }
         return exit==1 ? 1.0 : 0.0; //0: "still ok" node; 1: "low"
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
          int exit= vessel->get_cumcatches() > (vessel->get_carrycapacity() * 0.9); 
          if(exit)
          { 
              vessel->set_reason_to_go_back(3);
          }
       //  else
       //   {
       //     vessel->set_reason_to_go_back(-3);
       //  }
         return exit ? 1.0 : 0.0; //0: "still ok" node; 1: "fullfilled"
    }
};


class VesselNbOfDaysAtSeaSoFarIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselNbOfDaysAtSeaSoFarIsStateEvaluator() {}
    double evaluate(int, Vessel *vessel) const {
          return vessel->get_timeatsea() > 5*24  ? 1.0 : 0.0; //0:<5days ; 1:>5days
        }
};

class VesselMoreThan3DaysAfterFirstCatchIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselMoreThan3DaysAfterFirstCatchIsStateEvaluator() {}
    double evaluate(int, Vessel* vessel) const {
        return vessel->get_timeatseasincefirstcatch() > 3 * 24 ? 1.0 : 0.0; //0:<3days ; 1:>3days
    }
};




class VesselEndOfTheDayIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselEndOfTheDayIsStateEvaluator() {}
    double evaluate(int tstep, Vessel *vessel) const {
              int current_hour= (int)((tstep % 24)+0.5);
              // caution: we expect getWorkDayEndHour to be within 0 and 23...(i.e. no 24)
              int stop_fishing =
                  (vessel->getWorkDayEndHour() == 23 && current_hour == vessel->getWorkDayEndHour() - 1) ||  // end of day is true if +/- 1 hour of the usual return hour (make flexible because the vessel might be steaming at the exact hour then not taking any stop fishing decision...)
                  (current_hour == vessel->getWorkDayEndHour()) ||
                  (current_hour == vessel->getWorkDayEndHour() + 1) ||
                  (vessel->get_timeatsea() > 20); // worst case, end of the day triggered as soon as trying to fish again within the next day...
              if (stop_fishing)
              { 
                  vessel->set_reason_to_go_back(1);
              }
        // else 
        //{
        // vessel->set_reason_to_go_back(-1);
        // }
         return stop_fishing ? 0.0 : 1.0; //0: "true" node i.e. stop fishing; 1: "false"
          }
};


class VesselIsInAreaClosureEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselIsInAreaClosureEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
          auto lst_fgrounds_in_closed_areas=v->get_fgrounds_in_closed_areas();
          auto it= find (lst_fgrounds_in_closed_areas.begin(), lst_fgrounds_in_closed_areas.end(), types::NodeId(fground));
          bool isIt= (it != lst_fgrounds_in_closed_areas.end()); // found
          //cout << "isinareaclosure on this ground evaluated at "  << isIt << "\n";
          return  isIt ? 1.0 : 0.0; // Is yes or no in closed area?

        }
};


class VesselSmartCatchStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselSmartCatchStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isSmart= (types::NodeId(fground)==v->get_smartcatch());
        //cout << "smartcatch on this ground evaluated at " <<  isSmart << "\n";
        return  isSmart ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the tested ground a smart catch?
        }
};


class VesselHighPotentialCatchStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselHighPotentialCatchStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isHighPotential=(types::NodeId(fground)==v->get_highpotentialcatch());
        //cout << "highpotentialcatch on this ground evaluated at " << isHighPotential<< "\n";
        return   isHighPotential ? 1.0 : 0.0; // Is yes or no the tested ground the highest cpue ground?
        }
};

class VesselNotThatFarStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselNotThatFarStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isNotFar = (types::NodeId(fground)==v->get_notthatfar());
        //cout << "notthatfar on this ground evaluated at " << isNotFar << "\n";
        return  isNotFar ? 1.0 : 0.0; // Is yes or no the closest ground?
        }
};

class VesselKnowledgeOfThisGroundStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselKnowledgeOfThisGroundStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isWellKnown = (types::NodeId(fground)==v->get_mosthistoricallyused());
        //cout << "mosthistoricallyused on this ground evaluated at " << isWellKnown << "\n";
        return  isWellKnown ? 1.0 : 0.0; // Is yes or no the ground has been the most historically frequent?
        }
};

class VesselRiskOfBycatchAllStksIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselRiskOfBycatchAllStksIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        auto the_grds = v->get_fgrounds();
        int idx_node_r= find(the_grds.begin(), the_grds.end(), types::NodeId(fground)) - the_grds.begin();    // relative node index to this vessel
        //cout << "risk of bycatch on this ground being evaluated..." << "\n";
        vector <double> prop_bycatch = v->get_experienced_bycatch_prop_on_fgrounds();
        //cout << "...the discard ratio for that ground is: " << prop_bycatch.at(idx_node_r) << "\n";
        return  prop_bycatch.at(idx_node_r) > 0.2 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the vessel has experienced large bycatch (>20%) on this ground?
        }
};


class VesselRiskOfBycatchAvoidedStksIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselRiskOfBycatchAvoidedStksIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        auto the_grds = v->get_fgrounds();
        int idx_node_r= find(the_grds.begin(), the_grds.end(), types::NodeId(fground)) - the_grds.begin();    // relative node index to this vessel
        //cout << "risk of bycatch on this ground being evaluated..." << "\n";
        vector <double> prop_bycatch = v->get_experienced_avoided_stks_bycatch_prop_on_fgrounds();
        //cout << "...the discard ratio for that ground is: " << prop_bycatch.at(idx_node_r) << "\n";
        return  prop_bycatch.at(idx_node_r) > 0.2 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the vessel has experienced large bycatch (>20%) on this ground?
        }
};

class VesselindividualQuotaLeftOnAvoidedStksIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselindividualQuotaLeftOnAvoidedStksIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        double min_quota_left_among_avoided_stks = v->get_min_prop_remaining_individual_quotas_on_avoided_stks();
        return  min_quota_left_among_avoided_stks < 0.1 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the individual quotas (for avoided species) left is low
        }
};

class VesselglobalQuotaLeftOnAvoidedStksIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselglobalQuotaLeftOnAvoidedStksIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        double min_quota_left_among_avoided_stks = v->get_min_prop_remaining_global_quotas_on_avoided_stks();
//        cout << "min_quota_left_among_avoided_stks is " << min_quota_left_among_avoided_stks;
        return  min_quota_left_among_avoided_stks < 0.1 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the global quotas (for avoided species) left is low
        }
};

class VesselLowestTariffStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselLowestTariffStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool LowestTariff = (types::NodeId(fground)==v->get_lowesttariff());
        //cout << "lowesttariff on this ground evaluated at " << LowestTariff << "\n";
        return  LowestTariff ? 1.0 : 0.0; // Is yes or no the closest ground?
        }

};

class VesselTariffThisGroundIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselTariffThisGroundIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        //auto the_grds = v->get_fgrounds();
        //int idx_node_r= find(the_grds.begin(), the_grds.end(), types::NodeId(fground)) - the_grds.begin();    // relative node index to this vessel
        //cout << "Tariff on this ground being evaluated..." << "\n";
        vector <double> tariffs_over_layers = v->get_map_of_nodes().at(fground)->get_tariffs(); // using the superpower of omniscience (which is anyway quite expected on tariffs!)
        //cout << "...the overall tariff for that ground is: " << tariffs_over_layers.at(0) << "\n";
        return  tariffs_over_layers.at(0) >= 5 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf)  somewhat high tariff on this ground?
    }
};




// StartFishing dtree
class VesselRiskOfBycatchAvoidedStksHereIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselRiskOfBycatchAvoidedStksHereIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        auto the_grds = v->get_fgrounds();
        int idx_node_r= find(the_grds.begin(), the_grds.end(), types::NodeId(fground)) - the_grds.begin();    // relative node index to this vessel
        //cout << "risk of bycatch on this ground being evaluated..." << "\n";
        vector <double> prop_bycatch = v->get_experienced_avoided_stks_bycatch_prop_on_fgrounds();
        //cout << "...the discard ratio for that ground is: " << prop_bycatch.at(idx_node_r) << "\n";
        return  prop_bycatch.at(idx_node_r) > 0.2 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the vessel has experienced large bycatch (>20%) on this ground?
        }
};



class VesselsuitableBottomTypeIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselsuitableBottomTypeIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        int currentbottom = v->get_loc()->get_marine_landscape();
        vector<int> suitablebottoms = v->get_metier()->get_metier_suitable_seabottomtypes();
        return binary_search(suitablebottoms.begin(), suitablebottoms.end(), currentbottom) ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf)
        }
};

class VesselindividualQuotaLeftOnAvoidedStksHereIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselindividualQuotaLeftOnAvoidedStksHereIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        double min_quota_left_among_avoided_stks = v->get_min_prop_remaining_individual_quotas_on_avoided_stks();
        return  min_quota_left_among_avoided_stks < 0.1 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the individual quotas (for avoided species) left is low
        }
};

class VesselglobalQuotaLeftOnAvoidedStksHereIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselglobalQuotaLeftOnAvoidedStksHereIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        double min_quota_left_among_avoided_stks = v->get_min_prop_remaining_global_quotas_on_avoided_stks();
//        cout << "min_quota_left_among_avoided_stks is " << min_quota_left_among_avoided_stks;
        return  min_quota_left_among_avoided_stks < 0.1 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the global quotas (for avoided species) left is low
        }
};


class VesselotherVesselFishingHereStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselotherVesselFishingHereStateEvaluator() {}
    double evaluate(int fground, Vessel* v) const {
        vector<int> vids_already_fishing_there = v->get_loc()->get_vid();
        //        cout << "nb vessels already fishing there is " << min_quota_left_among_avoided_stks;
        return  vids_already_fishing_there.size() > 3 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the global quotas (for avoided species) left is low
    }
};




// ChangeGround
class VesselFeelingForCatchingElsewhereStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselFeelingForCatchingElsewhereStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        double min_quota_left_among_avoided_stks = v->get_min_prop_remaining_global_quotas_on_avoided_stks();
//        cout << "min_quota_left_among_avoided_stks is " << min_quota_left_among_avoided_stks;
        return  min_quota_left_among_avoided_stks < 0.1 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the global quotas (for avoided species) left is low
        }
};

class VesselSeeingOtherVesselFishingElsewhereStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselSeeingOtherVesselFishingElsewhereStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        double min_quota_left_among_avoided_stks = v->get_min_prop_remaining_global_quotas_on_avoided_stks();
//        cout << "min_quota_left_among_avoided_stks is " << min_quota_left_among_avoided_stks;
        return  min_quota_left_among_avoided_stks < 0.1 ? 1.0 : 0.0; // Is yes (right leaf) or no (left leaf) the global quotas (for avoided species) left is low
        }
};






}
}

#endif // VESSELSEVALUATORS_H
