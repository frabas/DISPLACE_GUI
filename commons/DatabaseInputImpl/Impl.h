//
// Created by fuga on 01/06/2020.
//

#ifndef DISPLACE_IMPL_H
#define DISPLACE_IMPL_H

#include "DatabaseModelLoader.h"

#include <msqlitecpp/v2/storage.h>

namespace sql = msqlitecpp::v2;

struct DatabaseModelLoader::Impl {
    std::shared_ptr<sql::Storage> db;

    Impl(std::shared_ptr<sql::Storage> d)
            : db(d)
    {}
};


#endif //DISPLACE_IMPL_H
