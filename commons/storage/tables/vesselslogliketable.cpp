#include "vesselslogliketable.h"


struct VesselsLoglikeTable::Impl {
    std::mutex mutex;
    bool init = false;

    PreparedInsert<
    FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>,
    FieldDef<FieldType::Integer>,
    FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
    FieldDef<FieldType::Real>,FieldDef<FieldType::Real>,
    FieldDef<FieldType::Integer>
    > insertStatement;
};


VesselsLoglikeTable::VesselsLoglikeTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name), p(std::make_unique<Impl>())
{
}

VesselsLoglikeTable::~VesselsLoglikeTable() noexcept = default;

void VesselsLoglikeTable::dropAndCreate()
{
    if (db()->tableExists(name()))
        db()->dropTable(name());

    create(std::make_tuple(
               fldRowId,
               fldId,
               fldTStep,
               fldTStepDep,
               fldNodeId,
               fldMetierId,
               fldLastHarbour,
               fldRevenueAV,
               revenueExAV,
               timeAtSea,
               reasonToGoBack,
               cumFuelCons,
               vpuf,
               fuelCost,
               gav,
               sweptArea,
               revenuePerSweptArea,
               GVA,
               GVAPerRevenue,
               LabourSurplus,
               GrossProfit,
               NetProfit,
               NetProfitMargin,
               RoFTA,
               GVAPerFTE,
               BER,
               CRBER,
               NetPresentValue,
               numTrips
    ));
}

size_t VesselsLoglikeTable::insertLog(const VesselsLoglikeTable::Log &log)
{
    std::unique_lock<std::mutex> l(p->mutex);
    if (!p->init) {
        p->init = true;
        p->insertStatement = prepareInsert(std::make_tuple(fldId,
                                                           fldTStep,
                                                           fldTStepDep,
                                                           fldNodeId,
                                                           fldMetierId,
                                                           fldLastHarbour,
                                                           fldRevenueAV,
                                                           revenueExAV,
                                                           timeAtSea,
                                                           reasonToGoBack,
                                                           cumFuelCons,
                                                           vpuf,
                                                           fuelCost,
                                                           gav,
                                                           sweptArea,
                                                           revenuePerSweptArea,
                                                           GVA,
                                                           GVAPerRevenue,
                                                           LabourSurplus,
                                                           GrossProfit,
                                                           NetProfit,
                                                           NetProfitMargin,
                                                           RoFTA,
                                                           GVAPerFTE,
                                                           BER,
                                                           CRBER,
                                                           NetPresentValue,
                                                           numTrips));
    }


    return SQLiteTable::insertAndGetRowId(p->insertStatement,
                                          std::make_tuple(
                log.id,
                log.tstep,
                log.tstepdep,
                (int)log.node_id.toIndex(),
                log.metierId,
                log.lastHarbour,
                log.revenueAV,
                log.revenueExAV,
                log.timeAtSea,
                log.reasonToGoBack,
                log.cumFuelCons,
                log.vpuf,
                log.fuelCost,
                log.gav,
                log.sweptArea,
                log.revenuePerSweptArea,
                log.GVA,
                log.GVAPerRevenue,
                log.LabourSurplus,
                log.GrossProfit,
                log.NetProfit,
                log.NetProfitMargin,
                log.RoFTA,
                log.GVAPerFTE,
                log.BER,
                log.CRBER,
                log.NetPresentValue,
                log.numTrips
                ));
}

