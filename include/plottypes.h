#ifndef PLOTTYPES_H
#define PLOTTYPES_H

namespace displace {
namespace plot {

enum class PopulationStat { Aggregate, Catches, Discards, Mortality, RavFMortality, NatMortality, NumberAtAge, WeightAtAge, MaturityAtAge,
                            SSB, QuotasUptake, Quotas, Choking, FFmsy, PropMature, FvsEffort };

enum class BenthosStat { B_TotBiomass, B_Number, B_MeanWeight, B_TotBiomassOverK, B_NumberOverK};
enum class FishfarmsStat { FF_FishMeanWeight, FF_FishHarvestedKg, FF_EggsHarvestedKg, FF_AnnualProfit,
                           FF_NetDischargeN, FF_NetDischargeP, FF_NetDischargeC, FF_CumulNetDischargeN, FF_CumulNetDischargeP, FF_CumulNetDischargeC};
enum class WindfarmsStat { WF_kWh, WF_kWProduction};
enum class ShipsStat { SH_NbTransportedUnits, SH_FuelPerHour, SH_NOxEmission_gperkW, SH_SOxEmission_PercentPerFuelMass,
                       SH_GHGEmission_gperkW, SH_PMEEmission_gperkW, SH_FuelUseLitre, SH_NOxEmission, SH_SOxEmission, SH_GHGEmission, SH_PMEEmission};

enum class NationsStat { Catches, Discards, Earnings, ExEarnings,TimeAtSea, Gav, Vpuf, SweptArea, RevenuePerSweptArea,  GVA, GVAPerRevenue,
                   LabourSurplus, GrossProfit,NetProfit, NetProfitMargin, GVAPerFTE, RoFTA, BER, CRBER, NetPresentValue, numTrips };

enum class VesselsStat { Catches, Discards, Earnings, ExEarnings,TimeAtSea, Gav, Vpuf, SweptArea, RevenuePerSweptArea,  GVA, GVAPerRevenue,
                   LabourSurplus, GrossProfit,NetProfit, NetProfitMargin, GVAPerFTE, RoFTA, BER, CRBER, NetPresentValue, numTrips };

enum class HarboursStat { H_Catches, H_Discards, H_Earnings, H_Gav, H_Vpuf, H_SweptArea, H_RevenuePerSweptArea,
                 H_GVA, H_GVAPerRevenue, H_LabourSurplus, H_GrossProfit, H_NetProfit, H_NetProfitMargin, H_GVAPerFTE, H_RoFTA, H_BER, H_CRBER, H_NetPresentValue, H_numTrips };

enum class MetiersStat { M_Catches, M_Discards, M_Revenues, M_Gav, M_Vpuf, M_SweptArea, M_RevenuesPerSweptArea,
                   M_GVA, M_GVAPerRevenue, M_LabourSurplus, M_GrossProfit, M_NetProfit, M_NetProfitMargin, M_GVAPerFTE, M_RoFTA, M_BER, M_CRBER, M_NetPresentValue, M_numTrips};

enum class AggregationType { None, Min, Max, Avg, Sum };

}
}

#endif // PLOTTYPES_H
