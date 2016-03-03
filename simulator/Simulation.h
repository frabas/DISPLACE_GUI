//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_SIMULATION_H
#define DISPLACE_SIMULATION_H

#include <list>
#include <string>

namespace displace {

class Simulation {
public:
    explicit Simulation();

    bool initialize(const std::list<std::string> &options);
    int run();
};

} // ns;

#endif //DISPLACE_SIMULATION_H
