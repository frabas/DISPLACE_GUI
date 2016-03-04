//
// Created by Federico Fuga on 03/03/16.
//

#include "LegacyLoader.h"

#include <fstream>
#include <iostream>
#include <formats/utils/LineNumberReader.h>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <utils/make_unique.h>

using namespace displace::io;

namespace helpers = displace::formats::helpers;

struct LegacyLoader::Status
{
    int a_graph;
};

LegacyLoader::LegacyLoader(const std::string &path)
: mStatus(utils::make_unique<LegacyLoader::Status>())
{
    boost::filesystem::path p(path);
    auto dir = p;
    dir.remove_filename();

    auto ldir = dir.filename();
    boost::regex r { ".*_([^_]+)$" };
    boost::smatch results;

    if (!boost::regex_match(ldir.string(), results, r)) {
        // throw an exception. The path is malformed
        throw std::runtime_error("Malformed file name");
    }

    mPath = dir.parent_path();
    mModel = results[1];
    mScenario = p.filename().replace_extension({""}).string();
}

LegacyLoader::~LegacyLoader() noexcept = default;

bool LegacyLoader::load(displace::Simulation *simulation)
{
    std::cout << "Loading from: " << mPath << " model is " << mModel << " scenario " << mScenario << std::endl;

    if (!loadConfigFile())
        return false;
    if (!loadScenarioFile())
        return false;
    if (!loadGraph())
        return false;

    return true;
}

bool LegacyLoader::loadConfigFile()
{
    auto filepath = (mPath /
                     boost::filesystem::path{boost::str(boost::format{"simusspe_%s"} % mModel)} /
                     boost::filesystem::path{"config.dat"} ).string();


    helpers::LineNumberReader reader;
    static const helpers::LineNumberReader::Specifications specs {
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

bool LegacyLoader::loadScenarioFile()
{
    auto filepath = (mPath /
        boost::filesystem::path{boost::str(boost::format{"simusspe_%s"} % mModel)} /
        boost::filesystem::path{boost::str(boost::format{"%s.dat"} % mScenario)}
        ).string();

    helpers::LineNumberReader reader;

    static const helpers::LineNumberReader::Specifications specs {
            {1,"dyn_alloc_sce"},{3,"dyn_pop_sce"},{5,"biolsce"},{7,"freq_do_growth"},{9,"freq_redispatch_the_pop"},
            {11,"a_graph"},{13,"nrow_coord"},{15,"nrow_graph"},{17,"a_port"},{19,"graph_res"},
            {21,"is_individual_vessel_quotas"},{23,"check_all_stocks_before_going_fishing"},{25,"dt_go_fishing"},
            {27,"dt_choose_ground"},{29,"dt_start_fishing"},{31,"dt_change_ground"},{33,"dt_stop_fishing"},
            {35,"dt_change_port"},{37,"use_dtrees"},{39,"tariff_pop"},{41,"freq_update_tariff_code"},
            {43,"arbitary_breaks_for_tariff"},{45,"total_amount_credited"},{47,"tariff_annual_hcr_percent_change"}
    };

    std::cout << "Reading Scenario file from " << filepath << std::endl;

    if (!reader.importFromFile(filepath, specs))
        return false;

    // dump
    std::cout << reader << std::endl;

    mStatus->a_graph = boost::lexical_cast<int>(reader.get("a_graph"));

    return true;
}

bool LegacyLoader::loadGraph()
{
    auto path = mPath / boost::filesystem::path{"graphsspe"};
    auto graph = (path / boost::filesystem::path{ boost::str(boost::format{"coord%d.dat"} % mStatus->a_graph) }).string();
    auto codearea = (path / boost::filesystem::path{ boost::str(boost::format{"code_area_for_graph%d_points.dat"} % mStatus->a_graph) }).string();
    auto marineland = (path / boost::filesystem::path{ boost::str(boost::format{"coord%d_with_landscape.dat"} % mStatus->a_graph) }).string();

    std::cout << "Loading graph from " << graph << std::endl;
    std::cout << "Loading codearea from " << codearea << std::endl;
    std::cout << "Loading marineland from " << marineland << std::endl;

    return true;
}
