//
// Created by happycactus on 1/10/20.
//

#ifndef DISPLACE_SIMMODEL_H
#define DISPLACE_SIMMODEL_H

#include "utils/spimpl.h"

#include "diffusion.h"

#include <vector>

namespace displace {
namespace commons {
struct Config;
struct Scenario;
}
}
class Node;

class Fishfarm;

class Windmill;

class GeoGraph;

class SimModel {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    SimModel();

    void initRandom(std::string const &namesimu);

    void setConfig(std::unique_ptr<displace::commons::Config> config);

    displace::commons::Config const &config() const;

    void setScenario(std::unique_ptr<displace::commons::Scenario> config);

    displace::commons::Scenario const &scenario() const;

    void setNodes(std::vector<Node *> nodes);

    std::vector<Node *> const &nodes() const;

    std::vector<Node *> &nodes();

    void set_graph_point_code_landscape(std::vector<int> v);

    std::vector<int> const &graph_point_code_landscape() const;

    std::vector<int> const &graph_point_code_landscape_unique() const;

    std::vector<int> &graph_point_code_landscape();

    void setWindmills(vector<Windmill *> windmills);

    vector<Windmill *> &windmills();

    vector<Windmill *> const &windmills() const;

    void setFishFarms(vector<Fishfarm *> fishfarms);

    std::vector<Fishfarm *> &fishfarms();

    std::vector<Fishfarm *> const &fishfarms() const;

    DiffusionTree &diffusionTree();

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

    bool is_tacs() const;

    bool is_fishing_credits() const;

    bool is_discard_ban() const;

    bool is_grouped_tacs() const;

    bool is_benthos_in_numbers() const;

    bool is_benthos_in_longevity_classes() const;

    bool is_direct_killing_on_benthos() const;

    bool is_resuspension_effect_on_benthos() const;

};


#endif //DISPLACE_SIMMODEL_H
