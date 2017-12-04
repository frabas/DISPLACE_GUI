#ifndef VESSELSLOGLIKETABLE_H
#define VESSELSLOGLIKETABLE_H

#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>

class VesselsLoglikeTable : public SQLiteTable
{
    FieldDef<FieldType::Integer> fldId = makeFieldDef("Id", FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    FieldDef<FieldType::Integer> fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();

public:
    VesselsLoglikeTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    struct Log {
        int id;
        int tstep;
        types::NodeId node_id;
    };

    void insertLog (const Log &log);
};

#endif // VESSELSLOGLIKETABLE_H
