//
// Created by Federico Fuga on 03/03/16.
//

#include "LegacyLoader.h"

#include <fstream>
#include <iostream>

using namespace displace::io;

LegacyLoader::LegacyLoader(const std::string &path)
{
    boost::filesystem::path p(path);
    auto dir = p.remove_filename();
    mPath = dir.parent_path();
}

bool LegacyLoader::load(displace::Simulation *simulation)
{
    std::cout << "Loading from: " << mPath << std::endl;

    return true;
}
