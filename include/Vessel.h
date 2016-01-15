// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

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
#include <string>
#include <iostream>
#include <deque>
#include <list>
#include <Node.h>
#include <Metier.h>
#include <Population.h>
#include <myutils.h>
#include <options.h>
#include <dtree/decisiontree.h>
#include <dtree/stateevaluator.h>

#include <pthread.h>

typedef int vertex_t;

class Vessel
{
	private:
    enum LengthClass {
        Under15 = 0, Between15and18, Between18and24, Between24and40, Over40
    };

		string name;
		int idx_vessel;
		//boost::shared_ptr<Node> m_location;
		Node* m_location;
		Metier* metier;
        vector<int> fgrounds;	 // idx nodes for fishing grounds specific to this vessel
        vector<int> fgrounds_init;	 // fishing grounds specific to e.g. port shared
        vector<int> fgrounds_in_closed_areas;	 // idx nodes
        vector<int> harbours;	 // idx nodes
								 // freq of visit per node
		vector<double> freq_fgrounds;
								 // freq of visit per node
        vector<double> freq_fgrounds_init;
                                 // freq of visit per node
        vector<double> freq_harbours;
								 // cumulated catch per node for the last trip, reinit when new trip start
		vector<double> cumcatch_fgrounds;
        vector<double> experienced_bycatch_prop_on_fgrounds;

        vector< vector<double> > cumcatch_fgrounds_per_pop;
								 // cumulated effort per node for the last trip, reinit when new trip start
		vector<double> cumeffort_fgrounds;
								 // from cumcatch/cumeffort for last trip
		vector<double> experiencedcpue_fgrounds;
								 // from cumcatch/cumeffort for last trip
		vector<vector<double> > experiencedcpue_fgrounds_per_pop;
								 // scaled to 1 from experiencedcpue_fgrounds
		vector<double> freq_experiencedcpue_fgrounds;
								 // scaled to 1 from experiencedcpue_fgrounds
		vector<vector<double> > freq_experiencedcpue_fgrounds_per_pop;
								 // skipper effect, target factor
		vector<double> vessel_betas_per_pop;
								 //
		vector<double> percent_tac_per_pop;
								 // clear at the end of the trip
		vector<int> idx_used_metiers_this_trip;
								 // list of possible metiers on specific ground
		multimap <int, int> possible_metiers;
								 // freq of the metier on specific fground
		multimap <int, double> freq_possible_metiers;
		vector< vector<double> > cpue_nodes_species;
		vector< vector<double> > gshape_cpue_nodes_species;
		vector< vector<double> > gscale_cpue_nodes_species;
        std::list<vertex_t> roadmap;
								 // because movement is semi-continuous...
		double x, y, prev_x, prev_y, next_x, next_y, course;
								 // fixed
		double speed, fuelcons, length, KW, tankcapacity, carrycapacity, nbfpingspertrip, resttime_par1, resttime_par2, av_trip_duration;
								 // dynamic
		double timeforrest, cumfuelcons, consotogetthere, cumsteaming, distprevpos, timeatsea, traveled_dist_this_trip, cumcatches, reason_to_go_back;
		double mult_fuelcons_when_steaming, mult_fuelcons_when_fishing, mult_fuelcons_when_returning, mult_fuelcons_when_inactive;

        int length_class;
        string nationality;
        LengthClass mLengthClassId;
		int message;
		int state;
		int tstep_dep;
		int previous_harbour_idx;

        double lastTrip_revenues;
        double lastTrip_explicit_revenues;
        double lastTrip_profit;
        double avgRevenues;
        double avgProfit;
        int numTrips;
								 // dynamic
		bool inharbour, inactive, natio;
		vector < vector<double> > catch_pop_at_szgroup;
        vector < vector<double> > discards_pop_at_szgroup;
        vector<int> individual_tac_per_pop;
        vector<double> fishing_credits;
		int targeting_non_tac_pop_only;

        int smartcatch, highpotentialcatch, notthatfar, mosthistoricallyused; // some relevant grounds

        pthread_mutex_t mutex;

protected:
        void init();
        void find_next_point_on_the_graph_unlocked(vector<Node* >& nodes);

public:
		//Vessel(string name,  boost::shared_ptr<Node> a_location);
		Vessel(string name,  Node* a_location);
		//Vessel(boost::shared_ptr<Node> a_location, int idx_vessel, string name);
		Vessel(Node* a_location, int idx_vessel, string name);
		//Vessel(boost::shared_ptr<Node> a_location, int idx_vessel, string name, int nbpops, int nbszgroups, vector<int> harbours, vector<int> fgrounds,
        Vessel(Node* a_location, int idx_vessel, string name, int nbpops, int nbszgroups, const vector<int> &harbours, const vector<int> &fgrounds, const vector<int> &fgrounds_init,
            const vector<double> &freq_harbours, const vector<double> &freq_fgrounds, const vector<double> &freq_fgrounds_init, const vector<double> &vessel_betas_per_pop,
            const vector<double> &percent_tac_per_pop,
            const multimap <int, int> &possible_metiers, const multimap <int, double> &freq_possible_metiers,
            double speed, double fuelcons, double length, double KW,
            double  carrycapacity, double tankcapacity, double nbfpingspertrip,
            double resttime_par1, double resttime_par2, double av_trip_duration,
            double mult_fuelcons_when_steaming, double mult_fuelcons_when_fishing,
            double mult_fuelcons_when_returning, double mult_fuelcons_when_inactive
            );
		Vessel();
		~Vessel();

        void lock() { pthread_mutex_lock (&mutex); }
        void unlock() { pthread_mutex_unlock (&mutex); }

		int get_idx () const;
		string get_name () const;
		Node* get_loc() const;
		Metier* get_metier() const;
		double get_speed () const;
		double get_length () const;
		double get_KW () const;
        int get_length_class() const;
		string get_nationality () const;
        const vector<int> &get_harbours () const;
        const vector<int> &get_fgrounds () const;
        const vector<int> &get_fgrounds_init () const;
        vector<int> &get_fgrounds_in_closed_areas ();
        const vector<double> &get_freq_harbours () const;
        const vector<double> &get_freq_fgrounds () const;
        const vector<double> &get_freq_fgrounds_init () const;
        const vector<double> &get_cumcatch_fgrounds () const;
        const vector<double> &get_experienced_bycatch_prop_on_fgrounds () const;
        const vector<vector<double> > &get_cumcatch_fgrounds_per_pop () const;
        const vector<double> &get_cumeffort_fgrounds () const;
        const vector<double> &get_experiencedcpue_fgrounds () const;
        const vector<vector<double> > &get_experiencedcpue_fgrounds_per_pop () const;
        const vector<double> &get_freq_experiencedcpue_fgrounds () const;
        const vector<vector<double> > &get_freq_experiencedcpue_fgrounds_per_pop () const;
        const vector<double> &get_vessel_betas_per_pop () const;
        const vector<double> &get_percent_tac_per_pop () const;
        const vector<double> &get_fishing_credits () const;
        const vector<int> &get_idx_used_metiers_this_trip ();
        const multimap<int,int> &get_possible_metiers () const;
        const multimap<int,double> &get_freq_possible_metiers () const;
        const std::list<vertex_t> &get_roadmap () const;
		bool get_inharbour() const;
		bool get_inactive() const;
		bool get_natio() const;
		int get_state() const;
		int get_reason_to_go_back () const;
		int get_tstep_dep() const;
		double get_x() const;	 // in the continuous space
		double get_y() const;	 // in the continuous space
		double get_course() const;
		double get_timeforrest() const;
		double get_fuelcons() const;
		double get_cumfuelcons() const;
		double get_consotogetthere() const;
		double get_cumsteaming() const;
		double get_distprevpos() const;
		double get_timeatsea() const;
		double get_traveled_dist_this_trip() const;
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
        int get_smartcatch() const;
        int get_highpotentialcatch() const;
        int get_notthatfar() const;
        int get_mosthistoricallyused() const;
        const vector<vector<double> > &get_gshape_cpue_nodes_species() const;
        const vector<vector<double> > &get_gscale_cpue_nodes_species() const;
        const vector < vector<double> > &get_catch_pop_at_szgroup() const;
        const vector < vector<double> > &get_discards_pop_at_szgroup() const;
        int read_message() const;
		int get_previous_harbour_idx() const;
		int get_individual_tac (int sp) const;
		int get_targeting_non_tac_pop_only() const;
        void set_speed (double val);
		void set_resttime_par1 (double val);
		void set_resttime_par2 (double val);
		void set_av_trip_duration (double val);
		void set_fuelcons (double val);
		void set_course (double val);
        void set_fgrounds (int val);
        void set_fgrounds_init (int val);
        void set_fgrounds_int_closed_areas (vector<int> grounds);
        void set_harbours (int val);
		void set_spe_harbours (vector<int> _harbours);
		void set_spe_fgrounds (vector<int> _fgrounds);		
        void set_spe_fgrounds_init (vector<int> _fgrounds_init);
        void set_fgrounds_in_closed_areas (vector<int> _fgrounds);
        void set_spe_freq_harbours (vector<double> _harbours);
		void set_spe_freq_fgrounds (vector<double> _fgrounds);
        void set_spe_freq_fgrounds_init (vector<double> _fgrounds_init);
        void set_fishing_credits (vector<double> _fishing_credits);
        void set_spe_cumcatch_fgrounds (vector<double> _cumcatch);
        void set_spe_experienced_bycatch_prop_on_fgrounds (vector<double> _experienced_bycatch_prop_on_fgrounds);
        void set_spe_cumeffort_fgrounds (vector<double> _cumeffort);
		void set_spe_experiencedcpue_fgrounds (vector<double> _experiencedcpue);
		void set_spe_betas_per_pop (vector<double> _betas_per_pop);
		void set_spe_percent_tac_per_pop (vector<double> _tacs_per_pop);
		void set_spe_possible_metiers (multimap <int, int>  _possible_metiers);
		void set_spe_freq_possible_metiers (multimap <int, double>  _freq_possible_metiers);
		void init_cpue_nodes_species(int nbnodes, int nbspecies);
		void set_cpue_nodes_species(int sp, vector<double> newval);
		void init_gshape_cpue_nodes_species(int nbnodes, int nbspecies);
		void set_gshape_cpue_nodes_species(int sp, vector<double> newval);
		void init_gscale_cpue_nodes_species(int nbnodes, int nbspecies);
		void set_gscale_cpue_nodes_species(int sp, vector<double> newval);
		void set_cumcatch_fgrounds(vector<double> newval);
        void set_experienced_bycatch_prop_on_fgrounds(vector<double> newval);
        void set_cumcatch_fgrounds_per_pop(vector<vector<double> > newval);
		void set_cumeffort_fgrounds(vector<double> newval);
		void set_experiencedcpue_fgrounds(vector<double> newval);
		void set_experiencedcpue_fgrounds_per_pop(vector<vector<double> > newval);
		void set_freq_experiencedcpue_fgrounds(vector<double> newval);
		void set_freq_experiencedcpue_fgrounds_per_pop(vector<vector<double> > newval);
		void clear_idx_used_metiers_this_trip();
        void set_roadmap (const std::list<vertex_t> &_roadmap);
		void set_inharbour (bool logic);
		void set_inactive (bool logic);
		void set_natio (bool logic);
        void set_smartcatch (int smartcatch);
        void set_highpotentialcatch (int highpotentialcatch);
        void set_notthatfar (int notthatfar);
        void set_mosthistoricallyused (int mosthistoricallyused);
        void set_state (int _state);
		void set_previous_harbour_idx (int previous_harbour_idx);
		void set_reason_to_go_back (int _reason_to_go_back);
		void set_tstep_dep (int _tstep);
		void set_timeforrest (double _timeforrest);
		void set_cumfuelcons (double _cumfuelcons);
		void set_distprevpos (double _distprevpos);
		void set_cumsteaming (double _cumsteaming);
		void set_consotogetthere (double _consotogetthere);
		void set_timeatsea (double _timeatsea);
		void set_traveled_dist_this_trip (double _traveled_dist_this_trip);
		void set_cumcatches (double _cumcatches);
		void set_nbfpingspertrip (int _nbfpingspertrip);
		void set_xy (double _x, double _y);
		void set_next_xy (double nx, double ny);
		void erode_roadmap ();
		void move_to(double nx, double ny);
		void move_to(Node* next_node);
		void set_metier(Metier* new_metier);
        void find_next_point_on_the_graph(vector<Node* >& nodes);
        void do_catch(ofstream& export_individual_tacs, vector<Population* >& populations, vector<Node* >& nodes, vector<int>& implicit_pops, int& tstep, double &graph_res,
                      bool &is_tacs, int &is_individual_vessel_quotas, bool &is_fishing_credits);
		void clear_catch_pop_at_szgroup();
        void clear_discards_pop_at_szgroup();
        void compute_experiencedcpue_fgrounds();
		void compute_experiencedcpue_fgrounds_per_pop();
		void clear_cumcatch_and_cumeffort();
		void receive_message(int message);
		void reset_message();
		void export_loglike_prop_met(ofstream& loglike_prop_met, int tstep, int nbpops);
		void reinit_after_a_trip();
		void alter_freq_fgrounds_for_nodes_in_polygons(multimap <int, int> nodes_in_polygons);
		void alloc_on_high_previous_cpue(int tstep,
			ofstream& freq_cpue);
        void alloc_on_high_profit_grounds(int tstep, const vector<int> &idx_path_shop,
            const deque<map<vertex_t, vertex_t> > &path_shop,
            const deque<map<vertex_t, weight_t> > &min_distance_shop,
            ofstream& freq_profit);
        vector<double> expected_profit_on_grounds(const vector<int> &idx_path_shop,
            const deque<map<vertex_t, vertex_t> > &path_shop,
            const deque<map<vertex_t, weight_t> > &min_distance_shop
            );
        void alloc_while_saving_fuel(int tstep, const vector<int> &idx_path_shop,
            const deque<map<vertex_t, vertex_t> > &path_shop,
            const deque<map<vertex_t, weight_t> > &min_distance_shop
            );
        void alloc_on_closer_grounds(int tstep, const vector<int> &idx_path_shop,
            const deque<map<vertex_t, vertex_t> > &path_shop,
            const deque<map<vertex_t, weight_t> > &min_distance_shop,
            ofstream& freq_distance);
        void choose_a_ground_and_go_fishing(int tstep, bool use_the_tree,
            const DynAllocOptions &dyn_alloc_sce,
            int create_a_path_shop,
            const vector <int>& idx_path_shop,
            const deque<map<vertex_t, vertex_t> >& path_shop,
            const deque<map<vertex_t, weight_t> >& min_distance_shop,
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
            ofstream& freq_distance);
        void choose_another_ground_and_go_fishing(int tstep,
            const DynAllocOptions &dyn_alloc_sce,
            int create_a_path_shop,
            const vector <int>& idx_path_shop,
            const deque<map<vertex_t, vertex_t> >& path_shop,
            const deque<map<vertex_t, weight_t> >& min_distance_shop,
            adjacency_map_t& adjacency_map,
            map<vertex_t, weight_t>& min_distance,
            map<vertex_t, vertex_t>& previous,
            vector <int>& relevant_nodes,
            const multimap<int, int> &nodes_in_polygons,
            vector<string>& vertex_names,
            vector<Node* >& nodes,
            vector <Metier*>& metiers,
            ofstream& freq_cpue,
            ofstream& freq_distance);
        void choose_a_port_and_then_return(int tstep,
            const DynAllocOptions &dyn_alloc_sce,
            int create_a_path_shop,
            const vector <int>& idx_path_shop,
            const deque<map<vertex_t, vertex_t> >& path_shop,
            const deque<map<vertex_t, weight_t> >& min_distance_shop,
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
            );
        void which_metier_should_i_go_for(vector <Metier*>& metiers);
                                 //yes:1; no=0
        int should_i_go_fishing(int tstep, bool use_the_tree, vector<int> &implicit_pops, int check_all_stocks);
		int should_i_start_fishing(map<string, int>& external_states, bool use_the_tree);
        int should_i_choose_this_ground(int tstep, vector<Node*>& nodes,  const vector <int>& idx_path_shop,
                                        const deque<map<vertex_t, vertex_t> >& path_shop,
                                        const deque<map<vertex_t, weight_t> >& min_distance_shop);
		int should_i_change_ground(map<string, int>& external_states, bool use_the_tree);
								 //yes:1; no=0
        int should_i_stop_fishing(const map<string, int> &external_states, bool use_the_tree,
            int tstep,
            const DynAllocOptions& dyn_alloc_sce,
            int create_a_path_shop,
            const vector<int> &idx_path_shop,
            const deque<map<vertex_t, vertex_t> > &path_shop,
            const deque<map<vertex_t, weight_t> > &min_distance_shop,
            adjacency_map_t& adjacency_map,
            map<vertex_t, weight_t> &min_distance,
            map<vertex_t, vertex_t> &previous,
            const vector<int> &relevant_nodes,
            vector<string>& vertex_names,
            vector<Node* >& nodes,
            vector <Metier*>& metiers,
            ofstream& freq_cpue,
            ofstream& freq_distance,
            vector <double>& dist_to_ports);
		int should_i_choose_this_port(map<string,int>& external_states, bool use_the_tree);

        void set_individual_tac_this_pop(ofstream& export_individual_tacs, int tstep, vector<Population* >& populations, int pop, int init, double a_tac);
		void set_targeting_non_tac_pop_only(int targeting_non_tac_pop_only);

        double getLastTripRevenues() const {
            return lastTrip_revenues;
        }
        double getLastTripExplicitRevenues() const {
            return lastTrip_explicit_revenues;
        }
        double getAvgTripRevenues() const {
            return avgRevenues;
        }
        double getLastTripProfit() const {
            return lastTrip_profit;
        }
        double getAvgTripProfit() const {
            return avgProfit;
        }
        int getNumTrips() const {
            return numTrips;
        }

        void updateTripsStatistics(const std::vector<Population *> &populations, vector<int> &implicit_pops);

        double traverseDtree (int tstep, dtree::DecisionTree *tree);

        /** \brief the State evaluators. Since the current vessel is passed as variable to evaluate()
         * we can put a static instance and avoid wasting a lot of memory.
         * The table is initialized once in init() (lazy initialization)
         */
        static std::vector<boost::shared_ptr<dtree::StateEvaluator> > mStateEvaluators;
        static std::string nationalityFromName (const std::string &name);
};
#endif							 // VESSEL_H
