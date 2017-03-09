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
    QVector<StatData> mDataPerFuncId;

public:
    BenthosStats();

    bool dirty() const { return mDirty; }
    void setDirty(bool dirty = true) { mDirty = dirty; }
    void clear();

    void collectBiomass(int step, int funcid, int node_idx, double biomass);
    void collectNumber (int step, int funcid, int node_idx, double number);
};

#endif // BENTHOSSTATS_H
