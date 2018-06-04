#include "nodesstattable.h"

#include <Node.h>

#include <sqlitestatementformatters.h>
#include <sqlitefieldsop.h>
#include <sqlitestatement.h>

struct NodesStatTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>, FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>, FieldDef<FieldType::Real>> insertStatement;
    sqlite::SQLiteStatement allNodesQueryStatement;
};

NodesStatTable::NodesStatTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name), p(std::make_unique<Impl>())
{
}

NodesStatTable::~NodesStatTable() noexcept = default;

void NodesStatTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldNodeId,
                           fldTStep,
                           cumFTime,
                           cumSwA,
                           cumSubSurfSwA,
                           cumCatches,
                           cumCatchesThrshld,
                           cumDisc,
                           cumDiscRatio
                           ));
}

void NodesStatTable::init()
{
    if (!p->init) {
        p->init = true;

        p->insertStatement = prepareInsert(std::make_tuple(fldTStep,
                                                           fldNodeId,
                                                           cumFTime,
                                                           cumSwA,
                                                           cumSubSurfSwA,
                                                           cumCatches,
                                                           cumCatchesThrshld,
                                                           cumDisc,
                                                           cumDiscRatio));

        auto sqlAllQuery = sqlite::statements::Select(name(),
                                                      fldNodeId,
                                                      cumFTime, cumSwA, cumSubSurfSwA, cumCatches, cumCatchesThrshld, cumDisc, cumDiscRatio,
                                                      sqlite::op::max(fldTStep)
                                                      )
                .where (sqlite::op::le(fldTStep))
                .groupBy (fldNodeId);

        p->allNodesQueryStatement = sqlite::SQLiteStatement(db(), sqlAllQuery);
    }
}

bool NodesStatTable::insert(int tstep, Node *node)
{

    if(node->get_cumftime()==0) return 1;

    std::unique_lock<std::mutex> m(p->mutex);
    init();

    double discratio= ((node->get_cumcatches()+node->get_cumdiscards())>0) ? node->get_cumdiscards()/(node->get_cumcatches()+node->get_cumdiscards()) :  0;

    SQLiteTable::insert(p->insertStatement,
                        std::make_tuple(tstep,
                            (int)node->get_idx_node().toIndex(),
                            node->get_cumftime(),
                            node->get_cumsweptarea(),
                            node->get_cumsubsurfacesweptarea(),
                            node->get_cumcatches(),
                            node->get_cumcatches_with_threshold(),
                            node->get_cumdiscards(),
                            discratio)
                        );
return 0;
}

void NodesStatTable::queryAllNodesAtStep(int tstep, std::function<bool (NodesStatTable::NodeStat)> op)
{
    init();

    p->allNodesQueryStatement.bind(1, tstep);
    p->allNodesQueryStatement.execute([this, &op](){
        auto &st = p->allNodesQueryStatement;
        NodeStat s;
        s.nodeId = types::NodeId(st.getIntValue(0));
        s.cumftime = st.getDoubleValue(1);
        s.cumswa = st.getDoubleValue(2);
        s.cumsubsurfswa = st.getDoubleValue(3);
        s.cumcatches = st.getDoubleValue(4);
        s.cumcatchesthrshld= st.getDoubleValue(5);
        s.cumdisc = st.getDoubleValue(6);
        s.tstep = st.getIntValue(7);
        s.cumcatchesthrshld= st.getDoubleValue(5);
        s.cumdisc = st.getDoubleValue(6);
        s.cumdiscratio = st.getDoubleValue(7);
        s.tstep = st.getIntValue(8);
        if (op)
            return op(s);
        return false;
    });
}
