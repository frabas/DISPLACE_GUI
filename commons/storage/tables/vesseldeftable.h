#ifndef VESSELDEFTABLE_H
#define VESSELDEFTABLE_H

#include "storage/sqlitetable.h"

class VesselDefTable : public sqlite::SQLiteTable
{
public:
    VesselDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);

    // SQLiteTable interface
protected:
    std::string getCreateDefinition() override;
};

#endif // VESSELDEFTABLE_H
