#include "fishfarmstable.h"

#include "Fishfarm.h"

struct FishfarmsTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<FieldDef<FieldType::Integer>,FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Integer>, FieldDef<FieldType::Integer>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
        FieldDef<FieldType::Real>,FieldDef<FieldType::Real>> statement;
};


FishfarmsTable::FishfarmsTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name), p(std::make_unique<Impl>())
{
}

FishfarmsTable::~FishfarmsTable() noexcept = default;

void FishfarmsTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(fldNodeId,
                           fldTStep,
                           fldFarmId,
                           fldFarmType,
                           fldMeanW,
                           fldFish,
                           fldEggs,
                           fldProfit
                           ));
}


void FishfarmsTable::exportFishfarmLog(Fishfarm *fishfarm, int tstep)
{
    std::unique_lock<std::mutex> m(p->mutex);
    if (!p->init) {
        p->init = true;
        p->statement = prepareInsert(std::make_tuple(fldNodeId,
                                                     fldTStep,
                                                     fldFarmId,
                                                     fldFarmType,fldMeanW,fldFish,fldEggs,fldProfit));
    }

    insert(p->statement, std::make_tuple(
               (int)fishfarm->get_loc_ff()->get_idx_node().toIndex(),
                tstep,
           fishfarm->get_name(),
           fishfarm->get_farmtype(),
           fishfarm->get_sim_individual_mean_kg(),
           fishfarm->get_sim_kg_harvested(),
           fishfarm->get_sim_kg_eggs_harvested(),
           fishfarm->get_sim_annual_profit())
           );
}
