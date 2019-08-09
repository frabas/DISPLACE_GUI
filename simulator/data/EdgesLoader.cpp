//
// Created by happycactus on 09/08/19.
//

#include "EdgesLoader.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

namespace db = msqlitecpp::v2;

struct EdgesLoader::Impl {
    msqlitecpp::v2::Storage &db;

    Impl(msqlitecpp::v2::Storage &thedb) : db(thedb)
    {}


};

namespace {
static const char *EdgesTableName = "Edges";

db::Column<db::ColumnTypes::Integer> fieldFromNode{"from_node_id"};
db::Column<db::ColumnTypes::Integer> fieldToNode{"to_node_id"};
db::Column<db::ColumnTypes::Real> fieldWeight{"w"};
db::Column<db::ColumnTypes::Integer> fieldGraphSce{"graphsce"};

}

EdgesLoader::EdgesLoader(msqlitecpp::v2::Storage &thedb)
        : p(spimpl::make_unique_impl<Impl>(thedb))
{

}

adjacency_map_t EdgesLoader::load(int agraph)
{
    adjacency_map_t amap;

    auto query = db::makeSelectStatement(p->db, EdgesTableName,
                                         fieldFromNode, fieldToNode, fieldWeight
    );
    query.where(fieldGraphSce == "agraph");

    query.bind(agraph);
    query.execute([&amap](int from, int to, double w) {
        amap[from].push_back(edge(to, w));
        return true;
    });

    return amap;
}
