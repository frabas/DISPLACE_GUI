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
db::Column<db::ColumnTypes::Integer> fieldCodeArea{"code_area"};
db::Column<db::ColumnTypes::Integer> fieldMarineLandscape{"landscape"};
db::Column<db::ColumnTypes::Real> fieldWind{"wind"};
db::Column<db::ColumnTypes::Real> fieldSalinity{"salinity"};
db::Column<db::ColumnTypes::Real> fieldSst{"sst"};
db::Column<db::ColumnTypes::Real> fieldNitrogen{"nitrogen"};
db::Column<db::ColumnTypes::Real> fieldPhosphorus{"phosphorus"};
db::Column<db::ColumnTypes::Real> fieldOxygen{"oxygen"};
db::Column<db::ColumnTypes::Real> fieldCarbon{"carbon"};
db::Column<db::ColumnTypes::Real> fieldBathymetry{"bathymetry"};
db::Column<db::ColumnTypes::Real> fieldShipping{"shipping"};
db::Column<db::ColumnTypes::Real> fieldSilt{"silt"};
db::Column<db::ColumnTypes::Real> fieldBenthosBio{"benthosbio"};
db::Column<db::ColumnTypes::Real> fieldBenthosNum{"benthosnum"};
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

    auto query = db::makeSelectStatement(p->db, NodesTableName,
                                         fieldId, fieldX, fieldY, fieldHIdx,
                                         fieldCodeArea, fieldMarineLandscape,
                                         fieldWind, fieldSalinity, fieldSst,
                                         fieldNitrogen, fieldPhosphorus, fieldOxygen,
                                         fieldCarbon,
                                         fieldBathymetry,
                                         fieldShipping, fieldSilt, fieldBenthosBio, fieldBenthosNum
    );
    query.where(fieldGraphSce == "agraph");

    query.bind(agraph);
    query.execute([&nodes](int id, double x, double y, int hidx, int codeArea, int marineLandscape,
                           double wind, double salinity, double sst, double ni, double ph, double ox, double ca,
                           double bath, double ship, double silt, double bb, double bn
    ) {
        auto node = new Node();
        node->set_idx_node(types::NodeId{static_cast<uint16_t>(id)});
        node->set_xy(x, y);
        node->set_is_harbour(hidx);

        node->setMarineLandscape(marineLandscape);
        node->setCodeArea(codeArea);

        node->setWind(wind);
        node->setSalinity(salinity);
        node->setSST(sst);
        node->setNitrogen(ni);
        node->setPhosphorus(ph);
        node->setOxygen(ox);
        node->setDissolvedCarbon(ca);
        node->setBathymetry(bath);
        node->setShippingdensity(ship);
        node->setSiltfraction(silt);
        node->setBenthosBiomass(bb);
        node->setBenthosNumber(bn);

        nodes.push_back(node);
        return true;
    });

    return nodes;
}
