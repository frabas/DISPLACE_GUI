//
// Created by happycactus on 1/10/20.
//

#ifndef DISPLACE_SIMMODEL_H
#define DISPLACE_SIMMODEL_H

#include "utils/spimpl.h"

class SimModel {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    SimModel();
};


#endif //DISPLACE_SIMMODEL_H
