#include "nodestariffstattable.h"

#include "Node.h"

#include "msqlitecpp/v1/sqlitestatementformatters.h"
#include "msqlitecpp/v1/sqlitefieldsop.h"
#include "msqlitecpp/v1/sqlitestatement.h"

struct NodesTariffStatTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert <FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>, FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>, FieldDef<FieldType::Real>> insertStatement;
    sqlite::SQLiteStatement allNodesQueryStatement;
};

NodesTariffStatTable::NodesTariffStatTable(std::shared_ptr<SQLiteStorage> db, std::string name)
        : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

NodesTariffStatTable::~NodesTariffStatTable() noexcept = default;

void NodesTariffStatTable::dropAndCreate()
{
    if (db()->tableExists(name())) {
        db()->dropTable(name());
    }

    create(std::make_tuple(fldNodeId,
                           fldTStep,
                           tariffMet0,
                           tariffMet1,
                           tariffMet2
    ));
}

void NodesTariffStatTable::init()
{
    if (!p->init) {
        p->init = true;

        p->insertStatement = prepareInsert(std::make_tuple(fldTStep,
                                                           fldNodeId,
                                                           tariffMet0,
                                                           tariffMet1,
                                                           tariffMet2
        ));

        auto sqlAllQuery = sqlite::statements::Select(name(),
                                                      fldNodeId,
                                                      tariffMet0, tariffMet1, tariffMet2,
                                                      sqlite::op::max(fldTStep)
        )
                .where(sqlite::op::le(fldTStep))
                .groupBy(fldNodeId);

        p->allNodesQueryStatement = sqlite::SQLiteStatement(db(), sqlAllQuery);
    }
}

bool NodesTariffStatTable::insert(int tstep, Node *node)
{

    if (node->get_tariffs().at(0) == 0) { return 1; } // avoid exporting full of 0s lines

    std::unique_lock<std::mutex> m(p->mutex);
    init();

    SQLiteTable::insert(p->insertStatement,
                        std::make_tuple(tstep,
                                        (int) node->get_idx_node().toIndex(),
                                        node->get_tariffs().at(0),
                                        node->get_tariffs().at(1),
                                        node->get_tariffs().at(1) //TODO: adapt for more met
                        )
    );
    return 0;
}

void NodesTariffStatTable::queryAllNodesAtStep(int tstep, std::function<bool(NodesTariffStatTable::NodeTariffStat)> op)
{
    init();

    p->allNodesQueryStatement.bind(1, tstep);
    p->allNodesQueryStatement.execute([this, &op]() {
        auto &st = p->allNodesQueryStatement;
        NodeTariffStat s;
        s.nodeId = types::NodeId(st.getIntValue(0));
        s.tariffmet0 = st.getDoubleValue(1);
        s.tariffmet1 = st.getDoubleValue(2);
        s.tariffmet2 = st.getDoubleValue(2); //TODO: adapt for more met
        if (op) {
            return op(s);
        }
        return false;
    });
}
