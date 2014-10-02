#ifndef DBHELPER_H
#define DBHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QThread>

QT_BEGIN_NAMESPACE
class QSqlQuery;
QT_END_NAMESPACE

class DisplaceModel;
class NodeData;
class VesselData;
class DbHelper;
class Config;
class Scenario;

class VesselPositionInserter : public QObject {
    Q_OBJECT

    friend class DbHelper;

    DbHelper *mHelper;
    QSqlDatabase mDb;
    QSqlQuery *mVesselInsertionQuery;

    int mFlushCount;
    int mCounter;
    int mLastStep;
public:
    explicit VesselPositionInserter(DbHelper *helper, QSqlDatabase *db);

public slots:
    void addVesselPosition (int step, int idx , double x, double y, double course, double fuel, int state);
    void flush();
};

class DbHelper : public QObject
{
    Q_OBJECT

    friend class VesselPositionInserter;

    QSqlDatabase mDb;
public:
    DbHelper();

    bool attachDb(QString file);

    void addNodesDetails(int idx, NodeData *node);
    void removeAllNodesDetails();

    void addNodesStats (int tstep, const QList<NodeData *> &nodes);

    void addVesselPosition (int step, int idx, VesselData *vessel);
    void removeAllVesselsDetails();
    void addVesselDetails (int idx, VesselData *vessel);

    bool loadConfig(Config &);
    bool saveConfig (const Config &);
    bool loadScenario (Scenario &);
    bool saveScenario (const Scenario &);

    bool loadNodes(QList<NodeData *> &nodes, DisplaceModel *model);
    bool loadVessels(const QList<NodeData *> &nodes, QList<VesselData *> &vessels);
    bool updateVesselsToStep(int steps, QList<VesselData *> &vessels);
    bool updateStatsForNodesToStep(int step, QList<NodeData *> &nodes);

    void beginTransaction();
    void endTransaction();
    void flushBuffers();

    void setMetadata (QString key, QString value);
    QString getMetadata (QString key);

    int getLastKnownStep();
signals:
    void postVesselInsertion (int step, int idx , double x, double y, double course, double fuel, int state);
    void flush();

protected:
    bool checkMetadataTable();
    bool checkNodesTable(int version);
    bool checkNodesStats(int version);
    bool checkVesselsTable(int version);
    bool checkVesselsPosTable(int version);

private:
    bool mOngoingTransaction;

    VesselPositionInserter *mInserter;
    QThread *mInsertThread;
    int mVersion;

    static const int VERSION;

    static const QString TBL_META;
    static const QString TBL_NODES;
    static const QString TBL_NODES_STATS;
    static const QString TBL_POPNODES_STATS;
    static const QString TBL_VESSELS;
    static const QString TBL_VESSELS_POS;

    static const QString META_VERSION;
};

#endif // DBHELPER_H
