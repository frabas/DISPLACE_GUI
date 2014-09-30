#ifndef DBHELPER_H
#define DBHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QThread>

QT_BEGIN_NAMESPACE
class QSqlQuery;
QT_END_NAMESPACE

class Node;
class Vessel;
class DbHelper;

class VesselPositionInserter : public QObject {
    Q_OBJECT

    friend class DbHelper;

    DbHelper *mHelper;
    QSqlDatabase mDb;
    QSqlQuery *mVesselInsertionQuery;

    int mLastStep;
public:
    explicit VesselPositionInserter(DbHelper *helper, QSqlDatabase *db);

public slots:
    void addVesselPosition (int step, int idx , double x, double y, double fuel, int state);
};

class DbHelper : public QObject
{
    Q_OBJECT

    friend class VesselPositionInserter;

    QSqlDatabase mDb;
public:
    DbHelper();

    bool attachDb(QString file);

    void addNodesDetails(int idx, Node *node);
    void removeAllNodesDetails();

    void addVesselPosition (int step, int idx, Vessel *vessel);
    void removeAllVesselsDetails();
    void addVesselDetails (int idx, Vessel *vessel);

    bool loadNodes(QList<Node *> &nodes);
    bool loadVessels(const QList<Node *> &nodes, QList<Vessel *> &vessels);

    void beginTransaction();
    void endTransaction();

    void setMetadata (QString key, QString value);
    QString getMetadata (QString key);

signals:
    void postVesselInsertion (int step, int idx , double x, double y, double fuel, int state);

protected:
    bool checkMetadataTable();
    bool checkNodesTable();
    bool checkVesselsTable();
    bool checkVesselsPosTable();

private:
    bool mOngoingTransaction;

    VesselPositionInserter *mInserter;
    QThread *mInsertThread;

    static const QString TBL_META;
    static const QString TBL_NODES;
    static const QString TBL_VESSELS;
    static const QString TBL_VESSELS_POS;
};

#endif // DBHELPER_H
