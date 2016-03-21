//
// Created by <fuga@studiofuga.com> on 29/02/16.
//

#include <Simulation.h>
#include <iostream>

int main(int argc, char *argv[])
{
    std::list<std::string> options;
    for (int i = 0; i < argc; ++i)
        options.emplace_back(std::string(argv[i]));

    displace::Simulation sim;
    if (!sim.initialize(options)) {
        std::cerr << "Cannot initialize simulation. Exiting." << std::endl;
        return 1;
    }

    auto ret = sim.run();

    sim.deinitialize();

    return ret;
}

