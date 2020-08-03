//
// Created by fuga on 01/06/2020.
//

#include "DatabaseModelLoader.h"
#include "Impl.h"
#include "Node.h"
#include "Harbour.h"
#include "SimModel.h"
#include "shortestpath/GeoGraph.h"

#include <msqlitecpp/v2/selectstatement.h>
#include <msqlitecpp/v2/fields.h>

namespace db = msqlitecpp::v2;

namespace {
static const char *NodesTableName = "Nodes";

db::Column<db::ColumnTypes::Integer> fieldId{"id"};
db::Column<db::ColumnTypes::Real> fieldX{"x"};
db::Column<db::ColumnTypes::Real> fieldY{"y"};
db::Column<db::ColumnTypes::Integer> fieldHIdx{"hidx"};
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
db::Column<db::ColumnTypes::Real> fieldIcesRectangleCode{ "icesrectanglecode" };
db::Column<db::ColumnTypes::Real> fieldBenthosBio{"benthosbio"};
db::Column<db::ColumnTypes::Real> fieldBenthosNum{"benthosnum"};

static const char *EdgesTableName = "Edges";

db::Column<db::ColumnTypes::Integer> fieldFromNode{"from_node_id"};
db::Column<db::ColumnTypes::Integer> fieldToNode{"to_node_id"};
db::Column<db::ColumnTypes::Real> fieldWeight{"w"};

db::Column<db::ColumnTypes::Integer> fieldGraphSce{"graphsce"};
}


bool DatabaseModelLoader::loadNodesAndGraphsDataImpl()
{
    std::vector<Node *> nodes;
    GeoGraph graph;

    auto query = db::makeSelectStatement(*p->db, NodesTableName,
                                         fieldId, fieldX, fieldY, fieldHIdx,
                                         fieldCodeArea, fieldMarineLandscape,
                                         fieldWind, fieldSalinity, fieldSst,
                                         fieldNitrogen, fieldPhosphorus, fieldOxygen,
                                         fieldCarbon,
                                         fieldBathymetry,
                                         fieldShipping, fieldSilt, fieldIcesRectangleCode, fieldBenthosBio, fieldBenthosNum
    );
    query.where(fieldGraphSce == "agraph");

    query.bind(model().scenario().a_graph);
    query.execute([&nodes, &graph](int id, double x, double y, int hidx, int codeArea, int marineLandscape,
                                   double wind, double salinity, double sst, double ni, double ph, double ox, double ca,
                                   double bath, double ship, double silt, double icesrectanglecode, double bb, double bn
    ) {
        Node *node;
        if (hidx != 0) {
            // is an harbour: sets harbours variables here
            auto harbour = new Harbour();

            harbour->set_is_harbour(hidx);

            node = harbour;
        } else {
            // is a standard Node

            node = new Node();
            node->set_is_harbour(0);

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
            node->setIcesrectanglecode(icesrectanglecode);
            node->setBenthosBiomass(bb);
            node->setBenthosNumber(bn);
        }

        node->set_idx_node(types::NodeId{static_cast<uint16_t>(id)});
        node->set_xy(x, y);

        nodes.push_back(node);

        graph.addNode(id, x, y);
        return true;
    });

    // edges
    auto query2 = db::makeSelectStatement(*p->db, NodesTableName,
                                          fieldFromNode, fieldToNode, fieldWeight);
    query2.where(fieldGraphSce == "agraph");
    query2.bind(model().scenario().a_graph);
    query2.execute([&graph](int f, int t, double w) {
        graph.addEdge(f, t, w);
        // Todo Check This:
        graph.addEdge(t, f, w);
        return true;
    });

    model().setNodes(std::move(nodes));
    model().setGeoGraph(graph);

    return true;
}

