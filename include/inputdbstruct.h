//
// Created by happycactus on 05/08/19.
//

#ifndef DISPLACE_INPUTDBSTRUCT_H
#define DISPLACE_INPUTDBSTRUCT_H

#include "msqlitecpp/v2/storage.h"
#include "msqlitecpp/v2/fields.h"

namespace db = msqlitecpp::v2;

namespace displace {
namespace in {

struct ConfigTable {
    static constexpr char * const TableName = "Config";

    db::Column<db::ColumnTypes::Integer> NbPops{"nbpops"};
    db::Column<db::ColumnTypes::Integer> NbBenthosPops{"nbbenthospops"};
    db::Column<db::ColumnTypes::Text> ImplicitStocks{"implicit_stocks"};
    db::Column<db::ColumnTypes::Text> CalibLandings{"calib_landings_stock"};
    db::Column<db::ColumnTypes::Text> CalibW{"calib_w_at_szgroup_stock"};
    db::Column<db::ColumnTypes::Text> CalibCpue{"calib_cpue_stock"};
    db::Column<db::ColumnTypes::Text> IntHarbours{"Interesting_harbours"};
    db::Column<db::ColumnTypes::Text> ImplicitLevel2{"implicit_pops_level2"};
    db::Column<db::ColumnTypes::Text> GroupedTacs{"grouped_tacs"};
    db::Column<db::ColumnTypes::Text> NbCouplingPops{"nbcp_coupling_pops"};
};

}
}

#endif //DISPLACE_INPUTDBSTRUCT_H
