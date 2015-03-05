#ifndef POPULATIONDISTRIBUTIONDATAMERGERSTRATEGY_H
#define POPULATIONDISTRIBUTIONDATAMERGERSTRATEGY_H

#include <workers/datamerger.h>

namespace displace {
namespace workers {

class PopulationDistributionDataMergerStrategy : public DataMerger::Strategy
{
public:
    PopulationDistributionDataMergerStrategy(DisplaceModel *model);

    void attach(DataMerger *merger);
    bool processHeaderField(QString field, int i) override;
    bool postHeaderProcessed() override;
    void processLine (int linenum, QString line) override;
    bool saveOutput(QString out) override;

    static const char *const YearField;
    static const char *const SemesterField;
    static const char *const LatField;
    static const char *const LongField;
    static const char *const StockField;
    static const char *const IndivFieldPattern;
private:
    DataMerger *mOwner;
    DisplaceModel *mModel;

    QStringList mFields;
    QMutex mutex;

    int col_yr = -1, col_sem = -1, col_lat = -1, col_lon = -1, col_stock = -1, col_indiv = -1;
    int num_col_indiv = 0;

    QMap<QString, int> mStockNames;

    int getStockName(QString nm);

    /* Result will be put in a set for processing */

    typedef unsigned long ResultKey;
    struct Result {
    public:
        unsigned int node = 0;
        unsigned int year = 0;
        unsigned int semester = 0;
        unsigned int stock = 0;
        QVector<double> population;
        QVector<double> weights;
        bool centered = false;  // flags the node for centered in a grid node - w=u(i)

        explicit Result(int n)
            : population(n), weights(n) {
        }
    };

    QMap<ResultKey, Result> mResults;

    ResultKey genResultKey (const Result &result);
};


} } // ns

#endif // POPULATIONDISTRIBUTIONDATAMERGERSTRATEGY_H
