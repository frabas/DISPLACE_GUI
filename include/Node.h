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

#ifndef NODE_H
#define NODE_H

#include<vector>
#include <algorithm>
#include<string>
#include<map>
#include "myutils.h"
#include "myRutils.h"

#include <pthread.h>


using namespace std;

class Node
{
	public:

		/**  constructor */
		Node ();
        Node (int idx_node, double xval, double yval, int _harbour, int _code_area, int _marine_landscape, int nbpops, int nbbenthospops,  int nbszgroups);
        Node (int idx_node, const vector<double> &graph_coord_x, const vector<double> &graph_coord_y,
            const vector<int> &graph_coord_harbour,
            const vector<int> &graph_point_code_area,
            const vector<int> &graph_marine_landscape,
            int nbpops,  int nbbenthospops, int nbszgroups);

		/**  destructor */
		~Node();

        void lock() { pthread_mutex_lock (&mutex); }
        void unlock() { pthread_mutex_unlock (&mutex); }

		int get_idx_node() const;
		int get_code_area() const;
        void setCodeArea(int id) {
            code_area = id;
        }

		int get_marine_landscape() const;
        void setMarineLandscape(int id) {
            marine_landscape = id;
        }
								 // declare virtual to enable dynamic binding for chlidren classes e.g. Harbour
		virtual string get_name() const;
		//virtual double get_prices(string met, int pop) ; // declare virtual to enable dynamic binding for chlidren classes e.g. Harbour
								 // declare virtual to enable dynamic binding for chlidren classes e.g. Harbour
		virtual double get_prices_per_cat(int pop, int cat) ;
								 // declare virtual to enable dynamic binding for chlidren classes e.g. Harbour
        virtual double get_fuelprices(int vsize) ;
        virtual const vector<int> &get_usual_fgrounds () const;
        virtual const vector<double> &get_freq_usual_fgrounds () const;
        virtual void set_usual_fgrounds (const vector<int> &usual_fgrounds);
        virtual void set_freq_usual_fgrounds (const vector<double> &_freq_usual_fgrounds);
        virtual const multimap<int,int> &get_usual_metiers () const;
        virtual const multimap<int,double> &get_freq_usual_metiers () const;
        virtual void set_usual_metiers (multimap <int,int> usual_metiers);
        virtual void set_freq_usual_metiers (multimap <int,double> freq_usual_metiers);
        virtual void set_usual_fgrounds_per_met (multimap <int,int> usual_fgrounds_per_met);
        virtual void set_freq_usual_fgrounds_per_met (multimap <int,double> freq_usual_fgrounds_per_met);
        virtual vector<int> get_usual_fgrounds_this_met(int met);
        virtual vector<double> get_freq_usual_fgrounds_this_met(int met);


        double get_x() const;
		double get_y() const;
		int get_harbour() const;
		bool get_is_harbour() const;
        const vector<double> &get_Ns_pops_at_szgroup(int name_pop) const;
        vector<double> get_Ns_pops_at_szgroup_at_month_start(int name_pop) const;
        vector<double> get_removals_pops_at_szgroup(int name_pop) const;
        vector<double> get_pressure_pops_at_szgroup(int name_pop) const;
        vector<double> get_avai_pops_at_selected_szgroup(int name_pop) const;
		double get_benthos_tot_biomass(int funcgr) const;
        const vector<double>& get_benthos_tot_biomass() const;
        double get_tariffs(int type) const;
        const vector<double>& get_tariffs() const;
        vector< vector<double> >  get_Ns_pops_at_szgroup() const;
		vector<int> get_vid();
		vector<int> get_pop_names_on_node();
        const vector<double>& get_impact_on_pops ();
        const vector<double>& get_cumcatches_per_pop ();
        int get_cumftime() const;
        double get_cumsweptarea() const;
        double get_cumcatches() const;
        void set_xy(double xval, double yval);
		void init_Ns_pops_at_szgroup(int nbpops, int nbszgroups);
        void set_Ns_pops_at_szgroup(int name_pop, const vector<double>& val);
        void set_Ns_pops_at_szgroup_at_month_start(int name_pop, const vector<double>& val);
        void set_removals_pops_at_szgroup(int name_pop, const vector<double>& val);
        void set_pressure_pops_at_szgroup(int name_pop, const vector<double>& val);
		void init_avai_pops_at_selected_szgroup(int nbpops, int selected_nbszgroups);
        void set_avai_pops_at_selected_szgroup(int name_pop, const vector<double>& val);
		void set_impact_on_pops(int name_pop, double val);
        void set_cumcatches_per_pop(int name_pop, double val);
        void set_vid(int val);
        void set_cumftime(int tot);
        void set_cumsweptarea(double tot);
        void set_cumcatches(double tot);
        void add_to_cumftime(int delta_time);
        void add_to_cumsweptarea(double sweptarea);
        void add_to_sweptarea(int sweptarea);
        void add_to_cumcatches(double catches);
        void add_to_cumcatches_per_pop(double catches, int pop);
        void set_pop_names_on_node(int name_pop);
		void set_benthos_tot_biomass(int funcgr, double value);
        void set_tariffs(vector<double> values);
        void set_tariffs(int type, double value);
        void clear_pop_names_on_node();
        void remove_pop_names_on_node(int name_pop);
		void clear_Ns_pops_at_szgroup();
        void clear_removals_pops_at_szgroup(int pop);
		void clear_avai_pops_at_selected_szgroup();
		void clear_impact_on_pops();
        void clear_cumcatches_per_pop();
        void apply_natural_mortality_at_node(int name_pop, const vector<double>& M_at_szgroup, vector<double>& prop_M_from_species_interactions);
        void apply_oth_land(int name_pop, double &oth_land_this_pop_this_node, const vector<double>&  weight_at_szgroup, const vector<double>& totN);
		void export_popnodes(ofstream& popnodes, multimap<int,double> weight_at_szgroup, int tstep);
		void export_popnodes_impact(ofstream& popnodes, int tstep, int pop);
		void export_popnodes_impact_per_szgroup(ofstream& popnodes, int tstep, int pop);
        void export_benthos_tot_biomass_per_funcgroup(ofstream& benthosnodes, int tstep, int funcgroup);
		void recover_benthos_tot_biomass_per_funcgroup();
		void export_popnodes_cumftime(ofstream& popnodes, int tstep);
        void export_popnodes_cumsweptarea(ofstream& popnodes, int tstep);
        void export_popnodes_cumcatches(ofstream& popnodes, int tstep);
        void export_popnodes_cumulcatches_per_pop(ofstream& popnodes, int tstep, int pop);
        void export_popnodes_tariffs(ofstream& popnodes, int tstep);
        void add_benthos_tot_biomass_on_node(double tot_biomass_this_group);

        void setAreaType(int _area_type);
        int evaluateAreaType();

        int get_nbpops() const { return m_nbpops; }
        int get_nbbenthospops() const { return m_nbbenthospops; }
        int get_nszgroups() const { return m_nszgrp; }

        void set_is_harbour(int id);
        void set_idx_node(int idx);


protected:
        void reinit(vector<double> &vec, unsigned int sz);
        void reinit(vector<vector<double> > &vec, unsigned int sz, unsigned int subsz);
private:
		int idx_node;
		double x;
		double y;
		int harbour;
		int code_area;
		int marine_landscape;
		bool is_harbour;
		int cumftime;
        double cumsweptarea;
        double cumcatches;
        vector< vector<double> > Ns_pops_at_szgroup;
		vector< vector<double> > Ns_pops_at_szgroup_at_month_start;
		vector< vector<double> > removals_pops_at_szgroup;
								 //i.e. impact
		vector< vector<double> > pressure_pops_at_szgroup;
		vector< vector<double> > avai_pops_at_selected_szgroup;
								 // a proportion i.e. ratio removals/available on node
		vector<double> impact_on_pops;
        vector<double> cumcatches_per_pop;
        vector<int> vid;		 // list of index of vessels currently on the node
		vector<int> pop_names_on_node;
								 // possibly per functional group
		vector<double> benthos_tot_biomass;
        vector<double> tariffs;

        static const vector<int> mUsualFGrounds;
        static const vector<double> mFreqUsualFGrounds;
        static const vector<int> mUsualFGroundsMet;
        static const vector<double> mFreqUsualFGroundsMet;
        static const multimap<int,int> mUsualMetiers;
        static const multimap<int,double> mFreqUsualMetiers;

        int m_nbpops;
        int m_nbbenthospops;
        int m_nszgrp;

        int area_type;

        pthread_mutex_t mutex;
};
#endif							 // NODE_H
