//
// Created by <fuga@studiofuga.com> on 29/02/16.
//

#include <Simulation.h>

int main(int argc, char *argv[])
{
    std::list<std::string> options;
    for (int i = 0; i < argc; ++i)
        options.emplace_back(std::string(argv[i]));

    Simulation sim(options);

    return sim.run();
}

