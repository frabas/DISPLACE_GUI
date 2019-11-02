//
// Created by happycactus on 07/08/19.
//

#include "LoaderTestSuite.h"

#include "db/ConfigTable.h"
#include "db/ScenarioConfigTable.h"
#include "NodesLoader.h"
#include "EdgesLoader.h"
#include "VesselsLoader.h"
#include "Node.h"

#include <boost/log/trivial.hpp>

struct LoaderTestSuite::Impl {
    msqlitecpp::v2::Storage &db;

    displace::in::ConfigTable config;
    displace::in::ScenarioConfigTable scenario;

    int graphsce;
    int biosce;
    int fleetsce;

    std::vector<Node *> nodes;
    adjacency_map_t admap;

    explicit Impl(msqlitecpp::v2::Storage &db_)
            : db(db_)
    {}
};

LoaderTestSuite::LoaderTestSuite(msqlitecpp::v2::Storage &db)
        : p(spimpl::make_unique_impl<Impl>(db))
{

}

void LoaderTestSuite::prepare()
{
    BOOST_LOG_TRIVIAL(info) << "Loading configuration";

    p->config.query(p->db);

    BOOST_LOG_TRIVIAL(info) << "Loading scenario data";
    p->scenario.query(p->db);

    p->graphsce = p->scenario.getAs<int>("a_graph");
    p->biosce = p->scenario.getAs<int>("biolsce");
    p->fleetsce = p->scenario.getAs<int>("fleetsce");

    BOOST_LOG_TRIVIAL(info) << "Graph Scenario: " << p->graphsce;
}

void LoaderTestSuite::loadNodes()
{
    BOOST_LOG_TRIVIAL(info) << "Loading nodes for graphsce " << p->graphsce;

    NodesLoader loader(p->db);
    p->nodes = loader.load(p->graphsce);

    BOOST_LOG_TRIVIAL(info) << "Nodes loaded: " << p->nodes.size() << " nodes";
}

void LoaderTestSuite::loadEdges()
{
    BOOST_LOG_TRIVIAL(info) << "Loading edges for graphsce " << p->graphsce;

    EdgesLoader loader(p->db);
    p->admap = loader.load(p->graphsce);

    size_t nnum = p->admap.size();
    size_t ednum = 0;
    for (auto &nn : p->admap) {
        ednum += nn.second.size();
    }

    BOOST_LOG_TRIVIAL(info) << "Edges: loaded " << nnum << " nodes adjancencies, total " << ednum << " edges";
}

void LoaderTestSuite::loadVessels()
{
    BOOST_LOG_TRIVIAL(info) << "Loading edges for fleetsce " << p->fleetsce;

    VesselsLoader loader(p->db);
    auto allvessels = loader.getListOfAllVessels();

    BOOST_LOG_TRIVIAL(info) << "Vessels: loaded " << allvessels.size() << " Vessels name";

    auto fuelprices = loader.getInitFuelPrices();

    BOOST_LOG_TRIVIAL(info) << "Vessels: loaded " << fuelprices.size() << " Initial fuel prices";

	std::vector<std::shared_ptr<VesselsLoader::VesselData>> v;
    size_t count = 0, ecount = 0;
    for (auto &vessel : allvessels) {
        for (int quarter = 1; quarter <= 4; ++quarter) {
            auto data = loader.getVesselData(vessel, quarter);
            ++count;
			v.push_back(data);
        }

        auto ecfeat = loader.getEconomicFeature(vessel);
        ecount += ecfeat.size();
    }

    BOOST_LOG_TRIVIAL(info) << "Vessels: loaded " << ecount << " Economic Features and " << count << " Records";
}
