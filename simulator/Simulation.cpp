//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#include "Simulation.h"
#include <io/Loader.h>
#include <utils/make_unique.h>

#include <Environment.h>
#include <iostream>

using namespace displace;

struct Simulation::Impl {
};

Simulation::Simulation()
: mImpl(utils::make_unique<Impl>()),
  mEnv(utils::make_unique<Environment>())
{
}

Simulation::~Simulation() noexcept = default;


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
    unsigned long numsteps = 10000;

    auto tstep = mEnv->clock().timestep();
    while (tstep < numsteps) {
        // do something...

        applyPopulationModel();

        applyManagementModel();

        applyVesselsModel();

        // export data

        // update clock and calendar

        mEnv->clock().advance();

        // last before restart
        tstep = mEnv->clock().timestep();
    }

    return 0;
}

void Simulation::applyPopulationModel()
{

}

void Simulation::applyManagementModel()
{

}

void Simulation::applyVesselsModel()
{

}







