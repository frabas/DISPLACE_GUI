#ifndef VESSELSEVALUATORS_H
#define VESSELSEVALUATORS_H

#include <Vessel.h>
#include <helpers.h>

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

class VesselMonthIsStateEvaluator : public dtree::StateEvaluator {
private:
public:

    VesselMonthIsStateEvaluator() {}
    double evaluate(int tstep, Vessel *vessel) const {
        UNUSED(vessel);
        return ((tstep/720) % 12) / 11.0;
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
    double evaluate(int tstep, Vessel *vessel) const {
          return (tstep % 24) < vessel->getWorkDayEndHour() ? 1.0 : 0.0; // hardcoded return for daily trip after 22 p.m. //0: "true" node; 1: "false"
        }
};



class VesselSmartCatchStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselSmartCatchStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isSmart= (types::NodeId(fground)==v->get_smartcatch());
        //cout << "smartcatch on this ground evaluated at " <<  isSmart << endl;
        return  isSmart ? 1.0 : 0.0; // Is yes or no the tested ground a smart catch?
        }
};


class VesselHighPotentialCatchStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselHighPotentialCatchStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isHighPotential=(types::NodeId(fground)==v->get_highpotentialcatch());
        //cout << "highpotentialcatch on this ground evaluated at " << isHighPotential<< endl;
        return   isHighPotential ? 1.0 : 0.0; // Is yes or no the tested ground the highest cpue ground?
        }
};

class VesselNotThatFarStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselNotThatFarStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isNotFar = (types::NodeId(fground)==v->get_notthatfar());
        //cout << "notthatfar on this ground evaluated at " << isNotFar << endl;
        return  isNotFar ? 1.0 : 0.0; // Is yes or no the closest ground?
        }
};

class VesselKnowledgeOfThisGroundStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselKnowledgeOfThisGroundStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        bool isWellKnown = (types::NodeId(fground)==v->get_mosthistoricallyused());
        //cout << "mosthistoricallyused on this ground evaluated at " << isWellKnown << endl;
        return  isWellKnown ? 1.0 : 0.0; // Is yes or no the ground has been the most historically frequent?
        }
};

class VesselRiskOfBycatchIsStateEvaluator : public dtree::StateEvaluator {
private:
public:
    VesselRiskOfBycatchIsStateEvaluator() {}
    double evaluate(int fground, Vessel *v) const {
        auto the_grds = v->get_fgrounds();
        int idx_node_r= find(the_grds.begin(), the_grds.end(), types::NodeId(fground)) - the_grds.begin();    // relative node index to this vessel
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
          auto lst_fgrounds_in_closed_areas=v->get_fgrounds_in_closed_areas();
          auto it= find (lst_fgrounds_in_closed_areas.begin(), lst_fgrounds_in_closed_areas.end(), types::NodeId(fground));
          bool isIt= (it != lst_fgrounds_in_closed_areas.end()); // found
          //cout << "isinareaclosure on this ground evaluated at "  << isIt << endl;
          return  isIt ? 1.0 : 0.0; // Is yes or no in closed area?

        }
};


}
}

#endif // VESSELSEVALUATORS_H
