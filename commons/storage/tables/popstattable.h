#ifndef POPSTATTABLE_H
#define POPSTATTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class COMMONSSHARED_EXPORT PopStatTable : public SQLiteTable
{
private:
    struct Impl;

    std::unique_ptr<Impl> p;

    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();

    FieldDef<FieldType::Real> cumFTime = makeFieldDef("CumFTime",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> cumSwA = makeFieldDef("CumSwA",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> cumCatches = makeFieldDef("CumCatches",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> cumDisc = makeFieldDef("CumDiscards",FieldType::Real()).notNull();

public:
    PopStatTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~PopStatTable() noexcept;
    void dropAndCreate();

    void insert (int tstep, Node *node);
};

#endif // POPSTATTABLE_H
