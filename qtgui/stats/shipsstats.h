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
            nb_transported_units = 0.0;
            fuel_use_h = 0.0;
            NOx_emission_gperkW = 0.0;
            SOx_emission_percentpertotalfuelmass = 0.0;
            GHG_emission_gperkW = 0.0;
            PME_emission_gperkW = 0.0;
            fuel_use_litre = 0.0;
            NOx_emission = 0.0;
            SOx_emission = 0.0;
            GHG_emission = 0.0;
            PME_emission = 0.0;
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


   double NbTransportedUnitsForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double FuelPerHourForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double NOxEmissionFactorForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double SOxEmissionFactorForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double GHGEmissionFactorForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double PMEEmissionFactorForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double FuelUseLitreForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double NOxEmissionForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double SOxEmissionForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double GHGEmissionForShipAndShipGroup(int shipid,  int shiptypeid) const;
   double PMEEmissionForShipAndShipGroup(int shipid,  int shiptypeid) const;


    static const ShipsStats::StatData NoData;
};



#endif // SHIPSSTATS_H



