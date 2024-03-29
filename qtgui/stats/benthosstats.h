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
        double meanweight = 0.0;
        double biomassOverK = 0.0;
        double numberOverK = 0.0;

        StatData() {}
        void clear() {
            biomass = 0.0;
            number = 0.0;
            meanweight = 0.0;
            biomassOverK = 0.0;
            numberOverK = 0.0;
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
    void collectNumber (int step, int funcid, int benthosid, double number);
    void collectMeanWeight(int step, int funcid, int benthosid, double number);
    void collectBiomassOverK(int step, int funcid, int benthosid, double biomass);
    void collectNumberOverK(int step, int funcid, int benthosid, double number);

    double biomassForBenthosAndFuncGroup(int funcgroup, int benthos) const;
    double numberForBenthosAndFuncGroup(int funcgroup, int benthos) const;
    double meanweightForBenthosAndFuncGroup(int funcgroup, int benthos) const;
    double biomassOverKForBenthosAndFuncGroup(int funcgroup, int benthos) const;
    double numberOverKForBenthosAndFuncGroup(int funcgroup, int benthos) const;

    static const BenthosStats::StatData NoData;
};

#endif // BENTHOSSTATS_H
