#ifndef PLOTTYPES_H
#define PLOTTYPES_H

namespace displace {
namespace plot {

enum class PopulationStat { Aggregate, Mortality, SSB };

enum class BenthosStat { B_TotBiomass, B_Number, B_MeanWeight, B_TotBiomassOverK, B_NumberOverK};
enum class FishfarmsStat { FF_FishMeanWeight, FF_FishHarvestedKg, FF_EggsHarvestedKg, FF_AnnualProfit,
                           FF_NetDischargeN, FF_NetDischargeP, FF_CumulNetDischargeN, FF_CumulNetDischargeP};
enum class WindfarmsStat { WF_kWh, WF_kWProduction};
enum class ShipsStat { SH_NbTransportedUnits, SH_FuelPerHour, SH_NOxEmission_gperkW, SH_SOxEmission_PercentPerFuelMass,
                       SH_GHGEmission_gperkW, SH_PMEEmission_gperkW, SH_FuelUseLitre, SH_NOxEmission, SH_SOxEmission, SH_GHGEmission, SH_PMEEmission};

enum class NationsStat { Catches, Discards, Earnings, ExEarnings,TimeAtSea, Gav, Vpuf, SweptArea, RevenuePerSweptArea,  GVA, GVAPerRevenue,
                   LabourSurplus, GrossProfit,NetProfit, NetProfitMargin, GVAPerFTE, RoFTA, BER, CRBER, NetPresentValue, numTrips

};

}
}

#endif // PLOTTYPES_H
