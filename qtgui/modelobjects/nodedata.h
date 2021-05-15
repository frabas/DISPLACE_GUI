/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef NODEDATA_H
#define NODEDATA_H

#include <Node.h>

#include <QList>
#include <QVector>

#include <boost/optional.hpp>

#include <memory>

namespace types {
struct NodesPopData;
struct NodesBenthosData;
}

class DisplaceModel;

/** \brief an extension class for Node
 *
 * */
class NodeData : public std::enable_shared_from_this<NodeData>
{
public:
    struct Edge {
        Edge()
            : source(), target(), weight(0.0) {
        }

        Edge(std::shared_ptr<NodeData> src, std::shared_ptr<NodeData> nd, double wt)
            : source(src), target(nd), weight(wt) {
        }

        std::weak_ptr<NodeData> source;
        std::weak_ptr<NodeData> target;
        double weight;
        bool highlighted = false;
    };

    typedef QVector<std::shared_ptr<Edge>> AdiacencyList;

    std::shared_ptr<Node> mNode;    /* This is crap. But we have no other choice */

    explicit NodeData(std::shared_ptr<Node> nd, DisplaceModel *model);
    virtual ~NodeData();

    /* Proxy functions to avoid changing too much code */
    types::NodeId get_idx_node() const { return mNode->get_idx_node(); }
    int get_marine_landscape() const { return mNode->get_marine_landscape(); }
    vector<int> get_ff_names_on_node() const { return mNode->get_ff_names_on_node(); }
    double get_wind() const { return mNode->get_wind(); }
    double get_sst() const { return mNode->get_sst(); }
    double get_salinity() const { return mNode->get_salinity(); }
    double get_Nitrogen() const { return mNode->get_Nitrogen(); }
    double get_Phosphorus() const { return mNode->get_Phosphorus(); }
    double get_Oxygen() const { return mNode->get_Oxygen(); }
    double get_DissolvedCarbon() const { return mNode->get_DissolvedCarbon(); }
    double get_bathymetry() const { return mNode->get_bathymetry(); }
    double get_shippingdensity() const { return mNode->get_shippingdensity(); }
    double get_siltfraction() const { return mNode->get_siltfraction(); }
    double get_icesrectanglecode() const { return mNode->get_icesrectanglecode(); }
    double get_init_benthos_biomass() const { return mNode->get_init_benthos_biomass(); }
    vector<double> get_benthos_biomass_per_funcgr() const { return mNode->get_benthos_biomass_per_funcgr(); }
    double get_init_benthos_number() const { return mNode->get_init_benthos_number(); }
    vector<double> get_benthos_number_per_funcgr() const { return mNode->get_benthos_number_per_funcgr(); }
    double get_init_benthos_meanweight() const { return mNode->get_init_benthos_meanweight(); }
    vector<double> get_benthos_meanweight_per_funcgr() const { return mNode->get_benthos_meanweight_per_funcgr(); }
    void setMarineLandscape(int id) { mNode->setMarineLandscape(id); }
    void setWind(double w) { mNode->setWind(w); }
    void setSST(double t) { mNode->setSST(t); }
    void setSalinity(double s) { mNode->setSalinity(s); }
    void setNitrogen(double n) { mNode->setNitrogen(n); }
    void setPhosphorus(double p) { mNode->setPhosphorus(p); }
    void setOxygen(double o) { mNode->setOxygen(o); }
    void setDissolvedCarbon(double dc) { mNode->setDissolvedCarbon(dc); }
    void setBathymetry(double b) { mNode->setBathymetry(b); }
    void setShippingdensity(double val) { mNode->setShippingdensity(val); }
    void setSiltfraction(double val) { mNode->setSiltfraction(val); }
    void setIcesrectanglecode(double val) { mNode->setIcesrectanglecode(val); }
    void setBenthosBiomass(double val) { mNode->setBenthosBiomass(val); }
    void setBenthosNumber(double val) { mNode->setBenthosNumber(val); }
    void setBenthosMeanweight(double val) { mNode->setBenthosMeanweight(val); }


      int get_code_area() const { return mNode->get_code_area(); }
    void setCodeArea(int id) { mNode->setCodeArea(id); }

    virtual string get_name() const { return mNode->get_name(); }
    double get_x() const { return mNode->get_x(); }
    double get_y() const { return mNode->get_y(); }
    int get_harbour() const { return mNode->get_harbour(); }
    bool get_is_harbour() const { return mNode->get_is_harbour(); }
    int get_cumftime() const { return mNode->get_cumftime(); }
    void set_cumftime(int i) { mNode->set_cumftime(i); }
    double get_cumsweptarea() const { return mNode->get_cumsweptarea(); }
    void set_cumsweptarea(int i) { mNode->set_cumsweptarea(i); }
    double get_cumsubsurfacesweptarea() const { return mNode->get_cumsubsurfacesweptarea(); }
    void set_cumsubsurfacesweptarea(int i) { mNode->set_cumsubsurfacesweptarea(i); }
    double get_cumcatches() const { return mNode->get_cumcatches(); }
    double get_cumcatches_with_threshold() const { return mNode->get_cumcatches_with_threshold(); }
    double get_cumdiscards() const { return mNode->get_cumdiscards(); }
    double get_cumdiscardsratio() const { return mNode->get_cumdiscardsratio(); }
    double get_nbchoked() const { return mNode->get_nbchoked(); }
    void set_cumcatches(double v) { mNode->set_cumcatches(v); }
    void set_cumcatches_with_threshold(int v) { mNode->set_cumcatches_with_threshold(v); }
    void set_cumdiscards(double v) { mNode->set_cumdiscards(v); }
    void set_cumdiscardsratio(double v) { mNode->set_cumdiscardsratio(v); }
    void set_nbchoked(double v) { mNode->set_nbchoked(v); }

    void set_totNs_per_pop (int pop, int v){ mNode->set_totNs_per_pop(pop, v); }
    void set_totWs_per_pop(int pop, int v){ mNode->set_totWs_per_pop(pop, v); }
    void set_cumcatches_per_pop(int pop, int v){ mNode->set_cumcatches_per_pop(pop, v); }
    void set_cumdiscards_per_pop(int pop, int v){ mNode->set_cumdiscards_per_pop(pop, v); }
    void set_impact_per_pop(int pop, int v){ mNode->set_impact_per_pop(pop, v); }


    vector<double> get_tariffs() const { return mNode->get_tariffs(); }
    void set_tariffs(vector<double> tariffs) { mNode->set_tariffs(tariffs); }
    void set_tariffs(int met, double tariff) { mNode->set_tariffs(met, tariff); }
    const vector<types::NodeId> &get_usual_fgrounds() { return mNode->get_usual_fgrounds(); }
    void set_usual_fgrounds(vector<types::NodeId> usual_fgrounds) { mNode->set_usual_fgrounds(usual_fgrounds); }
    const vector<double> &get_freq_usual_fgrounds() { return mNode->get_freq_usual_fgrounds(); }
    void set_freq_usual_fgrounds(vector<double> freq_usual_fgrounds) { mNode->set_freq_usual_fgrounds(freq_usual_fgrounds); }
    int evaluateAreaType() { return mNode->evaluateAreaType(); }


    /* Extending functions */

    DisplaceModel *getModel() const { return mModel; }

    int getPopCount() const;
    int getMetCount() const;
    int getBenthosPopCount() const;
    [[deprecated]]
    void setPop(int pop, double v);
    [[deprecated]]
    void setPopTot(double tot);
    [[deprecated]]
    void setPop(QList<double> v, double tot);
    boost::optional<double> getPop(int pop) const ;
    boost::optional<double> getPopTot () const;

    [[deprecated]]
    void setPopW(int pop, double val);
    [[deprecated]]
    void setPopWTot(double tot);
    [[deprecated]]
    void setPopW(QList<double> v, double tot);
    boost::optional<double> getPopW(int pop) const ;
    double getPopWTot () const;

    [[deprecated]]
    void setImpact(int pop, double impact);
    boost::optional<double> getImpact(int pop) const;

    [[deprecated]]
    void setCumcatchesPerPop(int pop, double cumcatchesperpop);
    boost::optional<double> getCumcatchesPerPop(int pop);

    [[deprecated]]
    void setCumdiscardsPerPop(int pop, double cumdiscardsperpop);
    double getCumdiscardsPerPop(int pop);

    [[deprecated]] void setBenthosBiomass(int func, double benthosbiomass);
    double getBenthosBiomass(int func) const;
    [[deprecated]] void setBenthosNumber(int func, double benthosnumber);
    double getBenthosNumber(int func) const;
    [[deprecated]] void setBenthosMeanweight(int func, double benthosmeanweight);
    double getBenthosMeanweight(int func) const ;
    [[deprecated]] void setBenthosBiomassOverK(int func, double benthosbiomassoverK);
    double getBenthosBiomassOverK(int func) const ;
    [[deprecated]] void setBenthosNumberOverK(int func, double benthosnumberoverK);
    double getBenthosNumberOverK(int func) const;

    void setFishfarmFishMeanWeight(int farmid, double meanw_kg);
    double getFishfarmFishMeanWeight(int farm) const {
        return mFishfarmMeanweight[farm];
    }

    void setFishfarmFishHarvestedKg(int farmid, double fish_harvested_kg);
    double getFishfarmFishHarvestedKg(int farm) const {
        return mFishfarmFishHarvestedKg[farm];
    }

    void setFishfarmEggsHarvestedKg(int farmid, double eggs_harvested_kg);
    double getFishfarmEggsHarvestedKg(int farm) const {
        return mFishfarmEggsHarvestedKg[farm];
    }

    void setFishfarmAnnualProfit(int farmid, double fishfarm_annualprofit);
    double getFishfarmAnnualProfit(int farm) const {
        return mFishfarmAnnualProfit[farm];
    }

    void setFishfarmNetDischargeN(int farmid, double fishfarm_netdischargeN);
    double getFishfarmNetDischargeN(int farm) const {
        return mFishfarmNetDischargeN[farm];
    }

    void setFishfarmNetDischargeP(int farmid, double fishfarm_netdischargeP);
    double getFishfarmNetDischargeP(int farm) const {
        return mFishfarmNetDischargeP[farm];
    }

    void setFishfarmCumulNetDischargeN(int farmid, double fishfarm_cumulnetdischargeN);
    double getFishfarmCumulNetDischargeN(int farm) const {
        return mFishfarmCumulNetDischargeN[farm];
    }

    void setFishfarmCumulNetDischargeP(int farmid, double fishfarm_cumulnetdischargeP);
    double getFishfarmCumulNetDischargeP(int farm) const {
        return mFishfarmCumulNetDischargeP[farm];
    }

    int getHarbourId() const;
    void setHarbourId(int value);

    /* Adiacency functions */
    int appendAdiancency(std::shared_ptr<NodeData> target, double weight);
    void removeAdiacencyByIdx(int idx);
    void removeAdiacencyByTarget(std::shared_ptr<NodeData> target);
    void removeAllAdiacencies();
    int getAdiacencyCount() const;
    std::shared_ptr<Edge> getAdiacencyByIdx(int idx) const;
    double getAdiacencyWeight(int idx) const;
    void setAdiacencyWeight (int idx, double w);
    void setAreaType (int value);

    bool isDeleted() const;
    void setDeleted(bool value = true);

    void setRelevant(bool relevant = true) { mRelevant = relevant; }
    bool isRelevant() const { return mRelevant; }

private:
    DisplaceModel *mModel;
    bool mDeleted;
    bool mRelevant;
    int mHarbourId;

    double *mFishfarmMeanweight;
    double *mFishfarmFishHarvestedKg;
    double *mFishfarmEggsHarvestedKg;
    double *mFishfarmAnnualProfit;
    double *mFishfarmNetDischargeN;
    double *mFishfarmNetDischargeP;
    double *mFishfarmCumulNetDischargeN;
    double *mFishfarmCumulNetDischargeP;

    std::shared_ptr<types::NodesPopData> getNodesData() const;
    std::shared_ptr<types::NodesBenthosData> getBenthosData() const;

    int areaType;

    AdiacencyList mAdiacency;
};

#endif // NODEDATA_H
