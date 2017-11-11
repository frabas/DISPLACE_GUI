#ifndef VESSELDEFTABLE_H
#define VESSELDEFTABLE_H

#include "sqlitetable.h"

class VesselDefTable : public sqlite::SQLiteTable
{
public:
    VesselDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    // SQLiteTable interface
protected:
};

#endif // VESSELDEFTABLE_H
