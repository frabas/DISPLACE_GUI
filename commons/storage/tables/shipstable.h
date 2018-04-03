#ifndef SHIPSTABLE_H
#define SHIPSTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitetable.h"

#include <string>

class Ship;

class COMMONSSHARED_EXPORT ShipsTable : public sqlite::SQLiteTable
{
    struct Impl;
    std::unique_ptr<Impl> p;
public:
    ShipsTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~ShipsTable() noexcept;

    void dropAndCreate();
    void exportShipsIndivators(int tstep, Ship* ship);
};

#endif // SHIPSTABLE_H
