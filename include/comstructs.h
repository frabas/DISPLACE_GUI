#ifndef COMSTRUCTS_H
#define COMSTRUCTS_H

#include <options.h>

namespace displace {
namespace commons {

struct Scenario {
    DynAllocOptions dyn_alloc_sce;
    PopSceOptions dyn_pop_sce;
    std::string biolsce;
    int a_graph;
    int nrow_coord;
    int nrow_graph;
    int a_port;
    double graph_res;
    double is_individual_vessel_quotas;

    std::string dt_go_fishing;
    std::string dt_choose_ground;
    std::string dt_start_fishing;
    std::string dt_change_ground;
    std::string dt_stop_fishing;
    std::string dt_change_port;

};

}
}

#endif // COMSTRUCTS_H

