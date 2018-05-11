#include "vesselvmslikefpingsonlytable.h"

#include "insertstatement.h"
#include "selectstatement.h"
#include "createstatement.h"
#include "deletestatement.h"
#include "clauses.h"

#include <sqlitefieldsop.h>

struct VesselVmsLikeFPingsOnlyTable::Impl
{
    std::shared_ptr<sqlite::SQLiteStorage> db;
    std::string name;

    FieldDef<FieldType::Integer> fldId {"VesselId", NotNull };
    FieldDef<FieldType::Integer> fldTStep { "TStep", NotNull };
    FieldDef<FieldType::Integer> fldMonth {"Month", NotNull };
    FieldDef<FieldType::Integer> fldTStepDep { "TStepDep", NotNull };
    FieldDef<FieldType::Integer> fldNodeId { "NodeId", NotNull };
    FieldDef<FieldType::Integer> fldPopId { "PopId", NotNull };
    FieldDef<FieldType::Integer> fldSzGroupId { "SzGrpId", NotNull };
    FieldDef<FieldType::Real> fldCatches { "Catches", NotNull };

    std::mutex mutex;
    bool init = false;

    InsertStatement<
                decltype(fldId),
                decltype(fldTStep),
                decltype(fldMonth),
                decltype(fldTStepDep),
                decltype(fldNodeId),
                decltype(fldPopId),
                decltype(fldSzGroupId),
                decltype(fldCatches)
    > insertStatement;
    SelectStatement<
                decltype(fldId),
                decltype(fldTStep),
                decltype(fldMonth),
                decltype(fldTStepDep),
                decltype(fldNodeId),
                decltype(fldPopId),
                decltype(fldSzGroupId),
                decltype(fldCatches),
                decltype(fldTStep)
    > selectStatement;
    Where<decltype(fldTStep)> where;

    DeleteStatement deleteStatement;
    Where<decltype(fldMonth)> deleteStatementWhere;

    Impl(std::shared_ptr<sqlite::SQLiteStorage> mydb, std::string myname)
        : db(mydb), name(myname),
          insertStatement(fldId, fldTStep, fldMonth, fldTStepDep,
                          fldNodeId, fldPopId, fldSzGroupId, fldCatches),
          selectStatement(fldId, fldTStep, fldMonth, fldTStepDep,
                          fldNodeId, fldPopId, fldSzGroupId,fldCatches, op::max(fldTStep))
    {

    }

    Impl &create () {
        std::cout << " REAL CREATE\n";
        CreateTableStatement<
                decltype(fldId),
                decltype(fldTStep),
                decltype(fldMonth),
                decltype(fldTStepDep),
                decltype(fldNodeId),
                decltype(fldPopId),
                decltype(fldSzGroupId),
                decltype(fldCatches)
           > create (fldId, fldTStep, fldMonth, fldTStepDep,
                     fldNodeId, fldPopId, fldSzGroupId, fldCatches);

        create.attach(db, name);

        statements::CreateTable::TableConstraint::PrimaryKey primaryKey(
                    "unpk", fldId, fldNodeId, fldPopId, fldSzGroupId);
        create.setTableConstraint(primaryKey.toString() + " ON CONFLICT REPLACE");
        create.execute();

        return *this;
    }
};

VesselVmsLikeFPingsOnlyTable::VesselVmsLikeFPingsOnlyTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>(db,name))
{
    std::cout << " ---- Create constructor\n";
    create();
    p->insertStatement.doReplace();
    p->insertStatement.attach(db,name);
    p->selectStatement.attach(db,name);
    p->where.attach(p->selectStatement.getStatement(), op::eq(p->fldId));

    p->selectStatement.prepare();

    p->deleteStatementWhere.attach(p->deleteStatement.getStatement(), op::eq(p->fldMonth));
    p->deleteStatement.attach(db, name);
    p->deleteStatement.where(p->deleteStatementWhere);
    p->deleteStatement.prepare();
}

VesselVmsLikeFPingsOnlyTable::~VesselVmsLikeFPingsOnlyTable() noexcept = default;

void VesselVmsLikeFPingsOnlyTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());
    create();
}

void VesselVmsLikeFPingsOnlyTable::create()
{
    if (!db()->tableExists(name())) {
        p->create();
    }
}

void VesselVmsLikeFPingsOnlyTable::insertLog(const VesselVmsLikeFPingsOnlyTable::Log &log)
{
    std::unique_lock<std::mutex> m(p->mutex);

    p->insertStatement.insert(log.id,
                              log.tstep,
                              log.tstep / 745,
                              log.tstep_dep,
                              log.nodeid,
                              log.popid,
                              log.szGroup,
                              log.catches
                              );
}

void VesselVmsLikeFPingsOnlyTable::queryAllVesselsAtStep(int tstep, std::function<bool (const VesselVmsLikeFPingsOnlyTable::Log &)> op)
{
    std::unique_lock<std::mutex> m(p->mutex);

    p->where.bind(tstep);
    p->selectStatement.exec([&op] (int id, int tstep, int month, int tstepdep, int nodeid, int popid, int szGroup, double catches, int tstepmax){
        Log l;
        l.id = id;
        l.tstep = tstep;
        l.month = month;
        l.tstep_dep = tstepdep;
        l.nodeid = nodeid;
        l.popid = popid;
        l.szGroup = szGroup;
        l.catches = catches;
        if (op)
            return op(l);
        return false;
    });
}

void VesselVmsLikeFPingsOnlyTable::deleteAllVesselsBeforeMonth(int month)
{
    p->deleteStatementWhere.bind(month);
    p->deleteStatement.exec();
}


