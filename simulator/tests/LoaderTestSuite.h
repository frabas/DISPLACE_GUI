//
// Created by happycactus on 07/08/19.
//

#ifndef DISPLACE_LOADERTESTSUITE_H
#define DISPLACE_LOADERTESTSUITE_H

#include "utils/spimpl.h"

namespace msqlitecpp { namespace v2 { class Storage; }}

class LoaderTestSuite {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    LoaderTestSuite(msqlitecpp::v2::Storage &db);

    void prepare();

    void loadNodes();

    void loadEdges();

    void loadVessels();

    void loadMetierClosures();
    void loadVSizeClosures();
};


#endif //DISPLACE_LOADERTESTSUITE_H
