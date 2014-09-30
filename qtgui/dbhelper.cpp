#include "dbhelper.h"

#include <Node.h>
#include <Vessel.h>

#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

const int DbHelper::VERSION = 1;

const QString DbHelper::TBL_META = "Metadata";
const QString DbHelper::TBL_NODES = "Nodes";
const QString DbHelper::TBL_VESSELS = "VesselsNames";
const QString DbHelper::TBL_VESSELS_POS = "VesselsPos";

const QString DbHelper::META_VERSION = "version";

#define DB_ASSERT(x,q) Q_ASSERT_X((x), __FUNCTION__, (q).lastError().text().toStdString().c_str());


DbHelper::DbHelper()
    : mDb(),
      mOngoingTransaction(false),
      mVersion(-1)
{
    mInsertThread = new QThread();
}

bool DbHelper::attachDb(QString file)
{
    mDb = QSqlDatabase::addDatabase("QSQLITE");
    mDb.setDatabaseName(file);

    if (!mDb.open()) {
        return false;
    }

    // prepare helpers query
    // check tables
    checkMetadataTable();
    checkNodesTable(mVersion);
    checkVesselsPosTable(mVersion);
    checkVesselsTable(mVersion);

    /* update ended here */
    mVersion = VERSION;

    mInserter = new VesselPositionInserter(this, &mDb);

    connect (this, SIGNAL(postVesselInsertion(int,int,double,double,double,double,int)), mInserter, SLOT(addVesselPosition(int,int,double,double,double,double,int)));
    connect (this, SIGNAL(flush()), mInserter, SLOT(flush()));

    mInserter->moveToThread(mInsertThread);
    mInsertThread->start();

    return true;
}

void DbHelper::addVesselPosition(int step, int idx, Vessel *vessel)
{
    emit postVesselInsertion(step, idx, vessel->get_x(), vessel->get_y(), vessel->get_course(), vessel->get_fuelcons(), vessel->get_state());
}

void DbHelper::removeAllNodesDetails()
{
    QSqlQuery q;
    bool res = q.exec("DELETE FROM " + TBL_NODES);
    DB_ASSERT(res,q);
}

void DbHelper::removeAllVesselsDetails()
{
    QSqlQuery q;
    bool res = q.exec("DELETE FROM " + TBL_VESSELS);
    DB_ASSERT(res,q);
}

void DbHelper::addNodesDetails(int idx, Node *node)
{
    bool res;
    QSqlQuery q;

    res = q.prepare("INSERT INTO " + TBL_NODES
                + "(_id,x,y,harbour,areacode,landscape) "
                + "VALUES (?,?,?,?,?,?)");

    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());

    q.addBindValue(idx);
    q.addBindValue(node->get_x());
    q.addBindValue(node->get_y());
    q.addBindValue(node->get_harbour());
    q.addBindValue(node->get_code_area());
    q.addBindValue(node->get_marine_landscape());

    res = q.exec();
    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());
}

void DbHelper::addVesselDetails(int idx, Vessel *vessel)
{
    bool res;

    QSqlQuery q;
    res = q.prepare("INSERT INTO " +TBL_VESSELS
                    + "(_id,name,node) VALUES(?,?,?)");

    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());

    q.addBindValue(idx);
    q.addBindValue(QString::fromUtf8(vessel->get_name().c_str()));
    q.addBindValue(vessel->get_loc()->get_idx_node());
    res = q.exec();
    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());
}

bool DbHelper::loadNodes(QList<Node *> &nodes)
{
    QSqlQuery q("SELECT _id,x,y,harbour,areacode,landscape,nbpops,szgroup FROM " + TBL_NODES + " ORDER BY _id");
    if (!q.exec()) {
        return false;
    }

    while (q.next()) {
        int idx = q.value(0).toInt();
        double x = q.value(1).toDouble();
        double y = q.value(2).toDouble();
        int harbour = q.value(3).toInt();
        int areacode = q.value(4).toInt();
        int landscape = q.value(5).toInt();

        int nbpops = 0; // not used!
        int szgroup = 0;

        Node *n = new Node(idx, x, y, harbour, areacode, landscape, nbpops, szgroup);

        while (nodes.size() < idx+1)
            nodes.push_back(0);

        nodes[idx] = n;
    }

    return true;
}

bool DbHelper::loadVessels(const QList<Node *> &nodes, QList<Vessel *> &vessels)
{
    QSqlQuery q("SELECT _id,name,node FROM " + TBL_VESSELS + " ORDER BY _id");
    if (!q.exec()) {
        return false;
    }

    while (q.next()) {
        int idx = q.value(0).toInt();
        int nidx = q.value(2).toInt();

        Vessel *v = new Vessel(nodes.at(nidx), idx, q.value(1).toString().toStdString());

        while (vessels.size() < idx+1)
            vessels.push_back(0);

        vessels[idx] = v;
    }

    return true;
}

bool DbHelper::updateVesselsToStep(int steps, QList<Vessel *> &vessels)
{
    QSqlQuery q;
    q.prepare("SELECT vesselid,x,y,fuel,state,cumcatches,timeatsea,reason_to_go_back,course FROM " + TBL_VESSELS_POS
              + " WHERE tstep=?");
    q.addBindValue(steps);

    q.exec();

    while (q.next()) {
        int idx = q.value(0).toInt();
        double x = q.value(1).toDouble();
        double y = q.value(2).toDouble();
        double fuel = q.value(3).toInt();
        int state = q.value(4).toDouble();
        double cum = q.value(5).toDouble();
        double tim = q.value(6).toDouble();
        int r = q.value(7).toInt();
        double course = q.value(8).toDouble();

        Vessel *v = vessels.at(idx);
        v->set_xy(x,y);
        v->set_fuelcons(fuel);
        v->set_state(state);
        v->set_cumcatches(cum);
        v->set_timeatsea(tim);
        v->set_reason_to_go_back(r);
        v->set_course(course);
    }
    return true;
}

void DbHelper::beginTransaction()
{
    mDb.transaction();
    mOngoingTransaction = true;
}

void DbHelper::endTransaction()
{
    mDb.commit();
    mOngoingTransaction = false;
}

void DbHelper::flushBuffers()
{
    emit flush();
}

void DbHelper::setMetadata(QString key, QString value)
{
    QSqlQuery q;
    q.prepare("INSERT OR REPLACE INTO " +TBL_META
              + "(key,value) VALUES(?,?)");

    q.addBindValue(key);
    q.addBindValue(value);
    bool res = q.exec();
    DB_ASSERT(res,q);
}

QString DbHelper::getMetadata(QString key)
{
    QSqlQuery q;
    q.prepare("SELECT value FROM " + TBL_META + " WHERE key=?");
    q.addBindValue(key);
    q.exec();
    if (q.next())
        return q.value(0).toString();
    return QString();
}

int DbHelper::getLastKnownStep()
{
    QSqlQuery q;
    q.exec("SELECT MAX(tstep) FROM " + TBL_VESSELS_POS);
    if (q.next())
        return q.value(0).toInt();
    return -1;
}

bool DbHelper::checkMetadataTable()
{
    if (!mDb.tables().contains(TBL_META)) {
        QSqlQuery q;
        bool r =
        q.exec("CREATE TABLE " + TBL_META + "("
               + "key VARCHAR(16) PRIMARY KEY,"
               + "value VARCHAR(16)"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    } else {
        bool ok;
        mVersion = getMetadata(META_VERSION).toInt(&ok);
        if (!ok)
            mVersion = 0;
    }

    setMetadata(META_VERSION, QString("%1").arg(VERSION));

    return true;
}

bool DbHelper::checkNodesTable(int version)
{
    if (!mDb.tables().contains(TBL_NODES)) {
        QSqlQuery q;
        bool r =
        q.exec("CREATE TABLE " + TBL_NODES + "("
               + "_id INTEGER PRIMARY KEY,"
               + "x REAL,"
               + "y REAL,"
               + "harbour INTEGER,"
               + "areacode INTEGER,"
               + "landscape INTEGER,"
               + "nbpops INTEGER,"
               + "szgroup INTEGER"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (version < 2) {

    }

    return true;
}

bool DbHelper::checkVesselsPosTable(int version)
{
    if (!mDb.tables().contains(TBL_VESSELS_POS)) {
        QSqlQuery q;
        bool r =
        q.exec("CREATE TABLE " + TBL_VESSELS_POS + "("
               + "_id INTEGER AUTO_INCREMENT PRIMARY KEY,"
               + "vesselid INTEGER,"
               + "tstep INTEGER,"
               + "x REAL,"
               + "y REAL,"
               + "course REAL,"
               + "fuel REAL,"
               + "state INTEGER,"
               + "cumcatches REAL,"
               + "timeatsea REAL,"
               + "reason_to_go_back INTEGER"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (version < 2) {

    }

    return true;
}

bool DbHelper::checkVesselsTable(int version)
{
    bool r;
    if (!mDb.tables().contains(TBL_VESSELS)) {
        QSqlQuery q;
        r =
        q.exec("CREATE TABLE " + TBL_VESSELS + "("
               + "_id INTEGER PRIMARY KEY,"
               + "name VARCHAR(16),"
               + "node INTEGER"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (version < 2) {

    }

    return true;
}


VesselPositionInserter::VesselPositionInserter(DbHelper *helper, QSqlDatabase *db)
    : QObject(),
      mHelper(helper),
      mDb(QSqlDatabase::cloneDatabase(*db, "inserter")),
      mVesselInsertionQuery(0),
      mFlushCount(10000),
      mCounter(0),
      mLastStep(-1)
{
    mVesselInsertionQuery = new QSqlQuery;
    mVesselInsertionQuery->prepare(
                "INSERT INTO " + DbHelper::TBL_VESSELS_POS
                + "(vesselid,tstep,x,y,course,fuel,state) VALUES (?,?,?,?,?,?,?)"
                );

    mDb.open();
}

void VesselPositionInserter::addVesselPosition(int step, int idx, double x, double y, double course,double fuel, int state)
{
    /* if it's a new step, commits the insertion and restart */
    ++mCounter;

    if (mCounter >= mFlushCount) {
        mHelper->endTransaction();
        mHelper->beginTransaction();
        mCounter = 0;
    }

    /* if there's no transaction ongoing, start it */
    if (!mHelper->mOngoingTransaction) {
        mHelper->beginTransaction();
    }

    mLastStep = step;

    mVesselInsertionQuery->addBindValue(idx);
    mVesselInsertionQuery->addBindValue(step);
    mVesselInsertionQuery->addBindValue(x);
    mVesselInsertionQuery->addBindValue(y);
    mVesselInsertionQuery->addBindValue(course);
    mVesselInsertionQuery->addBindValue(fuel);
    mVesselInsertionQuery->addBindValue(state);

    if (!mVesselInsertionQuery->exec())
        qDebug() << mVesselInsertionQuery->lastError();
}

void VesselPositionInserter::flush()
{
    mHelper->endTransaction();
    mCounter = 0;
}
