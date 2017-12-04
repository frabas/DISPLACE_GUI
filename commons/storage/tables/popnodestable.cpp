#include "popnodestable.h"

#include "Node.h"

PopNodesTable::PopNodesTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name)
{
    if (db->tableExists(name))
        db->dropTable(name);

    create(std::make_tuple(fldNodeId,
                           fldLong, fldLat
                           ));

}

void PopNodesTable::insert(Node *node)
{
    SQLiteTable::insert(fldNodeId.assign(node->get_idx_node().toIndex()),
                        fldLong.assign(node->get_x()),
                        fldLat.assign(node->get_y())
            );
}
