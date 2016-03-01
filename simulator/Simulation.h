//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_SIMULATION_H
#define DISPLACE_SIMULATION_H

#include <list>
#include <string>

class Simulation {
public:
    explicit Simulation(const std::list<std::string> &options);

    int run();
};


#endif //DISPLACE_SIMULATION_H
