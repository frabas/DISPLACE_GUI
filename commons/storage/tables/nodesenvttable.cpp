

#include "nodesenvttable.h"

#include <Node.h>

#include <sqlitestatementformatters.h>
#include <sqlitefieldsop.h>
#include <sqlitestatement.h>

struct NodesEnvtTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>,
                   FieldDef<FieldType::Integer>,
                   FieldDef<FieldType::Integer>,
                   FieldDef<FieldType::Real>,
                   FieldDef<FieldType::Real>,
                   FieldDef<FieldType::Real>,
                   FieldDef<FieldType::Real>,
                   FieldDef<FieldType::Real>,
                   FieldDef<FieldType::Real>,
                   FieldDef<FieldType::Real>
    > insertStatement;
    sqlite::SQLiteStatement allNodesQueryStatement;
};

NodesEnvtTable::NodesEnvtTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name), p(std::make_unique<Impl>())
{
}

NodesEnvtTable::~NodesEnvtTable() noexcept = default;

void NodesEnvtTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldTStep,
                           fldNodeId,
                           marineLandscape,
                           salinity,
                           sst,
                           wind,
                           nitrogen,
                           phosphorus,
                           oxygen,
                           dissolvedcarbon
                           ));
}

void NodesEnvtTable::init()
{
    if (!p->init) {
        p->init = true;

        p->insertStatement = prepareInsert(std::make_tuple(fldTStep,
                                                           fldNodeId,
                                                           marineLandscape,
                                                           salinity,
                                                           sst,
                                                           wind,
                                                           nitrogen,
                                                           phosphorus,
                                                           oxygen,
                                                           dissolvedcarbon));

        auto sqlAllQuery = sqlite::statements::Select(name(),
                                                      fldNodeId,
                                                      marineLandscape,
                                                      salinity,
                                                      sst,
                                                      wind,
                                                      nitrogen,
                                                      phosphorus,
                                                      oxygen,
                                                      dissolvedcarbon,
                                                      sqlite::op::max(fldTStep)
                                                      )
                .where (sqlite::op::le(fldTStep))
                .groupBy (fldNodeId);

        p->allNodesQueryStatement = sqlite::SQLiteStatement(db(), sqlAllQuery);
    }
}

bool NodesEnvtTable::insert(int tstep, Node *node)
{


    std::unique_lock<std::mutex> m(p->mutex);
    init();

    SQLiteTable::insert(p->insertStatement,
                        std::make_tuple(tstep,
                                        (int)node->get_idx_node().toIndex(),
                                        (int)node->get_marine_landscape(),
                                        node->get_salinity(),
                                        node->get_sst(),
                                        node->get_wind(),
                                        node->get_Nitrogen(),
                                        node->get_Phosphorus(),
                                        node->get_Oxygen(),
                                        node->get_DissolvedCarbon())
                        );
return 0;
}

void NodesEnvtTable::queryAllNodesAtStep(int tstep, std::function<bool (NodesEnvtTable::NodeEnvt)> op)
{
    init();

    p->allNodesQueryStatement.bind(1, tstep);
    p->allNodesQueryStatement.execute([this, &op](){
        auto &st = p->allNodesQueryStatement;
        NodeEnvt s;
        s.nodeId = types::NodeId(st.getIntValue(0));
        s.marineLandscape = st.getIntValue(1);
        s.salinity = st.getDoubleValue(2);
        s.sst = st.getDoubleValue(3);
        s.wind = st.getDoubleValue(4);
        s.nitrogen = st.getDoubleValue(5);
        s.phosphorus= st.getDoubleValue(6);
        s.oxygen = st.getDoubleValue(7);
        s.dissolvedcarbon = st.getDoubleValue(8);
        if (op)
            return op(s);
        return false;
    });
}
