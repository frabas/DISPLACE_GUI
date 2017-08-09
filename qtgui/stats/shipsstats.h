#ifndef SHIPSSTATS_H
#define SHIPSSTATS_H


#include <QVector>
#include <mutex>

class ShipsStats
{
public:
    struct StatData {
        double nb_transported_units = 0.0;
        double fuel_use_h = 0.0;
        double NOx_emission_gperkW = 0.0;
        double SOx_emission_percentpertotalfuelmass = 0.0;
        double GHG_emission_gperkW = 0.0;
        double PME_emission_gperkW = 0.0;
        double fuel_use_litre = 0.0;
        double NOx_emission = 0.0;
        double SOx_emission = 0.0;
        double GHG_emission = 0.0;
        double PME_emission = 0.0;

        StatData() {}
        void clear() {
            double nb_transported_units = 0.0;
            double fuel_use_h = 0.0;
            double NOx_emission_gperkW = 0.0;
            double SOx_emission_percentpertotalfuelmass = 0.0;
            double GHG_emission_gperkW = 0.0;
            double PME_emission_gperkW = 0.0;
            double fuel_use_litre = 0.0;
            double NOx_emission = 0.0;
            double SOx_emission = 0.0;
            double GHG_emission = 0.0;
            double PME_emission = 0.0;
        }
    };

private:
    bool mDirty = false;
    QVector<QVector<StatData>> mDataPerShipTypeAndShipId;
    mutable std::mutex mMutex;

    StatData &get(int shipid, int shiptype);
    const StatData &get(int shipid, int shiptype) const;

public:
    ShipsStats();
    ~ShipsStats() noexcept = default;

    ShipsStats(const ShipsStats& b);
    ShipsStats(ShipsStats &&b);

    ShipsStats &operator = (const ShipsStats &b);
    ShipsStats &operator = (ShipsStats &&b);

    bool dirty() const { return mDirty; }
    void setDirty(bool dirty = true) { mDirty = dirty; }
    void clear();

    void collectNbTransportedUnits (int step, int shipid, int shiptypeid,  double nb_transported_units);
    void collectFuelUseHour (int step, int shipid,  int shiptypeid,  double fuel_use_h);
    void collectNOxEmissiongPerkW (int step, int shipid, int shiptypeid,  double NOx_emission_gperkW);
    void collectSOxEmissionPercentPerTotalFuelmass (int step, int shipid,  int shiptypeid,  double SOx_emission_percentpertotalfuelmass);
    void collectGHGemissiongPerkW (int step,  int shipid,  int shiptypeid,  double GHG_emission_gperkW);
    void collectPMEemissiongPerkW (int step,  int shipid,  int shiptypeid,  double PME_emission_gperkW);
    void collectfuelUseLitre (int step,  int shipid,  int shiptypeid,  double fuel_use_litre);
    void collectNOxEmission (int step,  int shipid,  int shiptypeid,  double NOx_emission);
    void collectSOxEmission (int step,  int shipid,  int shiptypeid,  double SOx_emission);
    void collectGHGemission (int step,  int shipid,  int shiptypeid,  double GHG_emission);
    void collectPMEemission (int step,  int shipid,  int shiptypeid,  double PME_emission);


   double NbTransportedUnitsForShipAndShipGroup(int shiptypeid, int shipid) const;
   double FuelPerHourForShipAndShipGroup(int shiptypeid, int shipid) const;
   double NOxEmissionFactorForShipAndShipGroup(int shiptypeid, int shipid) const;
   double SOxEmissionFactorForShipAndShipGroup(int shiptypeid, int shipid) const;
   double GHGEmissionFactorForShipAndShipGroup(int shiptypeid, int shipid) const;
   double PMEEmissionFactorForShipAndShipGroup(int shiptypeid, int shipid) const;
   double FuelUseLitreForShipAndShipGroup(int shiptypeid, int shipid) const;
   double NOxEmissionForShipAndShipGroup(int shiptypeid, int shipid) const;
   double SOxEmissionForShipAndShipGroup(int shiptypeid, int shipid) const;
   double GHGEmissionForShipAndShipGroup(int shiptypeid, int shipid) const;
   double PMEEmissionForShipAndShipGroup(int shiptypeid, int shipid) const;


    static const ShipsStats::StatData NoData;
};



#endif // SHIPSSTATS_H



