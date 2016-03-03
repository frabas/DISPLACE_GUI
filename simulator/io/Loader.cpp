//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#include "Loader.h"

#include <io/legacy/LegacyLoader.h>
#include <utils/make_unique.h>

using namespace displace::io;

std::unique_ptr<Loader> Loader::create(const std::string &path)
{
    return utils::make_unique<LegacyLoader>(path);
}
