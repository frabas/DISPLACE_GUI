//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#include "Loader.h"

using namespace displace::io;

std::unique_ptr<Loader> Loader::create(const std::string &path)
{
    return std::unique_ptr<Loader>();
}
