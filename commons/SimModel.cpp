//
// Created by happycactus on 1/10/20.
//

#include "SimModel.h"

#include "comstructs.h"

struct SimModel::Impl {
    std::unique_ptr<displace::commons::Config> config;
    std::unique_ptr<displace::commons::Scenario> scenario;
};

SimModel::SimModel()
        : p(spimpl::make_unique_impl<Impl>())
{

}

void SimModel::setConfig(std::unique_ptr<displace::commons::Config> config)
{
    p->config = std::move(config);
}

displace::commons::Config const &SimModel::config() const
{
    return *p->config;
}

void SimModel::setScenario(std::unique_ptr<displace::commons::Scenario> scenario)
{
    p->scenario = std::move(scenario);
}

displace::commons::Scenario const &SimModel::scenario() const
{
    return *p->scenario;
}
