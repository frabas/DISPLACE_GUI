//
// Created by happycactus on 1/10/20.
//

#include "SimModel.h"
#include "comstructs.h"
#include "shortestpath/GeoGraph.h"

struct SimModel::Impl {
    int month = 0, quarter = 0, semester = 0;

    std::unique_ptr<displace::commons::Config> config;
    std::unique_ptr<displace::commons::Scenario> scenario;

    GeoGraph geoGraph;
    std::vector<Node *> nodes;

    std::vector<int> graph_point_code_landscape;
    std::vector<int> graph_point_code_landscape_unique;

    // cached

    bool is_tacs;
    bool is_fishing_credits;
    bool is_discard_ban;
    bool is_grouped_tacs;
    bool is_benthos_in_numbers;
    bool is_benthos_in_longevity_classes;
    bool is_direct_killing_on_benthos;
    bool is_resuspension_effect_on_benthos;
};

SimModel::SimModel()
        : p(spimpl::make_unique_impl<Impl>())
{

}

void SimModel::setConfig(std::unique_ptr<displace::commons::Config> config)
{
    p->config = std::move(config);
}

displace::commons::Config const &SimModel::config() const
{
    return *p->config;
}

void SimModel::setScenario(std::unique_ptr<displace::commons::Scenario> scenario)
{
    p->scenario = std::move(scenario);

    if (p->scenario->dyn_alloc_sce.option(Options::TACs)) {
        p->is_tacs = 1;
    } else {
        p->is_tacs = 0;
    }

    if (p->scenario->dyn_alloc_sce.option(Options::fishing_credits)) {
        p->is_fishing_credits = 1;
    } else {
        p->is_fishing_credits = 0;
    }

    if (p->scenario->dyn_alloc_sce.option(Options::discard_ban)) {
        p->is_discard_ban = 1;
    } else {
        p->is_discard_ban = 0;
    }

    if (p->scenario->dyn_alloc_sce.option(Options::groupedTACs)) {
        p->is_grouped_tacs = 1;
    } else {
        p->is_grouped_tacs = 0;
    }

    if (p->scenario->dyn_pop_sce.option(Options::modelBenthosInN)) {
        p->is_benthos_in_numbers = 1;
    } else {
        p->is_benthos_in_numbers = 0; // if not N then it impacts the benthos biomass by default
    }

    if (p->scenario->dyn_pop_sce.option(Options::modelBenthosInLongevity)) {
        p->is_benthos_in_longevity_classes = 1;
    } else {
        p->is_benthos_in_longevity_classes = 0;
    }
}

displace::commons::Scenario const &SimModel::scenario() const
{
    return *p->scenario;
}

void SimModel::setNodes(std::vector<Node *> nodes)
{
    p->nodes = std::move(nodes);
}

std::vector<Node *> const &SimModel::nodes() const
{
    return p->nodes;
}

std::vector<Node *> &SimModel::nodes()
{
    return p->nodes;
}

void SimModel::setQuarter(int quarter)
{
    p->quarter = quarter;
}

int SimModel::quarter() const
{
    return p->quarter;
}

void SimModel::setMonth(int month)
{
    p->month = month;
}

int SimModel::month() const
{
    return p->month;
}

void SimModel::setSemester(int semester)
{
    p->semester = semester;
}

int SimModel::semester() const
{
    return p->semester;
}

void SimModel::set_graph_point_code_landscape(std::vector<int> v)
{
    p->graph_point_code_landscape_unique = v;
    p->graph_point_code_landscape = std::move(v);

    sort(p->graph_point_code_landscape_unique.begin(), p->graph_point_code_landscape_unique.end());
    auto it = std::unique(p->graph_point_code_landscape_unique.begin(), p->graph_point_code_landscape_unique.end());
    p->graph_point_code_landscape_unique.resize(std::distance(p->graph_point_code_landscape_unique.begin(), it));
}

std::vector<int> const &SimModel::graph_point_code_landscape() const
{
    return p->graph_point_code_landscape;
}
std::vector<int> const& SimModel::graph_point_code_landscape_unique() const
{
    return p->graph_point_code_landscape_unique;
}

std::vector<int> &SimModel::graph_point_code_landscape()
{
    return p->graph_point_code_landscape;
}

void SimModel::setGeoGraph(GeoGraph geoGraph)
{
    p->geoGraph = std::move(geoGraph);
}

GeoGraph const &SimModel::geoGraph() const
{
    return p->geoGraph;
}

bool SimModel::is_tacs() const
{
    return p->is_tacs;
}

bool SimModel::is_fishing_credits() const
{
    return p->is_fishing_credits;
}

bool SimModel::is_discard_ban() const
{
    return p->is_discard_ban;
}

bool SimModel::is_grouped_tacs() const
{
    return p->is_grouped_tacs;
}

bool SimModel::is_benthos_in_numbers() const
{
    return p->is_benthos_in_numbers;
}

bool SimModel::is_benthos_in_longevity_classes() const
{
    return p->is_benthos_in_longevity_classes;
}

bool SimModel::is_direct_killing_on_benthos() const
{
    return p->is_direct_killing_on_benthos;
}

bool SimModel::is_resuspension_effect_on_benthos() const
{
    return p->is_resuspension_effect_on_benthos;
}

void SimModel::initRandom(std::string const &namesimu)
{

    size_t iii = 0;
    // detect a integer in the name of the simu
    for (; iii < namesimu.length(); iii++) { if (isdigit(namesimu[iii])) { break; }}

    // remove the first chars, which aren't digits
    std::string str = namesimu.substr(iii, namesimu.length() - iii);

    // convert the remaining text to an integer
    int a_seed = atoi(str.c_str());

    // print the result
    std::cout << "The random seed is set to: " << a_seed << std::endl;

    // set always the same seed for a given simu name (that should include a integer with pattern s100 or sim100 or simu100...)
    srand(a_seed);
}
