#include "vesselsloglikecatchestable.h"

#include <Population.h>

struct VesselsLoglikeCatchesTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>, FieldDef<FieldType::Real>, FieldDef<FieldType::Real>> insertStatement;
};

VesselsLoglikeCatchesTable::VesselsLoglikeCatchesTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

VesselsLoglikeCatchesTable::~VesselsLoglikeCatchesTable() noexcept = default;

void VesselsLoglikeCatchesTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(
               fldLoglikeId,
               fldPopId,
               fldCatches,
               fldDiscards
               ));
}

void VesselsLoglikeCatchesTable::insertPopulation(size_t rowid, int population, const std::vector<double> &catches, const std::vector<double> &discards)
{
    std::unique_lock<std::mutex> m(p->mutex);

    if (!p->init) {
        p->init = true;

        p->insertStatement = prepareInsert(std::make_tuple(fldLoglikeId,
                                                           fldPopId,
                                                           fldCatches,
                                                           fldDiscards));
    }

    SQLiteTable::insert(p->insertStatement, std::make_tuple((int)rowid,population,
                        catches[population],discards[population]));
}

