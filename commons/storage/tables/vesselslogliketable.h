#ifndef VESSELSLOGLIKETABLE_H
#define VESSELSLOGLIKETABLE_H

#include "commons_global.h"
#include "idtypes.h"

#include "sqlitefielddef.h"
#include "sqlitetable.h"
using namespace sqlite;

#include <vector>
#include <string>

class COMMONSSHARED_EXPORT VesselsLoglikeTable : public SQLiteTable
{
public:
    const FieldDef<FieldType::Integer> & fldRowId = makeFieldDef("RowId", FieldType::Integer()).primaryKey();
    const FieldDef<FieldType::Integer> & fldId = makeFieldDef("Id", FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> & fldTStep = makeFieldDef("TStep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> & fldTStepDep = makeFieldDef("TStepDep",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> & fldNodeId = makeFieldDef("NodeId",FieldType::Integer()).notNull();

    const FieldDef<FieldType::Integer> & fldMetierId = makeFieldDef("metierId",FieldType::Integer()).notNull();
    const FieldDef<FieldType::Integer> & fldLastHarbour = makeFieldDef("lastHarbour",FieldType::Integer()).notNull();

    const FieldDef<FieldType::Real> & fldRevenueAV = makeFieldDef("revenueAV",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & revenueExAV = makeFieldDef("revenueExAV",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & timeAtSea = makeFieldDef("timeAtSea",FieldType::Real()).notNull();

    const FieldDef<FieldType::Integer> & reasonToGoBack = makeFieldDef("reasonToGoBack",FieldType::Integer()).notNull();

    const FieldDef<FieldType::Real> & cumFuelCons = makeFieldDef("cumFuelCons",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & vpuf = makeFieldDef("vpuf",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & fuelCost = makeFieldDef("fuelCost",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & gav = makeFieldDef("gav",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & sweptArea = makeFieldDef("sweptArea",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & revenuePerSweptArea = makeFieldDef("revenuePerSweptArea",FieldType::Real()).notNull();

    const FieldDef<FieldType::Real> & GVA = makeFieldDef("GVA",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & GVAPerRevenue = makeFieldDef("GVAPerRevenue",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & LabourSurplus = makeFieldDef("LabourSurplus",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & GrossProfit = makeFieldDef("GrossProfit",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & NetProfit = makeFieldDef("NetProfit",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & NetProfitMargin = makeFieldDef("NetProfitMargin",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & RoFTA = makeFieldDef("RoFTA",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & GVAPerFTE = makeFieldDef("GVAPerFTE",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & BER = makeFieldDef("BER",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & CRBER = makeFieldDef("CRBER",FieldType::Real()).notNull();
    const FieldDef<FieldType::Real> & NetPresentValue = makeFieldDef("NetPresentValue",FieldType::Real()).notNull();

    const FieldDef<FieldType::Integer> & numTrips = makeFieldDef("numTrips",FieldType::Integer()).notNull();

    VesselsLoglikeTable(std::shared_ptr<sqlite::SQLiteStorage> db, std::string name);
    void dropAndCreate();

    struct Log {
        int id;
        int tstep, tstepdep;
        types::NodeId node_id;
        int metierId = -1;
        int lastHarbour = -1;
        double revenueAV = 0;
        double revenueExAV = 0;
        double timeAtSea = 0;
        int reasonToGoBack = 0;
        double cumFuelCons = 0;
        double vpuf = 0;
        double fuelCost = 0;
        double gav = 0;
        double sweptArea = 0;
        double revenuePerSweptArea = 0;
        double GVA = 0;
        double GVAPerRevenue = 0;
        double LabourSurplus = 0;
        double GrossProfit = 0;
        double NetProfit = 0;
        double NetProfitMargin = 0;
        double RoFTA = 0;
        double GVAPerFTE = 0;
        double BER = 0;
        double CRBER = 0;
        double NetPresentValue = 0;
        int numTrips=0;
    };

    size_t insertLog(const Log &log);
};

#endif // VESSELSLOGLIKETABLE_H
