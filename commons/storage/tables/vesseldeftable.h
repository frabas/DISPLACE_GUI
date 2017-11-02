#ifndef VESSELDEFTABLE_H
#define VESSELDEFTABLE_H

#include "storage/sqlitetable.h"

class VesselDefTable : public SQLiteTable
{
public:
    VesselDefTable(std::string name);
};

#endif // VESSELDEFTABLE_H
