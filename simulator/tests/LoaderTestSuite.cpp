//
// Created by happycactus on 07/08/19.
//

#include "LoaderTestSuite.h"

#include "db/ConfigTable.h"
#include "db/ScenarioConfigTable.h"
#include "NodesLoader.h"
#include "Node.h"

#include <boost/log/trivial.hpp>

struct LoaderTestSuite::Impl {
    msqlitecpp::v2::Storage &db;

    displace::in::ConfigTable config;
    displace::in::ScenarioConfigTable scenario;

    int graphsce;
    int biosce;

    std::vector<Node *> nodes;

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

    BOOST_LOG_TRIVIAL(info) << "Graph Scenario: " << p->graphsce;
}

void LoaderTestSuite::loadNodes()
{
    BOOST_LOG_TRIVIAL(info) << "Loading nodes for graphsce " << p->graphsce;

    NodesLoader loader(p->db);
    p->nodes = loader.load(p->graphsce);

    BOOST_LOG_TRIVIAL(info) << "Nodes loaded: " << p->nodes.size() << " nodes";
}
