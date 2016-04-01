//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#include "Simulation.h"
#include <io/Loader.h>
#include <utils/make_unique.h>

#include <Environment.h>
#include <iostream>

#include <ipc/messages/StepMessage.h>
#include <boost/make_shared.hpp>
#include <strategies/modelstrategy.h>
#include <strategies/models/SimpleIncrementalModel.h>

using namespace displace;

struct Simulation::Impl {
    std::unique_ptr<strategy::model::ModelStrategy> mModelStrategy;
};

Simulation::Simulation()
: mImpl(utils::make_unique<Impl>()),
  mEnv(utils::make_unique<Environment>())
{
    mImpl->mModelStrategy = utils::make_unique<strategy::model::SimpleIncrementalModel>();
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

    mOutQueueManager.start();

    return true;
}

bool Simulation::deinitialize()
{
    mOutQueueManager.finish();

    return true;
}


int Simulation::run()
{
    unsigned long numsteps = 10000;

    auto tstep = mEnv->clock().timestep();
    while (tstep < numsteps) {
        // do something...
        mOutQueueManager.enqueue(boost::make_shared<displace::ipc::StepMessage>(tstep));

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
    mImpl->mModelStrategy->updatePopulationModel(this);
}

void Simulation::applyManagementModel()
{

}

void Simulation::applyVesselsModel()
{

}





