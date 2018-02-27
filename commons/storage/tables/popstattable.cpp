#include "popstattable.h"

#include <Node.h>

struct PopStatTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>, FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>> insertStatement;
};

PopStatTable::PopStatTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name), p(std::make_unique<Impl>())
{
}

PopStatTable::~PopStatTable() noexcept = default;

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
    std::unique_lock<std::mutex> m(p->mutex);

    if (!p->init) {
        p->init = true;

        p->insertStatement = prepareInsert(std::make_tuple(fldTStep,
                                                           fldNodeId,
                                                           cumFTime,
                                                           cumSwA,
                                                           cumCatches,
                                                           cumDisc));
    }

    SQLiteTable::insert(p->insertStatement,
                        std::make_tuple(tstep,
                            (int)node->get_idx_node().toIndex(),
                            node->get_cumftime(),
                            node->get_cumsweptarea(),
                            node->get_cumcatches(),
                            node->get_cumdiscards())
                        );
}
