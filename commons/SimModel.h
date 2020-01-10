//
// Created by happycactus on 1/10/20.
//

#ifndef DISPLACE_SIMMODEL_H
#define DISPLACE_SIMMODEL_H

#include "utils/spimpl.h"

namespace displace {
namespace commons {
struct Config;
struct Scenario;
}
}

class SimModel {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    SimModel();

    void setConfig(std::unique_ptr<displace::commons::Config> config);

    displace::commons::Config const &config() const;

    void setScenario(std::unique_ptr<displace::commons::Scenario> config);

    displace::commons::Scenario const &scenario() const;
};


#endif //DISPLACE_SIMMODEL_H
