//
// Created by happycactus on 06/08/19.
//

#include "NodesLoader.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

#include "Node.h"

namespace db = msqlitecpp::v2;

struct NodesLoader::Impl {
    msqlitecpp::v2::Storage &db;

    Impl(msqlitecpp::v2::Storage &theDb) : db(theDb)
    {}
};

namespace {
static const char *NodesTableName = "Nodes";

db::Column<db::ColumnTypes::Integer> fieldId{"id"};
db::Column<db::ColumnTypes::Real> fieldX{"x"};
db::Column<db::ColumnTypes::Real> fieldY{"y"};
db::Column<db::ColumnTypes::Integer> fieldHIdx{"hidx"};
db::Column<db::ColumnTypes::Integer> fieldGraphSce{"graphsce"};
}

NodesLoader::NodesLoader(std::string inputFolder, std::string folder_name_parameterization, int nrow)
{
    throw std::runtime_error("NodesLoader for text files is not implemented");
}

NodesLoader::NodesLoader(msqlitecpp::v2::Storage &db)
        : p(spimpl::make_unique_impl<Impl>(db))
{

}

std::vector<Node *> NodesLoader::load(int agraph)
{
    std::vector<Node *> nodes;

    auto query = db::makeSelectStatement(p->db, NodesTableName, fieldId, fieldX, fieldY, fieldHIdx);
    query.where(fieldGraphSce == "agraph");

    query.bind(agraph);
    query.execute([&nodes](int id, double x, double y, int hidx) {
        auto node = new Node();
        node->set_idx_node(types::NodeId{static_cast<uint16_t>(id)});
        node->set_xy(x, y);
        node->set_is_harbour(hidx);

        nodes.push_back(node);
        return true;
    });

    return nodes;
}
