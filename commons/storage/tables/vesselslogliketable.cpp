#include "vesselslogliketable.h"

VesselsLoglikeTable::VesselsLoglikeTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name)
{
}

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

void VesselsLoglikeTable::insertLog(const VesselsLoglikeTable::Log &log)
{
    SQLiteTable::insert(
                fldId.assign(log.id),
                fldTStep.assign(log.tstep),
                fldTStepDep.assign(log.tstepdep),
                fldNodeId.assign(log.node_id.toIndex()),
                fldMetierId.assign(log.metierId),
                fldLastHarbour.assign(log.lastHarbour),
                fldRevenueAV.assign(log.revenueAV),
                revenueExAV.assign(log.revenueExAV),
                timeAtSea.assign(log.timeAtSea),
                reasonToGoBack.assign(log.reasonToGoBack),
                cumFuelCons.assign(log.cumFuelCons),
                vpuf.assign(log.vpuf),
                fuelCost.assign(log.fuelCost),
                gav.assign(log.gav),
                sweptArea.assign(log.sweptArea),
                revenuePerSweptArea.assign(log.revenuePerSweptArea),
                GVA.assign(log.GVA),
                GVAPerRevenue.assign(log.GVAPerRevenue),
                LabourSurplus.assign(log.LabourSurplus),
                GrossProfit.assign(log.GrossProfit),
                NetProfit.assign(log.NetProfit),
                NetProfitMargin.assign(log.NetProfitMargin),
                RoFTA.assign(log.RoFTA),
                GVAPerFTE.assign(log.GVAPerFTE),
                BER.assign(log.BER),
                CRBER.assign(log.CRBER),
                NetPresentValue.assign(log.NetPresentValue),
                numTrips.assign(log.numTrips)
                );
}

