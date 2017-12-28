#include "vesselslogliketable.h"

VesselsLoglikeTable::VesselsLoglikeTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name)
{
}

void VesselsLoglikeTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldId,
                           fldTStep,
                           fldNodeId
                           ));
}

void VesselsLoglikeTable::insertLog(const VesselsLoglikeTable::Log &log)
{
    insert(fldId.assign(log.id),
           fldTStep.assign(log.tstep),
           fldNodeId.assign(log.node_id.toIndex())
           );
}

