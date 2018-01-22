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

    create(std::make_tuple(fldTStep,
                           fldPopId,
                           fldGroup,
                           fldN,
                           fldF,
                           fldSSB
                           ));
}

void PopDynTable::insert(int tstep, int popid, Population *pop)
{
    const auto &N = pop->get_tot_N_at_szgroup();
    const auto &F = pop->get_tot_F_at_age();
    const auto &SSB = pop->get_SSB_at_szgroup();

    auto n = std::max(N.size(), std::max(F.size(), SSB.size()));

    for (size_t i = 0; i < n; ++i)
        SQLiteTable::insert(fldTStep.assign(tstep),
                            fldPopId.assign(popid),
                            fldGroup.assign(i),
                            fldN.assign(i < N.size() ? N.at(i) : -1),
                            fldF.assign(i < F.size() ? F.at(i) : -1),
                            fldSSB.assign(i < SSB.size() ? SSB.at(i) : -1)
                    );
}
