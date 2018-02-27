#ifndef POPTABLE_H
#define POPTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>
#include <map>

class Node;

class COMMONSSHARED_EXPORT PopTable : public SQLiteTable
{
    struct Impl;
    std::unique_ptr<Impl> p;

    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldPopId = makeFieldDef("PopId",FieldType::Integer()).notNull();

    FieldDef<FieldType::Real> fldTotNId = makeFieldDef("TotalN",FieldType::Real()).notNull();
    FieldDef<FieldType::Real> fldTotWId = makeFieldDef("TotalW",FieldType::Real()).notNull();

public:
    PopTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~PopTable() noexcept;
    void dropAndCreate();

    void insert (int tstep, Node *node, const std::multimap<int, double> &initstate);

    size_t getNbPops();
};

#endif // POPTABLE_H
