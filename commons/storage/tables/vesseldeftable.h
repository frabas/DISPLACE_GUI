#ifndef VESSELDEFTABLE_H
#define VESSELDEFTABLE_H

#include "storage/sqlitetable.h"

class VesselDefTable : public SQLiteTable
{
public:
    VesselDefTable(std::shared_ptr<SQLiteResultsStorage> db, std::string name);

    // SQLiteTable interface
protected:
    std::string getCreateDefinition() override;
};

#endif // VESSELDEFTABLE_H
