//
// Created by happycactus on 12/28/19.
//

#include "ModelLoader.h"
#include "Benthos.h"
#include "helpers.h"
#include "options.h"
#include "comstructs.h"
#include "SimModel.h"

struct ModelLoader::Impl {
    std::shared_ptr<SimModel> model;

    bool scenarioLoaded = false;
    bool configLoaded = false;
    bool nodesLoaded = false;

    bool doConsistencyTest = false;

    Impl(std::shared_ptr<SimModel> theModel)
            : model(theModel)
    {}
};

ModelLoader::ModelLoader(std::shared_ptr<SimModel> model)
        : p(spimpl::make_unique_impl<Impl>(model))
{

}

std::vector<Benthos *> ModelLoader::loadBenthos(PopSceOptions const &dyn_pop_sce,
                                                DynAllocOptions const &dyn_alloc_sce,
                                                std::string const &biolsce,
                                                std::string const &fleetsce)
{
    ensure(p->scenarioLoaded && p->nodesLoaded && p->configLoaded);

    auto loadedDataBenthos = loadBenthosData(dyn_pop_sce, dyn_alloc_sce, biolsce, fleetsce);

    int nbland = model().graph_point_code_landscape_unique().size();

    // creation of a vector of benthos shared (one benthos shared per landscape)
    auto benthoss = vector<Benthos *>(nbland);

    outc(cout << "nb of marine landscapes " << nbland << endl);

    // LOOP OVER BENTHOS OBJECT
    for (int landscape = 0; landscape < nbland; landscape++) {

        vector<double> init_meanw_funcgr_per_node;
        vector<double> init_prop_funcgr_number_per_node;
        vector<double> init_prop_funcgr_biomass_per_node;
        vector<double> init_benthos_number_carrying_capacity_K_per_landscape_per_funcgr;
        vector<double> init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr;
        vector<double> init_recovery_rates_per_funcgr;
        vector<double> init_h_betas_per_pop;

        int a_marine_landscape = model().graph_point_code_landscape_unique().at(landscape);

        outc(cout << "a marine landscape " << a_marine_landscape << endl);

        if (dyn_pop_sce.option(Options::modelBenthosInLongevity)) {
            multimap<int, double>::iterator lower_it_lgy = loadedDataBenthos.mmapidparam2.lower_bound(0);
            multimap<int, double>::iterator upper_it_lgy = loadedDataBenthos.mmapidparam2.upper_bound(0);

            vector<double> a_vector;
            for (multimap<int, double>::iterator pos = lower_it_lgy; pos != upper_it_lgy; pos++) {
                a_vector.push_back(pos->second);
            }


            if (a_vector.size() != model().config().nbbenthospops) {
                cout << "on node 0: " << " nb longevity class is " << a_vector.size() <<
                     ": error for benthos file: check the dims in longevity_classes_condition_per_node input file. kill, correct and re-run."
                     << endl;
                int aa;
                cin >> aa;
            }

            multimap<int, double>::iterator lower_landdd = loadedDataBenthos.mmapidparam3.lower_bound(
                    a_marine_landscape);
            multimap<int, double>::iterator upper_landdd = loadedDataBenthos.mmapidparam3.upper_bound(
                    a_marine_landscape);
            for (multimap<int, double>::iterator pos = lower_landdd; pos != upper_landdd; pos++) {
                outc(cout << pos->second << endl);
                // logistic recovery rates for this group specific to this landscape
                init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.push_back(pos->second);
            }
        } else {
            if (dyn_pop_sce.option(Options::modelBenthosInN)) {
                multimap<int, double>::iterator lower_land = loadedDataBenthos.mmapidparam4.lower_bound(
                        a_marine_landscape);
                multimap<int, double>::iterator upper_land = loadedDataBenthos.mmapidparam4.upper_bound(
                        a_marine_landscape);
                for (multimap<int, double>::iterator pos = lower_land; pos != upper_land; pos++) {
                    outc(cout << pos->second << endl);
                    // biomass per cell for this group specific to this landscape
                    init_prop_funcgr_number_per_node.push_back(pos->second);
                }

                multimap<int, double>::iterator lower_landddd = loadedDataBenthos.mmapidparam5.lower_bound(
                        a_marine_landscape);
                multimap<int, double>::iterator upper_landddd = loadedDataBenthos.mmapidparam5.upper_bound(
                        a_marine_landscape);
                for (multimap<int, double>::iterator pos = lower_landddd; pos != upper_landddd; pos++) {
                    outc(cout << pos->second << endl);
                    // logistic recovery rates for this group specific to this landscape
                    init_benthos_number_carrying_capacity_K_per_landscape_per_funcgr.push_back(pos->second);
                }


            } else {

                multimap<int, double>::iterator lower_land2 = loadedDataBenthos.mmapidparam6.lower_bound(
                        a_marine_landscape);
                multimap<int, double>::iterator upper_land2 = loadedDataBenthos.mmapidparam6.upper_bound(
                        a_marine_landscape);

                for (multimap<int, double>::iterator pos = lower_land2; pos != upper_land2; pos++) {
                    outc(cout << "check this: " << pos->second << endl);
                    // biomass per cell for this group specific to this landscape
                    init_prop_funcgr_biomass_per_node.push_back(pos->second);
                }


                if (init_prop_funcgr_biomass_per_node.size() != model().config().nbbenthospops) {
                    cout << a_marine_landscape << " nb funcgr is " << init_prop_funcgr_biomass_per_node.size() <<
                         ": error for benthos file: the file is likely to get an extra blank space here. stop, remove and rerun."
                         << endl;
                    int aa;
                    cin >> aa;
                }

                multimap<int, double>::iterator lower_landdd = loadedDataBenthos.mmapidparam3.lower_bound(
                        a_marine_landscape);
                multimap<int, double>::iterator upper_landdd = loadedDataBenthos.mmapidparam3.upper_bound(
                        a_marine_landscape);
                for (multimap<int, double>::iterator pos = lower_landdd; pos != upper_landdd; pos++) {
                    outc(cout << pos->second << endl);
                    // logistic recovery rates for this group specific to this landscape
                    init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.push_back(pos->second);
                }


            }
        }


        multimap<int, double>::iterator lower_land3 = loadedDataBenthos.mmapidparam1.lower_bound(a_marine_landscape);
        multimap<int, double>::iterator upper_land3 = loadedDataBenthos.mmapidparam1.upper_bound(a_marine_landscape);
        for (multimap<int, double>::iterator pos = lower_land3; pos != upper_land3; pos++) {
            outc(cout << pos->second << endl);
            // biomass per cell for this group specific to this landscape
            init_meanw_funcgr_per_node.push_back(pos->second);
        }


        multimap<int, double>::iterator lower_landd = loadedDataBenthos.mmapidparam7.lower_bound(a_marine_landscape);
        multimap<int, double>::iterator upper_landd = loadedDataBenthos.mmapidparam7.upper_bound(a_marine_landscape);
        for (multimap<int, double>::iterator pos = lower_landd; pos != upper_landd; pos++) {
            outc(cout << pos->second << endl);
            // logistic recovery rates for this group specific to this landscape
            init_recovery_rates_per_funcgr.push_back(pos->second);
        }

        multimap<int, double>::iterator lower_land2 = loadedDataBenthos.mmapidparam8.lower_bound(a_marine_landscape);
        multimap<int, double>::iterator upper_land2 = loadedDataBenthos.mmapidparam8.upper_bound(a_marine_landscape);
        for (multimap<int, double>::iterator pos = lower_land2; pos != upper_land2; pos++) {
            outc(cout << pos->second << endl);
            // habitat_deltas_per_pop specific to this landscape
            init_h_betas_per_pop.push_back(pos->second);
        }


        // add e.g. 2 functional groups per shared
        // and init with an arbitrary biomass.
        // init_biomass will be distributed evenly among nodes
        // belonging to this particular landscape

        benthoss[landscape] = new Benthos(landscape,
                                          a_marine_landscape,
                                          p->model->config().nbbenthospops,
                                          p->model->nodes(),
                                          init_prop_funcgr_biomass_per_node,
                                          init_prop_funcgr_number_per_node,
                                          init_meanw_funcgr_per_node,
                                          init_recovery_rates_per_funcgr,
                                          init_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr,
                                          init_benthos_number_carrying_capacity_K_per_landscape_per_funcgr,
                                          p->model->is_benthos_in_numbers(),
                                          p->model->is_benthos_in_longevity_classes(),
                                          init_h_betas_per_pop,
                                          loadedDataBenthos.mmapidparam2
        );
        //out(cout << "marine landscape for this benthos shared is " << benthoss.at(landscape)->get_marine_landscape() << endl);
        //out(cout <<"...and the biomass this node this func. grp is "  << benthoss.at(landscape)-> get_list_nodes().at(0)-> get_benthos_tot_biomass(0) << endl);

    }

    if (p->doConsistencyTest) {
        doBenthosConsistencyTest(benthoss);
    }

    return benthoss;
}

void ModelLoader::doBenthosConsistencyTest(std::vector<Benthos *> const &benthoss)
{
    // check
    for (unsigned int a_idx = 0; a_idx < p->model->nodes().size(); a_idx++) {
        dout(cout << "this node " << p->model->nodes().at(a_idx)->get_idx_node().toIndex() <<
                  " nb func. gr. " << p->model->nodes().at(a_idx)->get_benthos_tot_biomass().size() << endl);

        if (p->model->nodes().at(a_idx)->get_benthos_tot_biomass().size() != model().config().nbbenthospops) {
            cout
                    << "something wrong for benthos_tot_biomass here!...kill displace.exe and check consistency in landscape coding and benthos input files before trying again"
                    << endl;
            throw ConsistencyTestFailed("benthos_tot_biomass");
        }
    }

    // check the area distribution for benthos shared 0
    //vector<Node* > some_nodes= benthoss.at(0)-> get_list_nodes();
    //for(int a_idx=0; a_idx<some_nodes.size(); a_idx++){
    //    cout << some_nodes.at(a_idx)->get_idx_node() << endl;
    //}

    // check the biomasses
    vector<double> a_prop_funcgr_per_node = benthoss[0]->get_prop_funcgr_biomass_per_node();
    outc(cout << "check biomass per func. gr. for benthos shared 0  " << endl);
    for (unsigned int gr = 0; gr < a_prop_funcgr_per_node.size(); gr++) {
        outc(cout << a_prop_funcgr_per_node[gr] << " ");
    }
    outc(cout << endl);

    // check the biomasses for benthos shared 0 on the first node for the
    // first functional group
    //cout <<"...and the biomass this node this func. grp is "  <<
    //     benthoss.at(0)-> get_list_nodes().at(0)-> get_benthos_tot_biomass(0) << endl;

    //cout <<"...and the biomass this node this func. grp is "  <<
    //     benthoss.at(0)-> get_list_nodes().at(100)-> get_benthos_tot_biomass(1) << endl;

    //cout <<"...and the biomass this node this func. grp is "  <<
    //     benthoss.at(4)-> get_list_nodes().at(100)-> get_benthos_tot_biomass(1) << endl;

}

bool ModelLoader::doNodesAndGraphConsistencyTest()
{
    return true;
}

bool ModelLoader::loadConfig()
{
    auto config = std::make_unique<displace::commons::Config>();
    p->configLoaded = loadConfigImpl(config->nbpops,
                                     config->nbbenthospops,
                                     config->implicit_pops,
                                     config->implicit_pops_level2,
                                     config->grouped_tacs,
                                     config->nbcp_coupling_pops,
                                     config->calib_oth_landings,
                                     config->calib_weight_at_szgroup,
                                     config->calib_cpue_multiplier,
                                     config->interesting_harbours);
    p->model->setConfig(std::move(config));
    return p->configLoaded;
}

bool ModelLoader::loadScenario()
{
    auto scenario = std::make_unique<displace::commons::Scenario>();
    p->scenarioLoaded = loadScenarioImpl(*scenario);
    if (p->scenarioLoaded) {
        p->model->setScenario(std::move(scenario));
    }
    return p->scenarioLoaded;
}

bool ModelLoader::loadNodesAndGraphs()
{
    if (!loadNodesAndGraphsDataImpl()) {
        return false;
    }

    if (!doNodesAndGraphConsistencyTest()) {
        return false;
    }

    p->nodesLoaded = true;
    return true;
}

SimModel &ModelLoader::model()
{
    return *p->model;
}

SimModel const &ModelLoader::model() const
{
    return *p->model;
}

std::string ModelLoader::quarterString() const
{
    return std::to_string(model().quarter());
}

std::string ModelLoader::monthString() const
{
    return std::to_string(model().month());
}

std::string ModelLoader::semesterString() const
{
    return std::to_string(model().semester());
}
