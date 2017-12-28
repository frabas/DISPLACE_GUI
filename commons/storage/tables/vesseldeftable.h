#ifndef VESSELDEFTABLE_H
#define VESSELDEFTABLE_H

#include "commons_global.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>

class COMMONSSHARED_EXPORT VesselDefTable : public sqlite::SQLiteTable
{
private:
    FieldDef<FieldType::Integer> fldId = makeFieldDef("Id", FieldType::Integer()).primaryKey();
    FieldDef<FieldType::Text> fldVesselId = makeFieldDef("VesselId",FieldType::Text()).unique().notNull();
    FieldDef<FieldType::Real> fldSpeeds = makeFieldDef("IdSpeeds",FieldType::Real());
    FieldDef<FieldType::Real> fldFuelcons = makeFieldDef("fldFuelcons",FieldType::Real());
    FieldDef<FieldType::Real> fldLengths = makeFieldDef("fldLengths",FieldType::Real());
    FieldDef<FieldType::Real> fldVKWs = makeFieldDef("fldVKWs",FieldType::Real());
    FieldDef<FieldType::Real> fldCarrycapacities = makeFieldDef("fldCarrycapacities",FieldType::Real());
    FieldDef<FieldType::Real> fldTankcapacities = makeFieldDef("fldTankcapacities",FieldType::Real());
    FieldDef<FieldType::Real> fldNbfpingspertrips = makeFieldDef("fldNbfpingspertrips",FieldType::Real());
    FieldDef<FieldType::Real> fldResttime_par1s = makeFieldDef("fldResttime_par1s",FieldType::Real());
    FieldDef<FieldType::Real> fldResttime_par2s = makeFieldDef("fldResttime_par2s",FieldType::Real());
    FieldDef<FieldType::Real> fldAv_trip_duration = makeFieldDef("fldAv_trip_duration",FieldType::Real());
    FieldDef<FieldType::Real> fldMult_fuelcons_when_steaming = makeFieldDef("fldMult_fuelcons_when_steaming",FieldType::Real());
    FieldDef<FieldType::Real> fldMult_fuelcons_when_fishing = makeFieldDef("fldMult_fuelcons_when_fishing",FieldType::Real());
    FieldDef<FieldType::Real> fldMult_fuelcons_when_returning = makeFieldDef("fldMult_fuelcons_when_returning",FieldType::Real());
    FieldDef<FieldType::Real> fldMult_fuelcons_when_inactive = makeFieldDef("fldMult_fuelcons_when_inactive",FieldType::Real());
    FieldDef<FieldType::Integer> fldFirm_ids = makeFieldDef("Firm",FieldType::Integer());

public:
    VesselDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    void dropAndCreate();

    void feedVesselsDefTable(const std::vector<std::string> &vesselids,
                             const std::vector<double> &speeds,
                             const std::vector<double> &length);    // TODO fill all the rest
protected:
};

#endif // VESSELDEFTABLE_H
