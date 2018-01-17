#include "popstattable.h"

#include <Node.h>

PopStatTable::PopStatTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name)
{
}

void PopStatTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldNodeId,
                           fldTStep,
                           cumFTime,
                           cumSwA,
                           cumCatches,
                           cumDisc
                           ));
}

void PopStatTable::insert(int tstep, Node *node)
{
    SQLiteTable::insert(fldTStep.assign(tstep),
                        fldNodeId.assign(node->get_idx_node().toIndex()),
                        cumFTime.assign(node->get_cumftime()),
                        cumSwA.assign(node->get_cumsweptarea()),
                        cumCatches.assign(node->get_cumcatches()),
                        cumDisc.assign(node->get_cumdiscards())
                        );
}
