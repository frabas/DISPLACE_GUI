#include "vesseldeftable.h"

using namespace sqlite;

VesselDefTable::VesselDefTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name)
{

}

std::string VesselDefTable::getCreateDefinition()
{
    return std::string{"id NUMERIC, name STRING"};
}
