

#include "nodesenvttable.h"

#include <Node.h>

#include <sqlitestatementformatters.h>
#include <sqlitefieldsop.h>
#include <sqlitestatement.h>
#include <selectstatement.h>
#include <clauses.h>

struct NodesEnvtTable::Impl {
    std::mutex mutex;
    bool init = false;

    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();

    FieldDef<FieldType::Integer> marineLandscape = makeFieldDef("marineLandscape",FieldType::Integer()).notNull();
    FieldDef<FieldType::Real> salinity = makeFieldDef("salinity",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> sst = makeFieldDef("sst",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> wind = makeFieldDef("wind",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> nitrogen = makeFieldDef("nitrogen",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> phosphorus = makeFieldDef("phosphorus",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> oxygen = makeFieldDef("oxygen",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> dissolvedcarbon = makeFieldDef("dissolvedcarbon",FieldType::Real()).notNull();


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

    sqlite::SelectStatement<
        FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>
            >nodeQueryStatement;
    sqlite::Where<FieldType::Integer, FieldType::Integer> nodeQueryWhere;

    Impl()
        : nodeQueryStatement(op::max(fldTStep), fldNodeId, marineLandscape, salinity, sst, wind, nitrogen, phosphorus, oxygen, dissolvedcarbon) {

    }

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

    create(std::make_tuple(p->fldTStep,
                           p->fldNodeId,
                           p->marineLandscape,
                           p->salinity,
                           p->sst,
                           p->wind,
                           p->nitrogen,
                           p->phosphorus,
                           p->oxygen,
                           p->dissolvedcarbon
                           ));
}

void NodesEnvtTable::init()
{
    if (!p->init) {
        p->init = true;

        p->insertStatement = prepareInsert(std::make_tuple(p->fldTStep,
                                                           p->fldNodeId,
                                                           p->marineLandscape,
                                                           p->salinity,
                                                           p->sst,
                                                           p->wind,
                                                           p->nitrogen,
                                                           p->phosphorus,
                                                           p->oxygen,
                                                           p->dissolvedcarbon));

        auto sqlAllQuery = sqlite::statements::Select(name(),
                                                      p->fldNodeId,
                                                      p->marineLandscape,
                                                      p->salinity,
                                                      p->sst,
                                                      p->wind,
                                                      p->nitrogen,
                                                      p->phosphorus,
                                                      p->oxygen,
                                                      p->dissolvedcarbon,
                                                      sqlite::op::max(p->fldTStep)
                                                      )
                .where (sqlite::op::le(p->fldTStep))
                .groupBy (p->fldNodeId);

        p->allNodesQueryStatement = sqlite::SQLiteStatement(db(), sqlAllQuery);

        p->nodeQueryStatement.attach(db(), name());
        p->nodeQueryWhere.attach(p->nodeQueryStatement.getStatement(), sqlite::op::and_(sqlite::op::eq(p->fldNodeId), sqlite::op::le(p->fldTStep)));
        p->nodeQueryStatement.where(p->nodeQueryWhere);
        p->nodeQueryStatement.groupBy(p->fldNodeId);
        std::cout << "NodesEvt: " << p->nodeQueryStatement.statementString() << "\n";
        p->nodeQueryStatement.prepare();
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

void NodesEnvtTable::queryAllNodesAtStep(types::tstep_t tstep, std::function<bool (NodesEnvtTable::NodeEnvt)> op)
{
    init();

    p->allNodesQueryStatement.bind(1, tstep.value());
    p->allNodesQueryStatement.execute([this, &op, tstep](){
        auto &st = p->allNodesQueryStatement;
        NodeEnvt s;
        s.nodeId = types::NodeId(st.getIntValue(0));
        s.tstep = types::tstep_t(tstep);
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

void NodesEnvtTable::queryNodeAtStep(types::NodeId nodeId, types::tstep_t tstep, std::function<bool (NodesEnvtTable::NodeEnvt)> op)
{
    init();

    p->nodeQueryWhere.bind(nodeId.toIndex(), tstep.value());
    p->nodeQueryStatement.exec([this, &op](
                                   int nodeid, int tstep, int marinelandscape,
                                   double salinity, double sst, double wind,
                                   double no, double p, double o2, double co2
                                   ) {
        NodeEnvt s;
        s.nodeId = types::NodeId(nodeid);
        s.tstep = types::tstep_t(tstep);
        s.marineLandscape = marinelandscape;
        s.salinity = salinity;
        s.sst = sst;
        s.wind = wind;
        s.nitrogen = no;
        s.phosphorus= p;
        s.oxygen = o2;
        s.dissolvedcarbon = co2;
        if (op)
            return op(s);
        return false;
    });
}
