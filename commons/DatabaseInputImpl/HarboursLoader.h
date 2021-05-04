//
// Created by happycactus on 29/04/21.
//

#ifndef DISPLACE_HARBOURSLOADER_H
#define DISPLACE_HARBOURSLOADER_H

#include "myutils.h"
#include "utils/spimpl.h"

namespace msqlitecpp { namespace v2 { class Storage; }}

class HarboursLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;
public:
    explicit HarboursLoader(msqlitecpp::v2::Storage &db);


};


#endif //DISPLACE_HARBOURSLOADER_H
