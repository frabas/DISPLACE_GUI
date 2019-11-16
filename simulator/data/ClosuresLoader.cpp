//
// Created by happycactus on 11/16/19.
//

#include "ClosuresLoader.h"

#include "readdata.h"

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/selectstatement.h"
#include "msqlitecpp/v2/fields.h"

namespace db = msqlitecpp::v2;

namespace {
static const char *ClosuresTableName = "ClosuresSpe";

db::Column<db::ColumnTypes::Integer> fieldClosuresSce{"ClosureSce"};
db::Column<db::ColumnTypes::Integer> fieldClosuresId{"ClosureId"};
db::Column<db::ColumnTypes::Integer> fieldNodeId{"NodeId"};
db::Column<db::ColumnTypes::Integer> fieldPeriod{"Period"};
db::Column<db::ColumnTypes::Integer> fieldOpt{"Opt"};
db::Column<db::ColumnTypes::Text> fieldType{"Type"};
db::Column<db::ColumnTypes::Text> fieldClosures{"Closures"};

}

struct ClosuresLoader::Impl {
    msqlitecpp::v2::Storage &db;

    db::SelectStatement<
            decltype(fieldClosuresId), decltype(fieldNodeId),
            decltype(fieldOpt), decltype(fieldClosures)
    > selectClosures;


    Impl(msqlitecpp::v2::Storage &d);

    bool read_metier_closures(int graph_spe, int period, std::string keyword,
                              vector<NodeBanningInfo> &ban_info);
};

ClosuresLoader::ClosuresLoader(msqlitecpp::v2::Storage &db)
        : p(spimpl::make_unique_impl<Impl>(db))
{

}

bool ClosuresLoader::read_metier_closures(int graph_spe, int period, vector<NodeBanningInfo> &ban_info)
{
    return p->read_metier_closures(graph_spe, period, "metier", ban_info);
}

bool ClosuresLoader::read_vsize_closures(int graph_spe, int period, vector<NodeBanningInfo> &ban_info)
{
    return p->read_metier_closures(graph_spe, period, "vsize", ban_info);
}

ClosuresLoader::Impl::Impl(msqlitecpp::v2::Storage &d)
        : db(d),
          selectClosures(db, ClosuresTableName, fieldClosuresId,fieldNodeId,
          fieldOpt, fieldClosures)
{
    selectClosures.where(fieldClosuresSce == "sce" && fieldType == "type" && fieldPeriod == "period");
}

bool ClosuresLoader::Impl::read_metier_closures(int graph_spe, int period, std::string keyword,
                                                vector<NodeBanningInfo> &ban_info)
{
    selectClosures.bind(graph_spe, keyword, period);
    selectClosures.execute([&ban_info](int id, int node, int opt, std::string values) {
        NodeBanningInfo info;
        std::vector<std::string> sepvalues;
        boost::split(sepvalues, values, boost::is_any_of(" "));
        for (auto &value : sepvalues) {
                        info.banned.emplace_back(std::stoi(value));
                        info.nbOfDaysClosedPerMonth.push_back(opt);
        }
        info.nodeId = types::NodeId{static_cast<uint16_t>(node)};
        ban_info.push_back(info);
        return true;
    });

    return true;
}
