//
// Created by happycactus on 06/08/19.
//

#ifndef DISPLACE_NODETESTER_H
#define DISPLACE_NODETESTER_H

#include "utils/spimpl.h"

class NodeTester {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;

public:
    NodeTester(int nbpops, int nbbenthospops);

    void test();
};


#endif //DISPLACE_NODETESTER_H
