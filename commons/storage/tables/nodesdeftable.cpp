#include "nodesdeftable.h"

#include "Node.h"

NodesDefTable::NodesDefTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name)
{    
}

void NodesDefTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldNodeId,
                           fldNodeName,
                           fldLong, fldLat
                           ));
}

void NodesDefTable::insert(Node *node)
{
    SQLiteTable::insert(fldNodeId.assign(node->get_idx_node().toIndex()),
                        fldNodeName.assign(node->get_name()),
                        fldLong.assign(node->get_x()),
                        fldLat.assign(node->get_y())
            );
}
