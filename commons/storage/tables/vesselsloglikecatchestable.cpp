#include "vesselsloglikecatchestable.h"

#include <Population.h>

VesselsLoglikeCatchesTable::VesselsLoglikeCatchesTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name)
{
}

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
    SQLiteTable::insert(fldLoglikeId.assign(rowid),
                        fldPopId.assign(population),
                        fldCatches.assign(catches[population]),
                        fldDiscards.assign(discards[population]));
}

