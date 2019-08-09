//
// Created by happycactus on 09/08/19.
//

#ifndef DISPLACE_EDGESLOADER_H
#define DISPLACE_EDGESLOADER_H

#include "myutils.h"
#include "utils/spimpl.h"

namespace msqlitecpp { namespace v2 { class Storage; }}

class EdgesLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    explicit EdgesLoader(msqlitecpp::v2::Storage &db);

    adjacency_map_t load(int agraph);
};


#endif //DISPLACE_EDGESLOADER_H
