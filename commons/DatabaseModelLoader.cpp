//
// Created by happycactus on 5/23/20.
//

#include "DatabaseModelLoader.h"

#include "db/ConfigTable.h"
#include "db/ScenarioConfigTable.h"
#include "readdata.h"

#include <msqlitecpp/v2/storage.h>

namespace sql = msqlitecpp::v2;

struct DatabaseModelLoader::Impl {
    std::shared_ptr<sql::Storage> db;

    Impl(std::shared_ptr<sql::Storage> d)
            : db(d)
    {}
};

DatabaseModelLoader::DatabaseModelLoader(std::shared_ptr<SimModel> model, std::shared_ptr<sql::Storage> db)
        : ModelLoader(model),
          p(spimpl::make_unique_impl<Impl>(db))
{

}

void DatabaseModelLoader::loadCalendar()
{

}

void DatabaseModelLoader::loadShips()
{

}

void DatabaseModelLoader::loadVessels(int year, int month, int quarter, int semester)
{

}

void DatabaseModelLoader::loadMetiers()
{

}

bool DatabaseModelLoader::loadPopulations(int year)
{
    return false;
}

bool DatabaseModelLoader::loadConfigImpl(int &nbpops, int &nbbenthospops, std::vector<int> &implicit_pops,
                                         std::vector<int> &implicit_pops_level2, std::vector<int> &grouped_tacs,
                                         std::vector<int> &nbcp_coupling_pops, std::vector<double> &calib_oth_landings,
                                         std::vector<double> &calib_w, std::vector<double> &calib_cpue,
                                         std::vector<types::NodeId> &interesting_harbours)
{
    displace::in::ConfigTable config;

    config.query(*p->db);

    nbpops = config.getNbPops();
    nbbenthospops = config.getNbBenthosPops();
    implicit_pops = config.getImplicitStocks();
    implicit_pops_level2 = config.getImplicitPopLevels2();
    grouped_tacs = config.getGroupedTacs();
    nbcp_coupling_pops = config.getNbCouplingPops();
    calib_oth_landings = config.getCalibLandingsStock();
    calib_w = config.getCalibW();
    calib_cpue = config.getCalibCpue();
    interesting_harbours = config.getInterestingArbours();

    return true;
}

bool DatabaseModelLoader::loadScenarioImpl(displace::commons::Scenario &s)
{
    return read_scenario_config_file(p->db,
                                     "", "", "",
                                     s);
}

bool DatabaseModelLoader::loadNodesAndGraphsDataImpl()
{
    return false;
}

bool DatabaseModelLoader::loadFishFarmsImpl()
{
    return false;
}

bool DatabaseModelLoader::loadWindmillsImpl()
{
    return false;
}

ModelLoader::BenthosData
DatabaseModelLoader::loadBenthosData(const PopSceOptions &dyn_pop_sce, const DynAllocOptions &dyn_alloc_sce,
                                     const std::string &biolsce, const std::string &fleetsce)
{
    return BenthosData();
}
