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
    FieldDef<FieldType::Integer> fldId = makeFieldDef("VesselId", FieldType::Integer()).primaryKey();
    FieldDef<FieldType::Text> fldName = makeFieldDef("VesselName",FieldType::Text()).unique().notNull();
    FieldDef<FieldType::Text> fldNationality = makeFieldDef("Nationality",FieldType::Text());
    FieldDef<FieldType::Real> fldSpeeds = makeFieldDef("IdSpeeds",FieldType::Real());
    FieldDef<FieldType::Real> fldFuelcons = makeFieldDef("Fuelcons",FieldType::Real());
    FieldDef<FieldType::Real> fldLengths = makeFieldDef("Lengths",FieldType::Real());
    FieldDef<FieldType::Real> fldVKWs = makeFieldDef("VKWs",FieldType::Real());
    FieldDef<FieldType::Real> fldCarrycapacities = makeFieldDef("Carrycapacities",FieldType::Real());
    FieldDef<FieldType::Real> fldTankcapacities = makeFieldDef("Tankcapacities",FieldType::Real());
    FieldDef<FieldType::Real> fldNbfpingspertrips = makeFieldDef("Nbfpingspertrips",FieldType::Real());
    FieldDef<FieldType::Real> fldResttime_par1s = makeFieldDef("Resttime_par1s",FieldType::Real());
    FieldDef<FieldType::Real> fldResttime_par2s = makeFieldDef("Resttime_par2s",FieldType::Real());
    FieldDef<FieldType::Real> fldAv_trip_duration = makeFieldDef("Av_trip_duration",FieldType::Real());
    FieldDef<FieldType::Real> fldMult_fuelcons_when_steaming = makeFieldDef("Mult_fuelcons_when_steaming",FieldType::Real());
    FieldDef<FieldType::Real> fldMult_fuelcons_when_fishing = makeFieldDef("Mult_fuelcons_when_fishing",FieldType::Real());
    FieldDef<FieldType::Real> fldMult_fuelcons_when_returning = makeFieldDef("Mult_fuelcons_when_returning",FieldType::Real());
    FieldDef<FieldType::Real> fldMult_fuelcons_when_inactive = makeFieldDef("Mult_fuelcons_when_inactive",FieldType::Real());
    FieldDef<FieldType::Integer> fldFirm_ids = makeFieldDef("Firm",FieldType::Integer());

public:
    VesselDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    void dropAndCreate();

    void feedVesselsDefTable(const std::vector<std::string> &vesselids,
                             const std::vector<double> &speeds,
                             const std::vector<double> &length);    // TODO fill all the rest

    void createIndex();
protected:
};

#endif // VESSELDEFTABLE_H
