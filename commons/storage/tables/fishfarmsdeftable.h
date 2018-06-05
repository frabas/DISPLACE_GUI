//
// Created by Federico Fuga on 05/06/18.
//

#ifndef DISPLACE_FISHFARMSDEFTABLE_H
#define DISPLACE_FISHFARMSDEFTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"

#include <memory>

class Fishfarm;

class FishFarmsDefTable {
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    FishFarmsDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~FishFarmsDefTable() noexcept ;

    void dropAndCreate();
    void insertDef (const Fishfarm &fishfarm);

    using FishfarmSelectFunc = std::function<bool(int nodeid, std::shared_ptr<Fishfarm>)>;
    void getAllFishfarms (FishfarmSelectFunc func);
};


#endif //DISPLACE_FISHFARMSDEFTABLE_H
