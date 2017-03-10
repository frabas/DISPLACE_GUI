#ifndef BENTHOSSTATS_H
#define BENTHOSSTATS_H

#include <QVector>
#include <mutex>

class BenthosStats
{
public:
    struct StatData {
        double biomass = 0.0;
        double number = 0.0;

        StatData() {}
        void clear() {
            biomass = 0.0;
            number = 0.0;
        }
    };

private:
    bool mDirty = false;
    QVector<QVector<StatData>> mDataPerBenthosAndFuncId;
    mutable std::mutex mMutex;

    StatData &get(int funcid, int benthos);
    const StatData &get(int funcid, int benthos) const;

public:
    BenthosStats();
    ~BenthosStats() noexcept = default;

    BenthosStats(const BenthosStats& b);
    BenthosStats(BenthosStats &&b);

    BenthosStats &operator = (const BenthosStats &b);
    BenthosStats &operator = (BenthosStats &&b);

    bool dirty() const { return mDirty; }
    void setDirty(bool dirty = true) { mDirty = dirty; }
    void clear();

    void collectBiomass(int step, int funcid, int benthosid, double biomass);
    void collectNumber (int step, int funcid, int node_idx, double number);

    double biomassForBenthosAndFuncGroup(int funcgroup, int benthos) const;
    double numberForBenthosAndFuncGroup(int funcgroup, int benthos) const;

    static const BenthosStats::StatData NoData;
};

#endif // BENTHOSSTATS_H
