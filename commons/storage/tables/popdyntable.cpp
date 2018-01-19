#include "popdyntable.h"

#include <Population.h>

#include <assert.h>

PopDynTable::PopDynTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name)
{

}

void PopDynTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldNodeId,
                           fldTStep,
                           fldPopId,
                           fldN,
                           fldF,
                           fldSSB
                           ));
}

void PopDynTable::insert(int tstep, Population *pop)
{
    const auto &N = pop->get_tot_N_at_szgroup();
    const auto &F = pop->get_tot_F_at_age();
    const auto &SSB = pop->get_SSB_at_szgroup();
}
