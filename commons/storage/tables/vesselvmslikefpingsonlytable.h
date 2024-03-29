#ifndef VESSELVMSLIKEFPINGSONLYTABLE_H
#define VESSELVMSLIKEFPINGSONLYTABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include <functional>

#include "msqlitecpp/v1/sqlitefielddef.h"
#include "msqlitecpp/v1/sqlitetable.h"

#include <vector>
#include <string>

using namespace sqlite;

class  VesselVmsLikeFPingsOnlyTable : public sqlite::SQLiteTable {
private:
    struct Impl;
    std::unique_ptr<Impl> p;

    void create();

public:
    VesselVmsLikeFPingsOnlyTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    ~VesselVmsLikeFPingsOnlyTable() noexcept;

    void dropAndCreate();

    struct Log {
        int id;
        int month;
        int tstep;
        int tstep_dep;
        int nodeid;
        int popid;
        int szGroup;
        double catches;
    };

    void insertLog(const Log &log);

    void queryAllVesselsAtStep(int tstep, std::function<bool(const Log &)>);

    void deleteAllVesselsBeforeMonth(int month);
};

#endif // VESSELVMSLIKEFPINGSONLYTABLE_H
