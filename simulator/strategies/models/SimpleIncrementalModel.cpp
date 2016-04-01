//
// Created by Federico Fuga on 01/04/16.
//

#include "SimpleIncrementalModel.h"

#include <Simulation.h>
#include <env/Playground.h>
#include <env/Node.h>
#include <env/aspects/populationsaspect.h>

using namespace displace;
using namespace displace::strategy::model;

bool SimpleIncrementalModel::updatePopulationModel(Simulation *simulation)
{
    auto &env = simulation->environment();
    if (!env.calendar().isMonth(env.clock().timestep())) // only update at start of month
        return true;

    auto &pg = simulation->environment().playground();

    for (int i = 0; i < pg.getNodeCount(); ++i) {
        auto node = pg.node(i);

        auto &pop = node->populationsAspect();

        for (auto &p : pop.population) {
            for (auto &sz : p) {
                sz *= mFactor;
            }
        }
    }

    return true;
}

