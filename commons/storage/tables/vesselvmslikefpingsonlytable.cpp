#include "vesselvmslikefpingsonlytable.h"

#include "insertstatement.h"
#include "selectstatement.h"
#include "clauses.h"
#include <sqlitefieldsop.h>

struct VesselVmsLikeFPingsOnlyTable::Impl
{
    FieldDef<FieldType::Integer> fldId = makeFieldDef("Id", FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldTStepDep = makeFieldDef("TStepDep",FieldType::Integer()).notNull();
    //const FieldDef<FieldType::Real> fldPosLong = makeFieldDef("Long", FieldType::Real()).notNull();
    //const FieldDef<FieldType::Real> fldPosLat = makeFieldDef("Lat", FieldType::Real()).notNull();
    //const FieldDef<FieldType::Real> fldCourse = makeFieldDef("Course", FieldType::Real()).notNull();
    //const FieldDef<FieldType::Real> fldCumFuel = makeFieldDef("CumFuel", FieldType::Real()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId", FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId", FieldType::Integer()).notNull();

    std::mutex mutex;
    bool init = false;

    InsertStatement<decltype(fldId),
                decltype(fldTStep),
                decltype(fldTStepDep),
                decltype(fldNodeId),
                decltype(fldPopId)> insertStatement;
    SelectStatement<decltype(fldId),
                decltype(fldTStep),
                decltype(fldTStepDep),
                decltype(fldNodeId),
                decltype(fldPopId),
                decltype(fldTStep)>
        selectStatement;
    Where<decltype(fldTStep)> where;


    Impl()
        : insertStatement(fldId, fldTStep, fldTStepDep,
                          fldNodeId, fldPopId),
          selectStatement(fldId, fldTStep, fldTStepDep,
                          fldNodeId, fldPopId, op::max(fldTStep))
    {

    }
};

VesselVmsLikeFPingsOnlyTable::VesselVmsLikeFPingsOnlyTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
    create();
    p->insertStatement.doReplace();
    p->insertStatement.attach(db,name);
    p->selectStatement.attach(db,name);
    p->where.attach(p->selectStatement.getStatement(), op::eq(p->fldId));

    p->selectStatement.prepare();
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
        /*
        auto def = std::make_tuple (
                    p->fldId, p->fldTStep, p->fldTStepDep,
                    //fldPosLong, fldPosLat, fldCourse,
                    //fldCumFuel,
                    p->fldNodeId,
                    p->fldPopId
                    );

        SQLiteTable::create(def);*/
        SQLiteStatement stmt (db(),
                              "CREATE TABLE VesselVmsFPingsOnlyLike ("
                                "Id       INTEGER NOT NULL,"
                                "TStep    INTEGER NOT NULL,"
                                "TStepDep INTEGER NOT NULL,"
                                "NodeId   INTEGER NOT NULL,"
                                "PopId    INTEGER NOT NULL,"
                                "CONSTRAINT keyIdStep PRIMARY KEY (Id,NodeId,PopId) ON CONFLICT REPLACE"
                               ");");
        stmt.execute();
    }
}

void VesselVmsLikeFPingsOnlyTable::insertLog(const VesselVmsLikeFPingsOnlyTable::Log &log)
{
    std::unique_lock<std::mutex> m(p->mutex);

    p->insertStatement.insert(log.id,
                              log.tstep,
                              log.tstep_dep,
                              //log.p_long,
                              //log.p_lat,
                              //log.p_course,
                              //log.cum_fuel,
                              log.nodeid,
                              log.popid);
}

void VesselVmsLikeFPingsOnlyTable::queryAllVesselsAtStep(int tstep, std::function<bool (const VesselVmsLikeFPingsOnlyTable::Log &)> op)
{
    std::unique_lock<std::mutex> m(p->mutex);

    p->where.bind(tstep);
    p->selectStatement.exec([&op] (int id, int tstep, int tstepdep, int nodeid, int popid, int tstepmax){
        Log l;
        l.id = id;
        l.tstep = tstep;
        l.nodeid = nodeid;
        l.popid = popid;
        if (op)
            return op(l);
        return false;
    });
}
