#include "popdyntable.h"

#include <Population.h>
#include "utils/make_unique.h"

struct PopDynTable::Impl {
    bool init = false;
    std::mutex mutex;
    PreparedInsert<FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Real>, FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>, FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
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
                           fldNz,
                           fldC,
                           fldD,
                           fldF,
                           fldravF,
                           fldM,
                           fldNa,
                           fldW,
                           fldMat,
                           fldSSB,
                           fldFFmsy,
                           fldPropMature
                           ));
}

void PopDynTable::insert(int tstep, int popid, Population *pop)
{
    std::unique_lock<std::mutex> m(p->mutex);

    if (!p->init) {
        p->insertStatement = prepareInsert(std::make_tuple(fldTStep,
                                           fldPopId,
                                           fldGroup,
                                           fldNz,
                                           fldC,
                                           fldD,
                                           fldF,
                                           fldravF,
                                           fldM,
                                           fldNa,
                                           fldW,
                                           fldMat,
                                           fldSSB,
                                           fldFFmsy,
                                           fldPropMature));
        p->init = true;
    }

    const auto &Nz = pop->get_tot_N_at_szgroup();
    const auto &C = pop->get_tot_C_at_szgroup();
    const auto &D = pop->get_tot_D_at_szgroup();
    const auto &F = pop->get_tot_F_at_age();
    const auto &ravF = pop->get_tot_F_at_age_running_average();
    const auto &M = pop->get_tot_M_at_age();
    const auto &Na = pop->get_tot_N_at_age();
    const auto &W = pop->get_tot_W_at_age();
    const auto &Mat = pop->get_tot_Mat_at_age();
    const auto &SSB = pop->get_SSB_at_szgroup();
    const auto &FFmsy = pop->get_FFmsy();
    const auto &PropMature = pop->get_proportion_mature_fish();

    auto n = std::max(Nz.size(), std::max(F.size(),  SSB.size()));

    for (size_t i = 0; i < n; ++i)
        SQLiteTable::insert(p->insertStatement, std::make_tuple(tstep,
                            popid,
                            (int)i,
                            (i < Nz.size() ? Nz.at(i) : -1),
                            (i < C.size() ? C.at(i) : -1),
                            (i < D.size() ? D.at(i) : -1),
                            (i < F.size() ? F.at(i) : -1),
                            (i < ravF.size() ? ravF.at(i) : -1),
                            (i < M.size() ? M.at(i) : -1),
                            (i < Na.size() ? Na.at(i) : -1),
                            (i < W.size() ? W.at(i) : -1),
                            (i < Mat.size() ? Mat.at(i) : -1),
                            (i < SSB.size() ? SSB.at(i) : -1),
                            (i < FFmsy.size() ? FFmsy.at(i) : -1),
                            PropMature
                            )
                    );
}
