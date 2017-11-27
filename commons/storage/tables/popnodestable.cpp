#include "popnodestable.h"

PopNodesTable::PopNodesTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name)
{
    if (db->tableExists(name))
        db->dropTable(name);

    create(std::make_tuple(fldNodeId,
                           fldTStep,
                           fldLong, fldLat
                           ));

}

void PopNodesTable::insert(int tstep, Node *node, std::multimap<int, double> init_weight_per_szgroup)
{

}
