#ifndef WINDFARMSSTATS_H
#define WINDFARMSSTATS_H


#include <QVector>
#include <mutex>

class WindfarmsStats
{
public:
    struct StatData {
        double kWh = 0.0;
        double kWproduction = 0.0;

        StatData() {}
        void clear() {
            double kWh = 0.0;
            double kWproduction = 0.0;
        }
    };

private:
    bool mDirty = false;
    QVector<QVector<StatData>> mDataPerWindFarmTypeAndWindFarmId;
    mutable std::mutex mMutex;

    StatData &get(int windfarmid, int windfarmtype);
    const StatData &get(int windfarmid, int windfarmtype) const;

public:
    WindfarmsStats();
    ~WindfarmsStats() noexcept = default;

    WindfarmsStats(const WindfarmsStats& b);
    WindfarmsStats(WindfarmsStats &&b);

    WindfarmsStats &operator = (const WindfarmsStats &b);
    WindfarmsStats &operator = (WindfarmsStats &&b);

    bool dirty() const { return mDirty; }
    void setDirty(bool dirty = true) { mDirty = dirty; }
    void clear();

    void collectkWh(int step, int windfarmid, int windfarmtypeid, double kWh);
    void collectkWproduction(int step, int windfarmid, int windfarmtypeid, double kWproduction);

    double kWhForWindfarmAndWindfarmGroup(int windfarmid, int windfarmtypeid) const;
    double kWProductionForWindfarmAndWindfarmGroup(int windfarmid, int windfarmtypeid) const;

    static const WindfarmsStats::StatData NoData;
};







#endif // WINDFARMSSTATS_H
