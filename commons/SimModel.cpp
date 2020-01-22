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
