

#include "nodesdeftable.h"

#include "Node.h"
#include "Harbour.h"
#include "msqlitecpp/v1/sqlitestatementformatters.h"
#include "msqlitecpp/v1/sqlitestatement.h"

struct NodesDefTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert <FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Text>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>,
    FieldDef<FieldType::Integer>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>> statement;
};

NodesDefTable::NodesDefTable(std::shared_ptr<SQLiteStorage> db, std::string name)
        : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

NodesDefTable::~NodesDefTable() noexcept = default;

void NodesDefTable::dropAndCreate()
{
    if (db()->tableExists(name())) {
        db()->dropTable(name());
    }

    create(std::make_tuple(fldNodeId,
                           fldHarbourId,
                           fldNodeName,
                           fldLong, fldLat,
                           marineLandscape,
                           bathymetry,
                           shippingdensity,
                           siltfraction,
                           icesrectanglecode
    ));
}

void NodesDefTable::insert(Node *node)
{
    std::unique_lock<std::mutex> m(p->mutex);
    if (!p->init) {
        p->init = true;
        p->statement = prepareInsert(std::make_tuple(fldNodeId, fldHarbourId,
                                                     fldNodeName,
                                                     fldLong, fldLat, marineLandscape,
                                                     bathymetry, shippingdensity, siltfraction, icesrectanglecode));
    }

    SQLiteTable::insert(p->statement, std::make_tuple((int) node->get_idx_node().toIndex(),
                                                      node->get_harbour(),
                                                      node->get_name(),
                                                      node->get_x(),
                                                      node->get_y(),
                                                      node->get_marine_landscape(),
                                                      node->get_bathymetry(),
                                                      node->get_shippingdensity(),
                                                      node->get_siltfraction(),
                                                      node->get_icesrectanglecode()
                        )
    );
}

void NodesDefTable::queryAllNodes(std::function<void(std::shared_ptr<Node>, std::shared_ptr<Harbour>)> operation)
{
    sqlite::statements::Select s(name(), fldNodeId, fldHarbourId, fldNodeName, fldLong, fldLat, marineLandscape,
                                 bathymetry, shippingdensity, siltfraction, icesrectanglecode);
    sqlite::SQLiteStatement stmt(db(), s);

    stmt.execute([&operation, &stmt]() {
        std::shared_ptr<Node> n;
        std::shared_ptr<Harbour> h;
        auto hid = stmt.getIntValue(1);
        auto name = stmt.getStringValue(2);

        if (hid == 0) {
            n = std::make_shared<Node>();
        } else {
            h = std::make_shared<Harbour>(name);
            n = h;
        }
        n->set_is_harbour(hid);
        n->set_idx_node(types::NodeId(stmt.getIntValue(0)));
        n->set_xy(stmt.getDoubleValue(3), stmt.getDoubleValue(4));
        n->setMarineLandscape(stmt.getIntValue(5));
        n->setBathymetry(stmt.getDoubleValue(6));
        n->setShippingdensity(stmt.getDoubleValue(7));
        n->setSiltfraction(stmt.getDoubleValue(8));
        n->setIcesrectanglecode(stmt.getDoubleValue(9));

        operation(n, h);
        return true;
    });
}
