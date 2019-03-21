// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <idtypes.h>

#include<vector>
#include <commons_global.h>
#include <algorithm>
#include<string>
#include<map>
#include "myutils.h"
#include "myRutils.h"

#include <mutex>


using namespace std;

class COMMONSSHARED_EXPORT Node
{
	public:

		/**  constructor */
		Node ();
        Node (types::NodeId idx_node, double xval, double yval, int _harbour, int _code_area,
              int _marine_landscape, double _marine_landscape_norm, double _marine_landscape_alpha,
              double _wind, double _wind_norm, double _wind_alpha,
              double _sst, double sst_norm, double sst_alpha,
              double _salinity, double _salinity_norm, double _salinity_alpha,
              double _Nitrogen, double _Nitrogen_norm, double _Nitrogen_alpha,
              double _Phosphorus, double _Phosphorus_norm, double _Phosphorus_alpha,
              double _Oxygen, double _Oxygen_norm, double _Oxygen_alpha,
              double _DissolvedCarbon, double _DissolvedCarbon_norm, double _DissolvedCarbon_alpha,
              double _bathymetry, double _shippingdensity, double _siltfraction,
              double _benthos_biomass, double _benthos_number, double _benthos_meanweight, double _benthos_biomass_K, double _benthos_number_K,
              int nbpops, int nbbenthospops,  int nbszgroups);
        /*
        Node (types::NodeId idx_node, const vector<double> &graph_coord_x, const vector<double> &graph_coord_y,
            const vector<int> &graph_coord_harbour,
            const vector<int> &graph_point_code_area,
            const vector<int> &graph_marine_landscape,
            const vector<double> &graph_marine_wind,
            const vector<double> &graph_marine_sst,
            const vector<double> &graph_marine_salinity,
            const vector<double> &graph_benthos_biomass,
            const vector<double> &graph_benthos_number,
            double initmw,
            int nbpops,  int nbbenthospops, int nbszgroups);*/

		/**  destructor */
		~Node();

        void lock() { mutex.lock(); }
        void unlock() { mutex.unlock(); }

        void setNodeIdx(types::NodeId id) { idx_node = id; }

        types::NodeId get_idx_node() const;
		int get_code_area() const;
        void setCodeArea(int id) {
            code_area = id;
        }
        double get_init_benthos_biomass() const;
        vector<double> get_benthos_biomass_per_funcgr() const;
        void setBenthosBiomass(double value) {
        benthos_biomass = value;
        }
        double get_init_benthos_number() const;
        vector<double> get_benthos_number_per_funcgr() const;
        void setBenthosNumber(double value) {
        benthos_number = value;
        }
        double get_init_benthos_meanweight() const;
        vector<double> get_benthos_meanweight_per_funcgr() const;
        void setBenthosMeanweight(double value) {
        benthos_meanweight = value;
        }

        int get_marine_landscape() const;
        double get_marine_landscape_norm() const;
        double get_marine_landscape_alpha() const;
        void setMarineLandscape(int id) {
            marine_landscape = id;
        }
        void setMarineLandscapeNorm(double ml_n) {
            marine_landscape_norm = ml_n;
        }
        void setMarineLandscapeAlpha(double ml_a) {
            marine_landscape_alpha = ml_a;
        }

        double get_wind() const;
        double get_wind_norm() const;
        double get_wind_alpha() const;
        void setWind(double w) {
            wind = w;
        }
        void setWindNorm(double w_n) {
            wind_norm = w_n;
        }
        void setWindAlpha(double w_a) {
            wind_alpha = w_a;
        }

        double get_sst() const;
        double get_sst_norm() const;
        double get_sst_alpha() const;
        void setSST(double t) {
            sst = t;
        }
        void setSSTNorm(double t_n) {
            sst_norm = t_n;
        }
        void setSSTAlpha(double t_a) {
            sst_alpha = t_a;
        }

        double get_salinity() const;
        double get_salinity_norm() const;
        double get_salinity_alpha() const;
        void setSalinity(double s) {
            salinity = s;
        }
        void setSalinityNorm(double s_n) {
            salinity_norm = s_n;
        }
        void setSalinityAlpha(double s_a) {
            salinity_alpha = s_a;
        }

        double get_Nitrogen() const;
        double get_Nitrogen_norm() const;
        double get_Nitrogen_alpha() const;
        void setNitrogen(double N) {
            Nitrogen = N;
        }
        void setNitrogenNorm(double N_n) {
            Nitrogen_norm = N_n;
        }
        void setNitrogenAlpha(double N_a) {
            Nitrogen_alpha = N_a;
        }

        double get_Phosphorus() const;
        double get_Phosphorus_norm() const;
        double get_Phosphorus_alpha() const;
        void setPhosphorus(double P) {
            Phosphorus = P;
        }
        void setPhosphorusNorm(double P_n) {
            Phosphorus_norm = P_n;
        }
        void setPhosphorusAlpha(double P_a) {
            Phosphorus_alpha = P_a;
        }

        double get_Oxygen() const;
        double get_Oxygen_norm() const;
        double get_Oxygen_alpha() const;
        void setOxygen(double O) {
            Oxygen = O;
        }
        void setOxygenNorm(double O_n) {
            Oxygen_norm = O_n;
        }
        void setOxygenAlpha(double O_a) {
            Oxygen_alpha = O_a;
        }

        double get_DissolvedCarbon() const;
        double get_DissolvedCarbon_norm() const;
        double get_DissolvedCarbon_alpha() const;
        void setDissolvedCarbon(double dissC) {
            DissolvedCarbon = dissC;
        }
        void setDissolvedCarbonNorm(double dissC_n) {
            DissolvedCarbon_norm = dissC_n;
        }
        void setDissolvedCarbonAlpha(double dissC_a) {
            DissolvedCarbon_alpha = dissC_a;
        }

        double get_bathymetry() const;
        void setBathymetry(double depth) {
            bathymetry = depth;
        }

        double get_shippingdensity() const;
        void setShippingdensity(double density) {
            shippingdensity = density;
        }

        double get_siltfraction() const;
        void setSiltfraction(double fraction) {
            siltfraction = fraction;
        }

        // declare virtual to enable dynamic binding for chlidren classes e.g. Harbour
		virtual string get_name() const;
		//virtual double get_prices(string met, int pop) ; // declare virtual to enable dynamic binding for chlidren classes e.g. Harbour
								 // declare virtual to enable dynamic binding for chlidren classes e.g. Harbour
		virtual double get_prices_per_cat(int pop, int cat) ;
								 // declare virtual to enable dynamic binding for chlidren classes e.g. Harbour
        virtual double get_fuelprices(int vsize) ;
        virtual const vector<types::NodeId> &get_usual_fgrounds() const;
        virtual const vector<double> &get_freq_usual_fgrounds () const;
        virtual void set_usual_fgrounds (const vector<types::NodeId> &usual_fgrounds);
        virtual void set_freq_usual_fgrounds (const vector<double> &_freq_usual_fgrounds);
        virtual const multimap<types::NodeId, int> &get_usual_metiers() const;
        virtual const multimap<int, double> &get_freq_usual_metiers() const;
        virtual void set_usual_metiers (multimap <types::NodeId,int> usual_metiers);
        virtual void set_freq_usual_metiers (multimap <int,double> freq_usual_metiers);
        virtual void set_usual_fgrounds_per_met (multimap <int,types::NodeId> usual_fgrounds_per_met);
        virtual void set_freq_usual_fgrounds_per_met (multimap <int,double> freq_usual_fgrounds_per_met);
        virtual vector<types::NodeId> get_usual_fgrounds_this_met(int met);
        virtual vector<double> get_freq_usual_fgrounds_this_met(int met);


        double get_x() const;
		double get_y() const;
		int get_harbour() const;
		bool get_is_harbour() const;
        const vector<double> &get_Ns_pops_at_szgroup(int name_pop) const;
        vector<double> get_Ns_pops_at_szgroup_at_month_start(int name_pop) const;
        vector<double> get_removals_pops_at_szgroup(int name_pop) const;
        vector<double> get_last_oth_catch_pops_at_szgroup(int name_pop) const;
        vector<double> get_last_oth_disc_pops_at_szgroup(int name_pop) const;
        vector<double> get_pressure_pops_at_szgroup(int name_pop) const;
        vector<double> get_avai_pops_at_selected_szgroup(int name_pop) const;
        int get_benthos_id() const;
        double get_benthos_tot_biomass(int funcgr) const;
        const vector<double>& get_benthos_tot_biomass() const;
        double get_benthos_tot_number(int funcgr) const;
        const vector<double>& get_benthos_tot_number() const;
        double get_benthos_tot_meanweight(int funcgr) const;
        const vector<double>& get_benthos_tot_meanweight() const;
        double get_benthos_tot_number_K(int funcgr) const;
        const vector<double>& get_benthos_tot_number_K() const;
        double get_benthos_tot_biomass_K(int funcgr) const;
        const vector<double>& get_benthos_tot_biomass_K() const;
        const vector<double>& get_h_betas_per_pop() const;
        double get_tariffs(int type) const;
        const vector<double>& get_tariffs() const;
        const vector< vector<double> >  &get_Ns_pops_at_szgroup() const;
		vector<int> get_vid();
		vector<int> get_pop_names_on_node();
        vector<int> get_ff_names_on_node();
        const vector<double>& get_totNs_per_pop ();
        const vector<double>& get_totWs_per_pop ();
        const vector<double>& get_impact_per_pop ();
        const vector<double>& get_cumcatches_per_pop ();
        const vector<double>& get_cumdiscards_per_pop ();
        int get_cumftime() const;
        double get_cumsweptarea() const;
        double get_cumsubsurfacesweptarea() const;
        double get_cumcatches() const;
        double get_cumcatches_with_threshold() const;
        double get_cumdiscards() const;
        double get_cumdiscardsratio() const;
        double get_nbchoked() const;
        void set_xy(double xval, double yval);
		void init_Ns_pops_at_szgroup(int nbpops, int nbszgroups);
        void set_Ns_pops_at_szgroup(int name_pop, const vector<double>& val);
        void set_Ns_pops_at_szgroup_at_month_start(int name_pop, const vector<double>& val);        
        void set_removals_pops_at_szgroup(int name_pop, const vector<double>& val);
        void set_last_oth_catch_pops_at_szgroup(int name_pop, const vector<double>& val);
        void set_last_oth_disc_pops_at_szgroup(int name_pop, const vector<double>& val);
        void set_pressure_pops_at_szgroup(int name_pop, const vector<double>& val);
		void init_avai_pops_at_selected_szgroup(int nbpops, int selected_nbszgroups);
        void set_avai_pops_at_selected_szgroup(int name_pop, const vector<double>& val);
        void set_totNs_per_pop(int name_pop, double val);
        void set_totWs_per_pop(int name_pop, double val);
        void set_impact_per_pop(int name_pop, double val);
        void set_cumcatches_per_pop(int name_pop, double val);
        void set_cumdiscards_per_pop(int name_pop, double val);
        void set_vid(int val);
        void set_cumftime(int tot);
        void set_cumsweptarea(double tot);
        void set_cumsubsurfacesweptarea(double tot);
        void set_cumcatches(double tot);
        void set_cumcatches_with_threshold(double tot);
        void set_cumdiscards(double tot);
        void set_cumdiscardsratio(double rat);
        void set_nbchoked(double val);
        void add_to_cumftime(int delta_time);
        void add_to_cumsweptarea(double sweptarea);
        void add_to_cumsubsurfacesweptarea(double subsurfacesweptarea);
        void add_to_sweptarea(int sweptarea);
        void add_to_cumcatches(double catches);
        void add_to_cumcatches_per_pop(double catches, int pop);
        void add_to_cumdiscards(double discards);
        void add_to_cumdiscards_per_pop(double discards, int pop);
        void set_pop_names_on_node(int name_pop);
        void set_ff_names_on_node(int name_ff);
        void set_benthos_id(int id);
        void set_benthos_tot_biomass(int funcgr, double value);
        void set_benthos_tot_number(int funcgr, double value);
        void set_benthos_tot_meanweight(int funcgr, double value);
        void set_benthos_tot_biomass_K(int funcgr, double value);
        void set_benthos_tot_number_K(int funcgr, double value);
        void set_benthos_biomass_per_funcgr(vector<double> benthos);
        void set_tariffs(vector<double> values);
        void set_tariffs(int type, double value);
        void clear_pop_names_on_node();
        void remove_pop_names_on_node(int name_pop);
		void clear_Ns_pops_at_szgroup();
        void clear_removals_pops_at_szgroup(int pop);
        void clear_last_oth_catch_pops_at_szgroup(int pop);
        void clear_last_oth_disc_pops_at_szgroup(int pop);
        void clear_avai_pops_at_selected_szgroup();
        void clear_totNs_per_pop();
        void clear_totWs_per_pop();
        void clear_impact_per_pop();
        void clear_cumcatches_per_pop();
        void clear_cumdiscards_per_pop();
        void apply_natural_mortality_at_node(int name_pop,  const vector<double>& M_at_szgroup, vector<double>& prop_M_from_species_interactions);
        void apply_natural_mortality_at_node_from_size_spectra_approach(int name_pop,
                                                                        const vector<vector<double> > & Ws_at_szgroup,
                                                                        const vector<vector<vector<vector<double> > > > & predkernel,
                                                                        const vector<vector<double> > & searchVolMat,
                                                                        const vector<vector<double> > & juveniles_diet_preference,
                                                                        const vector<vector<double> > & adults_diet_preference,
                                                                        const vector<int> &mat_cats);
        void apply_oth_land(int name_pop, double &oth_land_this_pop_this_node, const vector<double>&  weight_at_szgroup, const vector<double>& totN, int will_I_discard_all, vector<vector<double> > &selectivity_per_stock_ogives_for_oth_land);
		void export_popnodes(ofstream& popnodes, multimap<int,double> weight_at_szgroup, int tstep);
		void export_popnodes_impact(ofstream& popnodes, int tstep, int pop);
		void export_popnodes_impact_per_szgroup(ofstream& popnodes, int tstep, int pop);
        void export_benthos_tot_biomass_per_funcgroup(ofstream& benthosbiomassnodes, int tstep, int funcgroup);
        void export_benthos_tot_number_per_funcgroup(ofstream& benthosnumbernodes, int tstep, int funcgroup);
        void export_popnodes_cumftime(ofstream& popnodes, int tstep);
        void export_popnodes_cumsweptarea(ofstream& popnodes, int tstep);
        void export_popnodes_cumcatches(ofstream& popnodes, int tstep);
        void export_popnodes_cumcatches_with_threshold(ofstream& popnodes, int tstep, int threshold);
        void export_popnodes_cumdiscards(ofstream& popnodes, int tstep);
        void export_popnodes_cumdiscardsratio(ofstream& popnodes, int tstep);
        void export_popnodes_nbchoked(ofstream& popnodes, int tstep);
        void export_popnodes_cumulcatches_per_pop(ofstream& popnodes, int tstep, int pop);
        void export_nodes_envt(ofstream& nodes_event, int tstep);
        void export_popnodes_tariffs(ofstream& popnodes, int tstep);
        void add_benthos_tot_biomass_on_node(double tot_biomass_this_group);
        void add_benthos_tot_number_on_node(double tot_number_this_group);
        void add_benthos_tot_meanweight_on_node(double meanweight_this_group);
        void add_benthos_tot_biomass_K_on_node(double tot_biomass_K_this_group);
        void add_benthos_tot_number_K_on_node(double tot_number_K_this_group);

        void setAreaType(int _area_type);
        int evaluateAreaType();

        int get_nbpops() const { return m_nbpops; }
        int get_nbbenthospops() const { return m_nbbenthospops; }
        int get_nszgroups() const { return m_nszgrp; }

        void set_is_harbour(int id);
        void set_idx_node(types::NodeId idx);

        void setBannedMetier(int metier) {
            while (mBannedMetiers.size() <= (size_t)metier)
                mBannedMetiers.push_back(false);
            mBannedMetiers[metier] = true;
        }

        void setBannedVsize(int vsize) {
            while (mBannedVsizes.size() <= (size_t)vsize)
                mBannedVsizes.push_back(false);
            mBannedVsizes[vsize] = true;
        }

      void setNbOfDaysClosedPerMonth (int metier, double NbOfDaysClosedPerMonth) {
          while (mNbDaysClosedForBannedMetiers.size() <= (size_t)metier)
              mNbDaysClosedForBannedMetiers.push_back(0.0);
          mNbDaysClosedForBannedMetiers[metier] = NbOfDaysClosedPerMonth;
      }



       bool isMetierBanned(int metier) const {
            if ((size_t)metier < mBannedMetiers.size())
                return mBannedMetiers[metier];
            return false;
        }

        bool isVsizeBanned(int vsize) const {
            if ((size_t)vsize < mBannedVsizes.size())
                return mBannedVsizes[vsize];
            return false;
        }

        double getNbOfDaysClosed(int metier) const {
            if ((size_t)metier < mNbDaysClosedForBannedMetiers.size())
                return mNbDaysClosedForBannedMetiers[metier];
            return 31.0;
        }

protected:
        void reinit(vector<double> &vec, unsigned int sz);
        void reinit(vector<vector<double> > &vec, unsigned int sz, unsigned int subsz);
private:
        types::NodeId idx_node;
		double x;
		double y;
		int harbour;
		int code_area;
		int marine_landscape;
        double  marine_landscape_norm,  marine_landscape_alpha;
        double wind, wind_norm, wind_alpha;
        double sst, sst_norm, sst_alpha;
        double salinity, salinity_norm, salinity_alpha;
        double Nitrogen, Nitrogen_norm, Nitrogen_alpha;
        double Phosphorus, Phosphorus_norm, Phosphorus_alpha;
        double Oxygen, Oxygen_norm, Oxygen_alpha;
        double DissolvedCarbon, DissolvedCarbon_norm, DissolvedCarbon_alpha;
        double bathymetry;
        double shippingdensity;
        double siltfraction;
        int benthos_id;
        double benthos_biomass;  // total bio on node from the GIS graph file
        double benthos_number;  // total bio on node from the GIS graph file
        double benthos_meanweight;  // total bio on node from the GIS graph file
        double benthos_biomass_K;
        double benthos_number_K;
        bool is_harbour;
		int cumftime;
        double cumsweptarea;
        double cumsubsurfacesweptarea;
        double cumcatches;
        double cumcatches_with_threshold;
        double cumdiscards;
        double cumdiscardsratio;
        double nbchoked;
        vector< vector<double> > Ns_pops_at_szgroup;
		vector< vector<double> > Ns_pops_at_szgroup_at_month_start;
		vector< vector<double> > removals_pops_at_szgroup;
        vector< vector<double> > last_oth_catch_pops_at_szgroup;
        vector< vector<double> > last_oth_disc_pops_at_szgroup;
        vector< vector<double> > pressure_pops_at_szgroup;
		vector< vector<double> > avai_pops_at_selected_szgroup;
								 // a proportion i.e. ratio removals/available on node
        vector<double> totNs_per_pop;
        vector<double> totWs_per_pop;
        vector<double> impact_per_pop;
        vector<double> cumcatches_per_pop;
        vector<double> cumdiscards_per_pop;
        vector<int> vid;		 // list of index of vessels currently on the node
		vector<int> pop_names_on_node;
        vector<int> ff_names_on_node;

        vector<double> benthos_tot_biomass;  // total bio on node per funcgr from sharing benthos_biomass per funcgr (this sharing is specific to Benthos landscape)
        vector<double> benthos_tot_number;  // total bio on node per funcgr from sharing benthos_number per funcgr (this sharing is specific to Benthos landscape)
        vector<double> benthos_tot_meanweight;  // meanweight on node per funcgr (this sharing is specific to Benthos landscape)
        vector<double> benthos_tot_biomass_K;  // vect over funcgrs
        vector<double> benthos_tot_number_K;  // vect over funcgrs

        vector<double> tariffs;

        vector<bool> mBannedMetiers;
        vector<bool> mBannedVsizes;
        vector<double> mNbDaysClosedForBannedMetiers;

        static const vector<types::NodeId> mUsualFGrounds;
        static const vector<double> mFreqUsualFGrounds;
        static const vector<types::NodeId> mUsualFGroundsMet;
        static const vector<double> mFreqUsualFGroundsMet;
        static const multimap<types::NodeId,int> mUsualMetiers;
        static const multimap<int,double> mFreqUsualMetiers;

        int m_nbpops;
        int m_nbbenthospops;
        int m_nszgrp;

        int area_type;

        std::mutex mutex;
};
#endif							 // NODE_H
