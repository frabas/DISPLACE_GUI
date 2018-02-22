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
    struct Impl;
    std::unique_ptr<Impl> p;

public:
    const FieldDef<FieldType::Integer> fldId = makeFieldDef("VesselId", FieldType::Integer()).primaryKey();
    const FieldDef<FieldType::Text> fldName = makeFieldDef("VesselName",FieldType::Text()).unique().notNull();
    const FieldDef<FieldType::Text> fldNationality = makeFieldDef("Nationality",FieldType::Text());
    const FieldDef<FieldType::Real> fldSpeeds = makeFieldDef("IdSpeeds",FieldType::Real());
    const FieldDef<FieldType::Real> fldFuelcons = makeFieldDef("Fuelcons",FieldType::Real());
    const FieldDef<FieldType::Real> fldLengths = makeFieldDef("Lengths",FieldType::Real());
    const FieldDef<FieldType::Real> fldVKWs = makeFieldDef("VKWs",FieldType::Real());
    const FieldDef<FieldType::Real> fldCarrycapacities = makeFieldDef("Carrycapacities",FieldType::Real());
    const FieldDef<FieldType::Real> fldTankcapacities = makeFieldDef("Tankcapacities",FieldType::Real());
    const FieldDef<FieldType::Real> fldNbfpingspertrips = makeFieldDef("Nbfpingspertrips",FieldType::Real());
    const FieldDef<FieldType::Real> fldResttime_par1s = makeFieldDef("Resttime_par1s",FieldType::Real());
    const FieldDef<FieldType::Real> fldResttime_par2s = makeFieldDef("Resttime_par2s",FieldType::Real());
    const FieldDef<FieldType::Real> fldAv_trip_duration = makeFieldDef("Av_trip_duration",FieldType::Real());
    const FieldDef<FieldType::Real> fldMult_fuelcons_when_steaming = makeFieldDef("Mult_fuelcons_when_steaming",FieldType::Real());
    const FieldDef<FieldType::Real> fldMult_fuelcons_when_fishing = makeFieldDef("Mult_fuelcons_when_fishing",FieldType::Real());
    const FieldDef<FieldType::Real> fldMult_fuelcons_when_returning = makeFieldDef("Mult_fuelcons_when_returning",FieldType::Real());
    const FieldDef<FieldType::Real> fldMult_fuelcons_when_inactive = makeFieldDef("Mult_fuelcons_when_inactive",FieldType::Real());
    const FieldDef<FieldType::Integer> fldFirm_ids = makeFieldDef("Firm",FieldType::Integer());

    VesselDefTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    ~VesselDefTable() noexcept;
    void dropAndCreate();

    void feedVesselsDefTable(const std::vector<std::string> &vesselids,
                             const std::vector<double> &speeds,
                             const std::vector<double> &length);    // TODO fill all the rest

    std::vector<std::string> getNationsList();

    void createIndex();
protected:
};

#endif // VESSELDEFTABLE_H
