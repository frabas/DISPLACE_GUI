#include "fishfarmstable.h"

#include "Fishfarm.h"

FishfarmsTable::FishfarmsTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db,name)
{
}

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
    insert(fldNodeId.assign(fishfarm->get_loc_ff()->get_idx_node().toIndex()),
           fldTStep.assign(tstep),
           fldFarmId.assign(fishfarm->get_name()),
           fldFarmType.assign(fishfarm->get_farmtype()),
           fldMeanW.assign(fishfarm->get_sim_individual_mean_kg()),
           fldFish.assign(fishfarm->get_sim_kg_harvested()),
           fldEggs.assign(fishfarm->get_sim_kg_eggs_harvested()),
           fldProfit.assign(fishfarm->get_sim_annual_profit()));
}
