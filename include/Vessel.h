// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef VESSEL_H
#define VESSEL_H

#include "commons_global.h"

#include "SimModel.h"
#include "vesselcalendar.h"
#include "myutils.h"
#include "options.h"
#include "dtree/decisiontree.h"
#include "dtree/stateevaluator.h"
#include "shortestpath/GeoGraph.h"
#include "SparseContainer.h"
#include "ValuesPerMetPerPopContainer.h"

#include <mutex>
#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <map>


typedef types::NodeId::type vertex_t;

class Node;
class Metier;
class Population;
class Benthos;

class Vessel {
public:
    enum LengthClass {
        Under15 = 0, Between15and18, Between18and24, Between24and40, Over40,
        NumLenghClasses
    };

    using ValueFgroundsPerMetPerPop = ValuesPerMetPerPopContainer;
    using ValueFgroundsPerYearQuarterPerPop = std::vector<std::vector<std::vector<double> > >;
    using ValuePerFgroundPerPop = std::vector<std::vector<double> >;
    using CumEffortPerTripPerFgroundsPerMet = std::vector<std::vector<double> >;

private:
    std::string name;
    int vid_is_active;
    int is_vessel_exited;
    int vid_is_part_of_ref_fleet;
    int idx_vessel;
    int firm_id;
    //std::shared_ptr<Node> m_location;
    Node *m_location;
    std::vector<Node *> map_of_nodes;
    Metier *metier;
    std::vector<types::NodeId> fgrounds;     // idx nodes for fishing grounds specific to this vessel
    std::vector<types::NodeId> fgrounds_init;     // fishing grounds specific to e.g. port shared
    std::vector<types::NodeId> fgrounds_in_closed_areas;     // idx nodes
    std::vector<types::NodeId> harbours;     // idx nodes
    // freq of visit per node
    std::vector<double> freq_fgrounds;
    // freq of visit per node
    std::vector<double> freq_fgrounds_init;
    // freq of visit per node
    std::vector<double> freq_harbours;
    // cumulated catch per node for the last trip, reinit when new trip start
    std::vector<double> cumcatch_fgrounds;
    std::vector<double> cumdiscard_fgrounds;
    std::vector<double> experienced_bycatch_prop_on_fgrounds;
    std::vector<double> experienced_avoided_stks_bycatch_prop_on_fgrounds;

    ValueFgroundsPerYearQuarterPerPop cumcatch_fgrounds_per_yearquarter_per_pop;
    ValuePerFgroundPerPop cumdiscard_fgrounds_per_pop;

    // cumulated effort per node for the last trip, reinit when new trip start
    std::vector<double> cumeffort_per_trip_per_fgrounds;
    CumEffortPerTripPerFgroundsPerMet cumeffort_per_trip_per_fgrounds_per_met;
    // cumulated effort per node for the last yearquarter, reinit when new quarter start
    std::vector<double> cumeffort_per_yearquarter_per_fgrounds;

    std::vector<double> experiencedcpue_fgrounds;
    std::vector<double> freq_experiencedcpue_fgrounds;
    // from cumcatch/cumeffort for last trip scaled to 1

    ValuePerFgroundPerPop cumcatch_fgrounds_per_pop;
    ValuePerFgroundPerPop experiencedcpue_fgrounds_per_pop;
    ValuePerFgroundPerPop freq_experiencedcpue_fgrounds_per_pop;

//        std::vector< std::vector< std::vector<double> > > cumcatch_fgrounds_per_met_per_pop;
    ValueFgroundsPerMetPerPop cumcatch_fgrounds_per_met_per_pop;

    ValueFgroundsPerMetPerPop experiencedcpue_fgrounds_per_met_per_pop;
    ValueFgroundsPerMetPerPop freq_experiencedcpue_fgrounds_per_met_per_pop;

    ValueFgroundsPerYearQuarterPerPop experiencedcpue_fgrounds_per_yearquarter_per_pop;
    ValueFgroundsPerYearQuarterPerPop freq_experiencedcpue_fgrounds_per_yearquarter_per_pop;

    // skipper effect, target factor:
    std::vector<double> vessel_betas_per_pop;
    //
    std::vector<double> percent_tac_per_pop;
    // clear at the end of the trip
    std::vector<int> idx_used_metiers_this_trip;
    // list of possible metiers on specific ground
    std::multimap<types::NodeId, int> possible_metiers;
    // freq of the metier on specific fground
    std::multimap<types::NodeId, double> freq_possible_metiers;
    std::vector<std::vector<double> > cpue_nodes_species;
    std::vector<std::vector<double> > gshape_cpue_nodes_species;
    std::vector<std::vector<double> > gscale_cpue_nodes_species;
    std::list<types::NodeId> roadmap;
    // because movement is semi-continuous...
    double x, y, prev_x, prev_y, next_x, next_y, course;
    // fixed
    double speed, fuelcons, length, KW, tankcapacity, carrycapacity, nbfpingspertrip, resttime_par1, resttime_par2, av_trip_duration;
    // dynamic
    double timeforrest, cumfuelcons, consotogetthere, cumsteaming, distprevpos, timeatsea, timeatseasincefirstcatch, traveled_dist_this_trip;
    double areasweptthistrip, subsurfaceareasweptthistrip, cumcatches, cumdiscards, reason_to_go_back;
    double mult_fuelcons_when_steaming, mult_fuelcons_when_fishing, mult_fuelcons_when_returning, mult_fuelcons_when_inactive;
    int hasfishedatleastonce;

    double this_vessel_nb_crew, annual_other_income;
    double landing_costs_percent, crewshare_and_unpaid_labour_costs_percent, other_variable_costs_per_unit_effort, annual_insurance_costs_per_crew;
    double standard_labour_hour_opportunity_costs, standard_annual_full_time_employement_hours, other_annual_fixed_costs;
    double vessel_value, annual_depreciation_rate, opportunity_interest_rate, annual_discount_rate;

    int length_class;
    std::string nationality;
    int nationality_idx;
    LengthClass mLengthClassId;
    int message;
		int state;
		int tstep_dep;
        types::NodeId previous_harbour_idx = types::special::InvalidNodeId;

        double lastTrip_revenues;
        double lastTrip_explicit_revenues;
        double lastTrip_profit;
        double avgRevenues;
        double avgProfit;
        int numTrips=0;

        // AER Economic Indicators (updated after each new trip)
        double TotLandingIncome=0; //cumul
        double TotHoursAtSea=0; //cumul
        double TotFuelCosts=0; //cumul
        double TotVarCosts=0;  //cumul
        double GVA=0; //updated
        double GVAPerRevenue=0; //updated
        double LabourSurplus=0; //updated
        double GrossProfit=0; //updated
        double NetProfit=0; //updated
        double NetProfitMargin=0; //updated
        double GVAPerFTE=0; //updated
        double RoFTA=0; //updated
        double BER=0; //updated
        double CRBER=0; //updated
        double NetPresentValue=0; //updated

        std::vector <double> daysSpentInRestrictedAreaThisMonth;

                                         // dynamic
		bool inharbour, inactive, natio;
        std::vector < std::vector<double> > catch_pop_at_szgroup;
        std::vector < std::vector<double> > ping_catch_pop_at_szgroup;
        std::vector < std::vector<double> > discards_pop_at_szgroup;
        std::vector<int> individual_tac_per_pop;
        std::vector<int> individual_tac_per_pop_at_year_start;
        std::vector<double> prop_remaining_individual_quotas;
        std::vector<double> prop_remaining_global_quotas;
        std::vector<double> fishing_credits;
		int targeting_non_tac_pop_only;

        std::vector<double> is_choked;

        VesselCalendar calendar;

        types::NodeId smartcatch;
        types::NodeId highpotentialcatch, notthatfar, lowesttariff, mosthistoricallyused; // some relevant grounds

        double effort_multiplier;  //for EffortControl HCR

        std::mutex mutex;

protected:
        void init();
        void find_next_point_on_the_graph_unlocked(std::vector<Node* >& nodes, int a_tstep, bool is_fishing_credits);

public:
        //Vessel(std::string name,  std::shared_ptr<Node> a_location);
        Vessel(std::string name,  Node *a_location);
        //Vessel(std::shared_ptr<Node> a_location, int idx_vessel, std::string name);
        Vessel(Node* a_location, int idx_vessel, std::string name);
        //Vessel(std::shared_ptr<Node> a_location, int idx_vessel, std::string name, int nbpops, int nbszgroups, std::vector<int> harbours, std::vector<int> fgrounds,
        Vessel(vector<string> dyn_alloc_sce, 
               Node* a_location,
               int idx_vessel,
               std::string name,
               int nbpops, int nbmets, int nbszgroups,
               const vector<types::NodeId> &harbours,
               const std::vector<types::NodeId> &fgrounds,
               const std::vector<types::NodeId> &fgrounds_init,
               const std::vector<double> &freq_harbours,
               const std::vector<double> &freq_fgrounds,
               const std::vector<double> &freq_fgrounds_init,
               const std::vector<double> &vessel_betas_per_pop,
               const std::vector<double> &percent_tac_per_pop,
               const multimap<types::NodeId, int> &possible_metiers,
               const multimap<types::NodeId, double> &freq_possible_metiers,
               const multimap<types::NodeId, double> &gshape_cpue_per_stk_on_nodes,
               const multimap<types::NodeId, double> &gscale_cpue_per_stk_on_nodes,
               int vid_is_active, int vid_is_part_of_ref_fleet,
               double speed, double fuelcons, double length, double KW,
               double  carrycapacity, double tankcapacity, double nbfpingspertrip,
               double resttime_par1, double resttime_par2, double av_trip_duration,
               double mult_fuelcons_when_steaming, double mult_fuelcons_when_fishing,
               double mult_fuelcons_when_returning, double mult_fuelcons_when_inactive,
               int _firm_id,
               VesselCalendar cd,
               double this_vessel_nb_crew,
               double annual_other_income,
               double landing_costs_percent,
               double crewshare_and_unpaid_labour_costs_percent,
               double other_variable_costs_per_unit_effort,
               double annual_insurance_costs_per_crew,
               double standard_labour_hour_opportunity_costs,
               double standard_annual_full_time_employement_hours,
               double other_annual_fixed_costs,
               double vessel_value,
               double annual_depreciation_rate,
               double opportunity_interest_rate,
               double annual_discount_rate
            );
		Vessel();
		~Vessel();

        void lock() { mutex.lock(); }
        void unlock() { mutex.unlock(); }

		int get_idx () const;
        std::string get_name () const;
        int get_vid_is_active () const;
        int get_vid_is_part_of_ref_fleet() const;
        int get_firm_id () const;
        Node* get_loc() const;
        const std::vector<Node *> &get_map_of_nodes() const;
        Metier* get_metier() const;
		double get_speed () const;
        double get_length () const;
		double get_KW () const;
        int get_length_class() const;
        std::string get_nationality () const;
        int get_nationality_idx() const;
        const vector<types::NodeId> &get_harbours() const;
        const vector<types::NodeId> &get_fgrounds() const;
        const vector<types::NodeId> &get_fgrounds_init() const;
    vector<types::NodeId> &get_fgrounds_in_closed_areas();
    const std::vector<double> &get_freq_harbours() const;
    const std::vector<double> &get_freq_fgrounds() const;
    const std::vector<double> &get_freq_fgrounds_init() const;
    const std::vector<double> &get_cumcatch_fgrounds() const;
    const std::vector<double> &get_cumdiscard_fgrounds() const;
    const std::vector<double> &get_experienced_bycatch_prop_on_fgrounds() const;
    const std::vector<double> &get_experienced_avoided_stks_bycatch_prop_on_fgrounds() const;
    const std::vector<std::vector<double> > &get_cumcatch_fgrounds_per_pop() const;
//        const std::vector<std::vector<vector <double> > >& get_cumcatch_fgrounds_per_met_per_pop() const;
    double get_cumcatch_fgrounds_per_met_per_pop(int fground, int met, int pop) const;
    const std::vector<std::vector<vector<double> > > &get_cumcatch_fgrounds_per_yearquarter_per_pop() const;
    const std::vector<std::vector<double> > &get_cumdiscard_fgrounds_per_pop() const;
    const std::vector<double> &get_cumeffort_per_trip_per_fgrounds() const;
    const std::vector<double> &get_cumeffort_per_yearquarter_per_fgrounds() const;
    const std::vector<vector<double >> &get_cumeffort_per_trip_per_fgrounds_per_met() const;
    const std::vector<double> &get_experiencedcpue_fgrounds() const;
    const std::vector<std::vector<double> > &get_experiencedcpue_fgrounds_per_pop() const;
    double get_experiencedcpue_fgrounds_per_met_per_pop(types::NodeId n, int met, int pop) const;
    const std::vector<std::vector<vector<double> > > &get_experiencedcpue_fgrounds_per_yearquarter_per_pop() const;
    const std::vector<double> &get_freq_experiencedcpue_fgrounds() const;
    const std::vector<std::vector<double> > &get_freq_experiencedcpue_fgrounds_per_pop() const;
    const Vessel::ValueFgroundsPerMetPerPop &copy_freq_experiencedcpue_fgrounds_per_met_per_pop() const;
    double get_freq_experiencedcpue_fgrounds_per_met_per_pop(types::NodeId node, int met, int pop) const;
    const std::vector<std::vector<vector<double> > > &get_freq_experiencedcpue_fgrounds_per_yearquarter_per_pop() const;
    const std::vector<double> &get_vessel_betas_per_pop() const;
    const std::vector<double> &get_percent_tac_per_pop() const;
    const std::vector<double> &get_fishing_credits() const;
    const std::vector<int> &get_idx_used_metiers_this_trip();
    const multimap<types::NodeId, int> &get_possible_metiers() const;
    const multimap<types::NodeId, double> &get_freq_possible_metiers() const;
    const std::list<types::NodeId> &get_roadmap() const;
    bool get_inharbour() const;
    bool get_inactive() const;
    int get_is_vessel_exited() const;
    bool get_natio() const;
    int get_state() const;
    int get_reason_to_go_back() const;
    int get_tstep_dep() const;
    double get_x() const;     // in the continuous space
    double get_y() const;     // in the continuous space
    double get_course() const;
    double get_timeforrest() const;
    double get_fuelcons() const;
    double get_cumfuelcons() const;
    double get_consotogetthere() const;
    double get_cumsteaming() const;
    double get_distprevpos() const;
    double get_timeatsea() const;
    int get_hasfishedatleastonce() const;
    double get_timeatseasincefirstcatch() const;
    double get_traveled_dist_this_trip() const;
    double get_sweptareathistrip() const;
    double get_subsurfacesweptareathistrip() const;
    double get_tankcapacity() const;
    double get_carrycapacity() const;
		int get_nbfpingspertrip() const;
		double get_resttime_par1() const;
		double get_resttime_par2() const;
		double get_av_trip_duration() const;
		double get_mult_fuelcons_when_steaming() const;
    	double get_mult_fuelcons_when_fishing() const;
	    double get_mult_fuelcons_when_returning() const;
	    double get_mult_fuelcons_when_inactive() const;		
		double get_cumcatches() const;
        double get_cumdiscards() const;
        double get_this_vessel_nb_crew() const;
        double get_annual_other_income() const;
        double get_landing_costs_percent() const;
        double get_crewshare_and_unpaid_labour_costs_percent() const;
        double get_other_variable_costs_per_unit_effort() const;
        double get_annual_insurance_costs_per_crew() const;
        double get_standard_labour_hour_opportunity_costs() const;
        double get_standard_annual_full_time_employement_hours() const;
        double get_other_annual_fixed_costs() const;
        double get_vessel_value() const;
        double get_annual_depreciation_rate() const;
        double get_opportunity_interest_rate() const;
        double get_annual_discount_rate() const;
        double getDaysSpentInRestrictedAreaThisMonth(int idx_met);
        types::NodeId get_smartcatch() const;
        types::NodeId get_highpotentialcatch() const;
        types::NodeId get_notthatfar() const;
        types::NodeId get_lowesttariff() const;
        types::NodeId get_mosthistoricallyused() const;
        const std::vector<std::vector<double> > &get_gshape_cpue_nodes_species() const;
        const std::vector<std::vector<double> > &get_gscale_cpue_nodes_species() const;
        const std::vector < std::vector<double> > &get_catch_pop_at_szgroup() const;
        const std::vector < std::vector<double> > &get_ping_catch_pop_at_szgroup() const;
        const std::vector < std::vector<double> > &get_discards_pop_at_szgroup() const;
        int read_message() const;
        types::NodeId get_previous_harbour_idx() const;
		int get_individual_tac (int sp) const;
        vector<double> get_is_choked () const;
        int get_individual_tac_per_pop_at_year_start (int sp) const;
        double get_prop_remaining_individual_quotas (int sp) const;
        double get_prop_remaining_global_quotas (int sp) const;
        double get_min_prop_remaining_individual_quotas_on_avoided_stks ();
        double get_min_prop_remaining_global_quotas_on_avoided_stks();
        double get_min_prop_remaining_individual_quotas ();
        double get_min_prop_remaining_global_quotas();
        int get_targeting_non_tac_pop_only() const;
        double get_GVA() const;
        double get_GVAPerRevenue() const;
        double get_LabourSurplus() const;
        double get_GrossProfit() const;
        double get_NetProfit() const;
        double get_NetProfitMargin() const;
        double get_GVAPerFTE() const;
        double get_RoFTA() const;
        double get_BER() const;
        double get_CRBER() const;
        double get_NetPresentValue() const;
        int get_numTrips()const;
        double get_effort_multiplier() const;
        void addADayPortionToDaysSpentInRestrictedAreaThisMonth(int idx_met, double a_portion);
        void reinitDaysSpentInRestrictedAreaThisMonthtoZero();
        void set_map_of_nodes (const vector<Node* > &_map_of_nodes);
        void set_firm_id (int val);
        void set_vid_is_active (int val);
        void set_vid_is_part_of_ref_fleet(int val);
        void set_speed (double val);
        void set_vessel_value (double val);
		void set_resttime_par1 (double val);
		void set_resttime_par2 (double val);
		void set_av_trip_duration (double val);
		void set_fuelcons (double val);
		void set_course (double val);
        void set_tankcapacity (double val);
        void set_fgrounds (int val);
        void set_fgrounds_init (int val);
        //void set_fgrounds_int_closed_areas (std::vector<int> grounds);
        void set_harbours (int val);
        void set_spe_harbours (const std::vector<types::NodeId> &_harbours);
        void set_spe_fgrounds (const vector<types::NodeId> &_fgrounds);
        void set_spe_fgrounds_init (const vector<types::NodeId> &_fgrounds_init);
        void set_fgrounds_in_closed_areas (const vector<types::NodeId> &_fgrounds);
        void set_spe_freq_harbours (const std::vector<double> &_harbours);
        void set_spe_freq_fgrounds (const std::vector<double> &_fgrounds);
        void set_spe_freq_fground (int index, double _fground);
        void set_spe_freq_fgrounds_init (const std::vector<double> &_fgrounds_init);
        void set_fishing_credits (const std::vector<double> &_fishing_credits);
        void set_spe_cumcatch_fgrounds (const std::vector<double> &_cumcatch);
        void set_spe_cumdiscard_fgrounds (const std::vector<double> &_cumdiscard);
        void set_spe_experienced_bycatch_prop_on_fgrounds (const std::vector<double> &_experienced_bycatch_prop_on_fgrounds);
        void set_spe_experienced_avoided_stks_bycatch_prop_on_fgrounds (const std::vector<double> &_experienced_avoided_stks_bycatch_prop_on_fgrounds);
        void set_spe_cumeffort_per_trip_per_fgrounds(const std::vector<double> &_cumeffort);
        void set_spe_cumeffort_per_yearquarter_per_fgrounds(const std::vector<double>& _cumeffort);
        void set_spe_experiencedcpue_fgrounds (const std::vector<double> &_experiencedcpue);
        void set_spe_betas_per_pop (const std::vector<double> &_betas_per_pop);
        void set_spe_percent_tac_per_pop (const std::vector<double> &_tacs_per_pop);
        void set_catch_pop_at_szgroup(std::vector<vector <double>> & init_catch_pop_at_szgroup);
        void set_ping_catch_pop_at_szgroup(std::vector<vector <double>>& init_ping_catch_pop_at_szgroup);
        void set_discards_pop_at_szgroup(std::vector<vector <double>> & init_discards_pop_at_szgroup);
        void set_spe_possible_metiers (const std::multimap <types::NodeId, int>  &_possible_metiers);
        void set_spe_freq_possible_metiers (const std::multimap <types::NodeId, double>  &_freq_possible_metiers);
    void init_cpue_nodes_species(int nbnodes, int nbspecies);
    void set_cpue_nodes_species(int sp, const std::vector<double> &newval);
    void init_gshape_cpue_nodes_species(int nbnodes, int nbspecies);
    void set_gshape_cpue_nodes_species(int sp, const std::vector<double> &newval);
    void init_gscale_cpue_nodes_species(int nbnodes, int nbspecies);
    void set_gscale_cpue_nodes_species(int sp, const std::vector<double> &newval);
    void set_cumcatch_fgrounds(const std::vector<double> &newval);
    void set_cumdiscard_fgrounds(const std::vector<double> &newval);
    void set_experienced_bycatch_prop_on_fgrounds(const std::vector<double> &newval);
    void set_experienced_avoided_stks_bycatch_prop_on_fgrounds(const std::vector<double> &newval);
    void set_cumcatch_fgrounds_per_pop(const std::vector<std::vector<double> > &newval);
    void set_cumcatch_fgrounds_per_met_per_pop(ValueFgroundsPerMetPerPop newval);
    void set_cumcatch_fgrounds_per_yearquarter_per_pop(const std::vector<std::vector<vector<double> > > &newval);
    void set_cumdiscard_fgrounds_per_pop(const std::vector<std::vector<double> > &newval);
    void set_cumeffort_per_trip_per_fgrounds(const std::vector<double> &newval);
    void set_cumeffort_per_yearquarter_per_fgrounds(const std::vector<double> &newval);
    void set_cumeffort_per_trip_per_fgrounds_per_met(const std::vector<vector<double> > &newval);
    void set_experiencedcpue_fgrounds(const std::vector<double> &newval);
    void set_experiencedcpue_fgrounds_per_pop(const std::vector<std::vector<double> > &newval);
    void set_experiencedcpue_fgrounds_per_met_per_pop(ValueFgroundsPerMetPerPop newval);
    void set_experiencedcpue_fgrounds_per_yearquarter_per_pop(const std::vector<std::vector<vector<double> > > &newval);
    void set_freq_experiencedcpue_fgrounds(const std::vector<double> &newval);
    void set_freq_experiencedcpue_fgrounds_per_pop(const std::vector<std::vector<double> > &newval);
    void set_freq_experiencedcpue_fgrounds_per_met_per_pop(ValueFgroundsPerMetPerPop newval);
    void
    set_freq_experiencedcpue_fgrounds_per_yearquarter_per_pop(const std::vector<std::vector<vector<double> > > &newval);
        void clear_idx_used_metiers_this_trip();
        void set_roadmap (const std::list<types::NodeId> &_roadmap);
		void set_inharbour (bool logic);
		void set_inactive (bool logic);
        void set_vessel_exited (int logic);
		void set_natio (bool logic);
        void set_smartcatch (types::NodeId smartcatch);
        void set_highpotentialcatch (types::NodeId highpotentialcatch);
        void set_notthatfar (types::NodeId notthatfar);
        void set_lowesttariff (types::NodeId lowesttariff);
        void set_mosthistoricallyused (types::NodeId mosthistoricallyused);
        void set_state (int _state);
        void set_previous_harbour_idx (types::NodeId previous_harbour_idx);
		void set_reason_to_go_back (int _reason_to_go_back);
		void set_tstep_dep (int _tstep);
		void set_timeforrest (double _timeforrest);
		void set_cumfuelcons (double _cumfuelcons);
		void set_distprevpos (double _distprevpos);
		void set_cumsteaming (double _cumsteaming);
		void set_consotogetthere (double _consotogetthere);
		void set_timeatsea (double _timeatsea);
        void set_hasfishedatleastonce(int _hasfishedatleastonce);
        void set_timeatseasincefirstcatch(double _timeatseasincefirstcatch);
        void set_traveled_dist_this_trip (double _traveled_dist_this_trip);
        void set_sweptareathistrip (double _sweptareathistrip);
        void set_subsurfacesweptareathistrip(double _subsurfacesweptareathistrip);
        void set_cumcatches (double _cumcatches);
        void set_cumdiscards (double _cumdiscards);

    void set_nbfpingspertrip(int _nbfpingspertrip);

    void set_xy(double _x, double _y);

    void set_next_xy(double nx, double ny);

    void set_effort_multiplier(double multi);

    void erode_roadmap();

    void move_to(double nx, double ny);

    void move_to(Node *next_node);

    void set_metier(Metier *new_metier);

    void set_is_choked(int pop, int val);

    void find_next_point_on_the_graph(std::vector<Node *> &nodes, int a_tstep, bool is_fishing_credits);

    void do_catch(const DynAllocOptions& dyn_alloc_sce,
                  std::ofstream &export_individual_tacs,
                  int a_tstep,
                  int a_month,
                  int a_quarter,
                  std::vector<Population *> const &populations,
                  std::vector<Node *> const &nodes,
                  vector<Benthos *> const &benthoshabs,
                  std::vector<int> const &implicit_pops,
                  vector<int> const &grouped_tacs,
                  int tstep,
                  vector<double> const &graph_res,
                  bool is_tacs,
                  bool is_individual_vessel_quotas,
                  bool check_all_stocks_before_going_fishing,
                  bool is_discard_ban,
                  bool is_realtime_closure,
                  bool is_grouped_tacs,
                  double tech_creeping_multiplier,
                  bool is_fishing_credits,
                  bool direct_killing_on_benthos,
                  bool resuspension_effect_on_benthos,
                  bool is_benthos_in_numbers);

    void clear_catch_pop_at_szgroup();

    void clear_ping_catch_pop_at_szgroup();

    void clear_discards_pop_at_szgroup();

    void compute_experiencedcpue_fgrounds();
    void compute_experiencedcpue_fgrounds_per_pop();
    void compute_experiencedcpue_fgrounds_per_met_per_pop();
    bool compute_experiencedcpue_fgrounds_per_yearquarter_per_pop(int the_year, int the_quarter);

    void clear_cumcatch_and_cumeffort_per_trip(const DynAllocOptions& dyn_alloc_sce);
    void clear_cumeffort_per_yearquarter();
    void reinit_after_a_trip(const DynAllocOptions& dyn_alloc_sce);

	void receive_message(int message);
	void reset_message();
    
    void export_loglike_prop_met(std::ofstream& loglike_prop_met, int tstep, int nbpops);
    
    void alter_freq_fgrounds_for_nodes_in_polygons(std::multimap <int, int> nodes_in_polygons);
	void alloc_on_high_previous_cpue(const SimModel& simModel,
                                         int tstep,
                                         std::ofstream& freq_cpue);
    void alloc_on_high_profit_grounds(const SimModel& simModel,
                                          int tstep,
                                          int use_static_paths,
                                          vector<Node *> &nodes,
                                          const std::vector<types::NodeId> &relevant_nodes,
                                          const std::vector<PathShop> &pathshops,
                                          std::ofstream& freq_profit,
                                          const DynAllocOptions& dyn_alloc_sce);
    std::vector<double> expected_profit_on_grounds(const SimModel& simModel, 
                                                       int use_static_paths,
                                                       vector<Node*>& nodes,
                                                       const std::vector<types::NodeId> &relevant_nodes,
                                                       const std::vector<PathShop> &pathshops,
                                                       const DynAllocOptions& dyn_alloc_sce);
    void alloc_while_saving_fuel(const SimModel& simModel, 
                                     int tstep,
                                     int use_static_paths,
                                     vector<Node*>& nodes,
                                    const std::vector<types::NodeId> &relevant_nodes,
           const std::vector<PathShop> &pathshops);
    void alloc_on_closer_grounds(const SimModel& simModel, 
                                     int tstep, int use_static_paths,
                                     vector<Node *> &nodes,
                                     const std::vector<types::NodeId> &relevant_nodes,
            const std::vector<PathShop> &pathshops,
            std::ofstream& freq_distance);

    bool choose_a_ground_and_go_fishing(const SimModel& simModel,
                                           int tstep, 
            const displace::commons::Scenario &scenario, bool use_the_tree,
            const DynAllocOptions &dyn_alloc_sce,
            int use_static_paths,
            const std::vector <PathShop>& pathshops,
            adjacency_map_t& adjacencymap,
            vector<types::NodeId> &relevant_nodes,
            std::multimap<int, int>& nodes_in_polygons,
            std::vector<Node* >& nodes,
            std::vector <Metier*>& metiers,
            std::ofstream& freq_cpue,
            std::ofstream& freq_profit,
            std::ofstream& freq_distance);


    int choose_another_ground_and_go_fishing(const SimModel& simModel, 
                                                 int tstep,
                                                 const DynAllocOptions &dyn_alloc_sce,
                                                 int use_static_paths,
                                                 const std::vector<PathShop> &pathshops,
                                                 adjacency_map_t &adjacency_map,
                                                 vector<types::NodeId> &relevant_nodes,
                                                 const std::multimap<int, int> &nodes_in_polygons,
                                                 std::vector<Node *> &nodes,
                                                 std::vector<Metier *> &metiers,
                                                 std::ofstream &freq_cpue,
                                                 std::ofstream &freq_distance);

    void choose_a_port_and_then_return(const SimModel& simModel, 
                                       int tstep,
                                       const DynAllocOptions &dyn_alloc_sce,
                                       int use_static_paths,
                                       const std::vector<PathShop> &pathshops,
                                       adjacency_map_t &adjacency_map,
                                       vector<types::NodeId> &relevant_nodes,
                                       std::vector<Node *> &nodes,
                                       std::vector<Metier *> &metiers,
                                       std::ofstream &freq_cpue,
                                       std::ofstream &freq_distance,
                                       std::vector<double> &dist_to_ports
    );

    void which_metier_should_i_go_for(std::vector<Metier *> &metiers);

    //yes:1; no=0
    int should_i_go_fishing(int tstep, 
                            std::vector<Population *> &populations,
                            bool use_the_tree,
                            const DynAllocOptions &dyn_alloc_sce,
                            std::vector<int> const &implicit_pops, 
                            int is_individual_vessel_quotas,
                            int check_all_stocks_before_going_fishing);

    types::NodeId should_i_choose_this_ground(const SimModel& simModel, 
                                              int tstep, int use_static_paths,
                                              std::vector<Node *> &nodes,
                                              const std::vector<types::NodeId> &relevant_nodes,
                                              const std::vector<PathShop> &pathshops,
                                              const DynAllocOptions &dyn_alloc_sce);

    int should_i_change_ground(const SimModel& simModel, 
                               std::map<std::string, int> &external_states, bool use_the_tree);

    //yes:1; no=0
    int should_i_stop_fishing(const SimModel& simModel, 
                              const std::map<std::string, int> &external_states, bool use_the_tree,
                              int tstep,
                              const DynAllocOptions &dyn_alloc_sce,
                              int use_static_paths,
                              const std::vector<PathShop> &pathshops,
                              adjacency_map_t &adjacency_map,
                              const vector<types::NodeId> &relevant_nodes,
                              std::vector<Node *> &nodes,
                              std::vector<Metier *> &metiers,
                              std::ofstream &freq_cpue,
                              std::ofstream &freq_distance,
                              std::vector<double> &dist_to_ports);

    int should_i_choose_this_port(const SimModel& simModel, 
                                 std::map<std::string, int> &external_states,
                                 bool use_the_tree);

    void set_individual_tac_this_pop(std::ofstream &export_individual_tacs, int tstep,
                                     std::vector<Population *> const &populations, std::vector<int> implicit_pops,
                                     int pop,
                                     int init, double a_tac);

    void set_targeting_non_tac_pop_only(int targeting_non_tac_pop_only);

    double getLastTripRevenues() const
    {
        return lastTrip_revenues;
    }

    double getLastTripExplicitRevenues() const
    {
        return lastTrip_explicit_revenues;
    }

    double getAvgTripRevenues() const
    {
        return avgRevenues;
    }

    double getLastTripProfit() const
    {
        return lastTrip_profit;
    }

    double getAvgTripProfit() const
    {
        return avgProfit;
    }

    int getNumTrips() const
    {
        return numTrips;
    }

    void updateTripsStatistics(const std::vector<Population *> &populations,
                               std::vector<int> const &implicit_pops, int tstep,
                               const DynAllocOptions &dyn_alloc_sce);

    double traverseDtree(int tstep, dtree::DecisionTree *tree);

    // Calendar values access functions
    void updateCalendar(const VesselCalendar &c)
    {
        calendar = c;
    }

    int getWeekEndStartDay() const
    { return calendar.weekEndStartDay; }

    int getWeekEndEndDay() const
    { return calendar.weekEndEndDay; }

    int getWorkDayStartHour() const
    { return calendar.workStartHour; }

    int getWorkDayEndHour() const
    { return calendar.workEndHour; }

    /** \brief the State evaluators. Since the current vessel is passed as variable to evaluate()
     * we can put a static instance and avoid wasting a lot of memory.
     * The table is initialized once in init() (lazy initialization)
     */
    static std::vector<std::shared_ptr<dtree::StateEvaluator> > mStateEvaluators;

    static std::string nationalityFromName(const std::string &name);
};
#endif							 // VESSEL_H
