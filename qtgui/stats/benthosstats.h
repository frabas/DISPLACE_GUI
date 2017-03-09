#ifndef BENTHOSSTATS_H
#define BENTHOSSTATS_H

#include <QVector>

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

    StatData &get(int funcid, int benthos);
    const StatData &get(int funcid, int benthos) const;

public:
    BenthosStats();

    bool dirty() const { return mDirty; }
    void setDirty(bool dirty = true) { mDirty = dirty; }
    void clear();

    void collectBiomass(int step, int funcid, int benthosid, double biomass);
    void collectNumber (int step, int funcid, int node_idx, double number);

    double biomassForBenthosAndFuncGroup(int benthos, int funcgroup) const;
    double numberForBenthosAndFuncGroup(int benthos, int funcgroup) const;
};

#endif // BENTHOSSTATS_H
