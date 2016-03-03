//
// Created by Federico Fuga on 03/03/16.
//

#include "LegacyLoader.h"

#include <fstream>
#include <iostream>
#include <formats/utils/LineNumberReader.h>

#include <boost/regex.hpp>

using namespace displace::io;

LegacyLoader::LegacyLoader(const std::string &path)
{
    boost::filesystem::path p(path);
    auto dir = p.remove_filename();

    auto ldir = dir.filename();
    boost::regex r { ".*_([^_]+)$" };
    boost::smatch results;

    if (!boost::regex_match(ldir.string(), results, r)) {
        // throw an exception. The path is malformed
        throw std::runtime_error("Malformed file name");
    }

    mPath = dir.parent_path();
    mModel = results[1];
}

bool LegacyLoader::load(displace::Simulation *simulation)
{
    std::cout << "Loading from: " << mPath << " model is " << mModel << std::endl;

    if (!loadConfigFile())
        return false;

    return true;
}

bool LegacyLoader::loadConfigFile()
{
    std::string filepath = mPath.append(std::string {"simusspe_"} + mModel).append("config.dat").string();

    displace::formats::helpers::LineNumberReader reader;
    static const displace::formats::helpers::LineNumberReader::Specifications specs {
            {1,"nbpops"},{3,"nbbenthospops"},{5,"implicit_pops"},{7,"calib_oth_landings"},
            {9,"calib_weight_at_szgroup"},{11,"calib_cpue_multiplier"},{13,"int_harbours"}
    };

    std::cout << "Reading config file from " << filepath << std::endl;

    if (!reader.importFromFile(filepath, specs))
        return false;

    // dump
    std::cout << reader << std::endl;

    return true;
}
