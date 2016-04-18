#ifndef MERGERSTRATEGY_H
#define MERGERSTRATEGY_H

#include <workers/datamerger.h>

namespace displace {
namespace workers {

class MergerStrategy : public DataMerger::Strategy
{
public:
    enum MergeType {
        Weights, Ping, PopulationDistribution
    };

    MergerStrategy (MergeType type);
    void attach(DataMerger *merger) override;
    bool processHeaderField(QString field, int i) override;
    bool postHeaderProcessed() override;
    void processLine (int linenum, QString line) override;
    bool saveOutput(QString out) override;

    static const char *const MergedField;
    static const char *const LatField;
    static const char *const LongField;

private:
    DataMerger *mOwner;
    MergeType mType;

    bool colpresent = true;
    int col_pt_graph = -1;
    int col_lat = -1;
    int col_lon = -1;

    QStringList mFields;
    QStringList mResults;
    QMutex mutex;
};

}}

#endif // MERGERSTRATEGY_H
