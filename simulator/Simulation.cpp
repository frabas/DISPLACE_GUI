//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#include "Simulation.h"
#include <io/Loader.h>

using namespace displace;

Simulation::Simulation()
{

}

bool Simulation::initialize(const std::list<std::string> &options)
{
    auto project_file = options.back();

    auto loader = io::Loader::create(project_file);
    if (!loader)
        return false;

    if (!loader->load(this))
        return false;

    return true;
}

int Simulation::run()
{
    return 0;
}

