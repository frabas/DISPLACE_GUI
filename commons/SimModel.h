//
// Created by happycactus on 1/10/20.
//

#ifndef DISPLACE_SIMMODEL_H
#define DISPLACE_SIMMODEL_H

#include "utils/spimpl.h"

#include <vector>

namespace displace {
namespace commons {
struct Config;
struct Scenario;
}
}
class Node;

class GeoGraph;

class SimModel {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    SimModel();

    void setConfig(std::unique_ptr<displace::commons::Config> config);

    displace::commons::Config const &config() const;

    void setScenario(std::unique_ptr<displace::commons::Scenario> config);

    displace::commons::Scenario const &scenario() const;

    void setNodes(std::vector<Node *> nodes);

    std::vector<Node *> const &nodes() const;

    std::vector<Node *> &nodes();

    void set_graph_point_code_landscape(std::vector<int> v);

    std::vector<int> const &graph_point_code_landscape() const;
    std::vector<int> const& graph_point_code_landscape_unique() const;

    std::vector<int> &graph_point_code_landscape();

    //
    void setGeoGraph(GeoGraph geoGraph);

    GeoGraph const &geoGraph() const;

    /// calendar

    void setQuarter(int quarter);

    int quarter() const;

    void setMonth(int month);

    int month() const;

    void setSemester(int semester);

    int semester() const;
};


#endif //DISPLACE_SIMMODEL_H
