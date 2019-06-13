#include "popquotastable.h"

#include "Population.h"
#include "utils/make_unique.h"

struct PopQuotasTable::Impl {
    bool init = false;
    std::mutex mutex;
    PreparedInsert <FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Real>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>> insertStatement;
};

PopQuotasTable::PopQuotasTable(std::shared_ptr<SQLiteStorage> db, std::string name)
        : SQLiteTable(db, name), p(utils::make_unique<Impl>())
{
}

PopQuotasTable::~PopQuotasTable() noexcept = default;

void PopQuotasTable::dropAndCreate()
{
    if (db()->tableExists(name())) {
        db()->dropTable(name());
    }

    create(std::make_tuple(fldTStep,
                           fldPopId,
                           fldGroup,
                           fldQuotasUptake,
                           fldQuotas,
                           fldChoking
    ));
}

void PopQuotasTable::insert(int tstep, int popid, Population *pop)
{
    std::unique_lock<std::mutex> m(p->mutex);

    if (!p->init) {
        p->insertStatement = prepareInsert(std::make_tuple(fldTStep,
                                                           fldPopId,
                                                           fldGroup,
                                                           fldQuotasUptake,
                                                           fldQuotas,
                                                           fldChoking));
        p->init = true;
    }

    const auto &QuotasUptake = pop->get_quota_uptake();
    const auto &Quotas = pop->get_quota();
    const auto &Choking = static_cast<double>(pop->get_is_choking_fisheries());

    auto n = 1;

    for (size_t i = 0; i < n; ++i)
        SQLiteTable::insert(p->insertStatement, std::make_tuple(tstep,
                                                                popid,
                                                                (int) i,
                                                                (QuotasUptake),
                                                                (Quotas),
                                                                (Choking)
        ));
}
