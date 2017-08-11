#ifndef FISHFARMSSTATS_H
#define FISHFARMSSTATS_H


#include <QVector>
#include <mutex>

class FishfarmsStats
{
public:
    struct StatData {
        double meanw = 0.0;
        double fishharvestkg = 0.0;
        double eggsharvestkg = 0.0;
        double annualprofit = 0.0;
        double netdischargeN = 0.0;
        double netdischargeP = 0.0;
        double cumulnetdischargeN = 0.0;
        double cumulnetdischargeP = 0.0;

        StatData() {}
        void clear() {
            meanw = 0.0;
            fishharvestkg = 0.0;
            eggsharvestkg = 0.0;
            annualprofit = 0.0;
            netdischargeN = 0.0;
            netdischargeN = 0.0;
            cumulnetdischargeP = 0.0;
            cumulnetdischargeP = 0.0;
        }
    };

private:
    bool mDirty = false;
    QVector<QVector<StatData>> mDataPerFarmTypeAndFarmId;
    mutable std::mutex mMutex;

    StatData &get(int farmid, int farmtype);
    const StatData &get(int farmid, int farmtype) const;

public:
    FishfarmsStats();
    ~FishfarmsStats() noexcept = default;

    FishfarmsStats(const FishfarmsStats& b);
    FishfarmsStats(FishfarmsStats &&b);

    FishfarmsStats &operator = (const FishfarmsStats &b);
    FishfarmsStats &operator = (FishfarmsStats &&b);

    bool dirty() const { return mDirty; }
    void setDirty(bool dirty = true) { mDirty = dirty; }
    void clear();

    void collectMeanWeightKg(int step, int farmid, int farmtypeid, double meanw_kg);
    void collectFishHarvestedKg (int step, int farmid, int farmtypeid, double fish_harvested_kg);
    void collectEggsHarvestedKg(int step, int farmid, int farmtypeid, double eggs_harvested_kg);
    void collectFishfarmAnnualProfit(int step, int farmid, int farmtypeid, double fishfarm_annual_profit);
    void collectNetDischargeN(int step, int farmid, int farmtypeid, double net_discharge_N);
    void collectNetDischargeP(int step, int farmid, int farmtypeid, double net_discharge_P);
    void collectCumulNetDischargeN(int step, int farmid, int farmtypeid, double cumul_net_discharge_N);
    void collectCumulNetDischargeP(int step, int farmid, int farmtypeid, double cumul_net_discharge_P);

    double meanwForFishfarmAndFarmGroup(int farmid, int farmtypeid) const;
    double fishharvestedkgForFishfarmAndFarmGroup(int farmid, int farmtypeid) const;
    double eggsharvestedkgForFishfarmAndFarmGroup(int farmid, int farmtypeid) const;
    double annualprofitForFishfarmAndFarmGroup(int farmid, int farmtypeid) const;
    double netdischargeNForFishfarmAndFarmGroup(int farmid, int farmtypeid) const;
    double netdischargePForFishfarmAndFarmGroup(int farmid, int farmtypeid) const;
    double cumulnetdischargeNForFishfarmAndFarmGroup(int farmid, int farmtypeid) const;
    double cumulnetdischargePForFishfarmAndFarmGroup(int farmid, int farmtypeid) const;

    static const FishfarmsStats::StatData NoData;
};


#endif // FISHFARMSSTATS_H
