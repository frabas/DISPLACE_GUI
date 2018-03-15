#include "popdyntable.h"

#include <Population.h>
#include "utils/make_unique.h"

struct PopDynTable::Impl {
    bool init = false;
    std::mutex mutex;
    PreparedInsert<FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real> > insertStatement;
};

PopDynTable::PopDynTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name), p(utils::make_unique<Impl>())
{
}

PopDynTable::~PopDynTable() noexcept = default;

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
    std::unique_lock<std::mutex> m(p->mutex);

    if (!p->init) {
        p->insertStatement = prepareInsert(std::make_tuple(fldTStep,
                                           fldPopId,
                                           fldGroup,
                                           fldN,
                                           fldF,
                                           fldSSB));
        p->init = true;
    }

    const auto &N = pop->get_tot_N_at_szgroup();
    const auto &F = pop->get_tot_F_at_age();
    const auto &SSB = pop->get_SSB_at_szgroup();

    auto n = std::max(N.size(), std::max(F.size(), SSB.size()));

    for (size_t i = 0; i < n; ++i)
        SQLiteTable::insert(p->insertStatement, std::make_tuple(tstep,
                            popid,
                            (int)i,
                            (i < N.size() ? N.at(i) : -1),
                            (i < F.size() ? F.at(i) : -1),
                            (i < SSB.size() ? SSB.at(i) : -1))
                    );
}
