#include "dbhelper.h"

#include <config.h>
#include <scenario.h>

#include <modelobjects/nodedata.h>
#include <modelobjects/vesseldata.h>
#include <Harbour.h>

#include <displacemodel.h>

#include <profiler.h>

#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

const int DbHelper::VERSION = 1;

const QString DbHelper::TBL_META = "Metadata";
const QString DbHelper::TBL_NODES = "Nodes";
const QString DbHelper::TBL_NODES_STATS = "NodesStats";
const QString DbHelper::TBL_POPNODES_STATS = "PopNodesStats";
const QString DbHelper::TBL_POP_STATS = "popStats";
const QString DbHelper::TBL_VESSELS = "VesselsNames";
const QString DbHelper::TBL_VESSELS_POS = "VesselsPos";

const QString DbHelper::META_VERSION = "version";

#define DB_ASSERT(x,q) Q_ASSERT_X((x), __FUNCTION__, (q).lastError().text().toStdString().c_str());


DbHelper::DbHelper()
    : mDb(),
      mOngoingTransactionsCount(0),
      mVersion(-1)
{
    mInsertThread = new QThread();
}

DbHelper::~DbHelper()
{
    mDb.commit();
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
    checkNodesStats(mVersion);
    checkVesselsPosTable(mVersion);
    checkVesselsTable(mVersion);
    checkStatsTable(mVersion);

    /* update ended here */
    mVersion = VERSION;

    mInserter = new VesselPositionInserter(this, &mDb);

    connect (this, SIGNAL(postVesselInsertion(int,int,double,double,double,double,int)), mInserter, SLOT(addVesselPosition(int,int,double,double,double,double,int)));
    connect (this, SIGNAL(flush()), mInserter, SLOT(flush()));

    mInserter->moveToThread(mInsertThread);
    mInsertThread->start();

    return true;
}

QString DbHelper::lastDbError() const
{
    return mDb.lastError().text();
}

void DbHelper::addVesselPosition(int step, int idx, VesselData *vessel)
{
    emit postVesselInsertion(step, idx, vessel->mVessel->get_x(), vessel->mVessel->get_y(), vessel->mVessel->get_course(), vessel->mVessel->get_fuelcons(), vessel->mVessel->get_state());
}

void DbHelper::removeAllNodesDetails()
{
    QSqlQuery q;
    bool res = q.exec("DELETE FROM " + TBL_NODES);
    DB_ASSERT(res,q);
}

void DbHelper::addNodesStats(int tstep, const QList<NodeData *> &nodes)
{
    QSqlQuery q, sq;

    bool r =
    q.prepare("INSERT INTO " + TBL_NODES_STATS
              + "(nodeid,tstep,cumftime,totpop) "
              + "VALUES (?,?,?,?)");

    DB_ASSERT(r,q);

    r = sq.prepare("INSERT INTO " + TBL_POPNODES_STATS
        + "(statid,tstep,nodeid,popid,value) VALUES(?,?,?,?,?)");
    DB_ASSERT(r,sq);

    foreach (NodeData *n, nodes) {
        q.addBindValue(n->get_idx_node());
        q.addBindValue(tstep);
        q.addBindValue(n->get_cumftime());
        q.addBindValue(n->getPopTot());

        bool res = q.exec();
        DB_ASSERT(res, q);

        int statid = q.lastInsertId().toInt();

        for (int i = 0; i < n->getPopCount(); ++i) {
            sq.addBindValue(statid);
            sq.addBindValue(tstep);
            sq.addBindValue(n->get_idx_node());
            sq.addBindValue(i);
            sq.addBindValue(n->getPop(i));

            res = sq.exec();
            DB_ASSERT(res,sq);
        }
    }
}

void DbHelper::addPopStats(int tstep, const QVector<PopulationData > &pops)
{
    QSqlQuery q;

    bool r =
    q.prepare("INSERT INTO " + TBL_POP_STATS
              + "(tstep,popid,N,F) "
              + "VALUES (?,?,?,?)");

    DB_ASSERT(r,q);

    foreach (const PopulationData &p, pops) {
        q.addBindValue(tstep);
        q.addBindValue(p.getId());
        q.addBindValue(p.getAggregate());
        q.addBindValue(p.getMortality());

        bool res = q.exec();
        DB_ASSERT(res, q);
    }
}

void DbHelper::removeAllVesselsDetails()
{
    QSqlQuery q;
    bool res = q.exec("DELETE FROM " + TBL_VESSELS);
    DB_ASSERT(res,q);
}

void DbHelper::addNodesDetails(int idx, NodeData *node)
{
    Q_UNUSED(idx);
    bool res;
    QSqlQuery q;

    res = q.prepare("INSERT INTO " + TBL_NODES
                + "(_id,x,y,harbour,areacode,landscape,name) "
                + "VALUES (?,?,?,?,?,?,?)");

    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());

    q.addBindValue(node->get_idx_node());
    q.addBindValue(node->get_x());
    q.addBindValue(node->get_y());
    q.addBindValue(node->get_harbour());
    q.addBindValue(node->get_code_area());
    q.addBindValue(node->get_marine_landscape());
    if (node->get_harbour()) {
        q.addBindValue(QString::fromStdString(node->get_name()));
    } else {
        q.addBindValue(QVariant());
    }


    res = q.exec();
    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());
}

void DbHelper::addVesselDetails(int idx, VesselData *vessel)
{
    bool res;

    QSqlQuery q;
    res = q.prepare("INSERT INTO " +TBL_VESSELS
                    + "(_id,name,node) VALUES(?,?,?)");

    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());

    q.addBindValue(idx);
    q.addBindValue(QString::fromUtf8(vessel->mVessel->get_name().c_str()));
    q.addBindValue(vessel->mVessel->get_loc()->get_idx_node());
    res = q.exec();
    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());
}

bool DbHelper::loadConfig(Config &cfg)
{
    cfg.setNbpops(getMetadata("config::nbpops").toInt());

    QList<int> ipops;
    QStringList lsi = getMetadata("config::ipops").split(" ");
    foreach (QString i, lsi) {
        ipops.push_back(i.toInt());
    }
    cfg.setImplicit_pops(ipops);

    QList<double> vl;
    lsi = getMetadata("config::calib_oth_landings").split(" ");
    foreach (QString i, lsi) {
        vl.push_back(i.toDouble());
    }
    cfg.setCalib_oth_landings(vl);

    vl.clear();
    lsi = getMetadata("config::calib_weight_at_szgroup").split(" ");
    foreach (QString i, lsi) {
        vl.push_back(i.toDouble());
    }
    cfg.setCalib_weight_at_szgroup(vl);

    vl.clear();
    lsi = getMetadata("config::calib_cpue_multi").split(" ");
    foreach (QString i, lsi) {
        vl.push_back(i.toDouble());
    }
    cfg.setCalib_cpue_multiplier(vl);

    return true;
}

bool DbHelper::saveConfig(const Config &cfg)
{
    setMetadata("config::nbpops", QString::number(cfg.getNbpops()));

    QStringList str;
    QList<int> il = cfg.implicit_pops();
    foreach (int i, il)
        str.push_back(QString::number(i));
    setMetadata("config::ipops", str.join(" "));

    str.clear();
    QList<double> dl = cfg.calib_oth_landings();
    foreach (double d, dl)
        str.push_back(QString::number(d));
    setMetadata("config::calib_oth_landings", str.join(" "));

    str.clear();
    dl = cfg.calib_weight_at_szgroup();
    foreach (double d, dl)
        str.push_back(QString::number(d));
    setMetadata("config::calib_weight_at_szgroup", str.join(" "));

    str.clear();
    dl = cfg.calib_cpue_multiplier();
    foreach (double d, dl)
        str.push_back(QString::number(d));
    setMetadata("config::calib_cpue_multi", str.join(" "));

    return true;
}

bool DbHelper::loadScenario(Scenario &sce)
{
    sce.setGraph(getMetadata("sce::graph").toInt());
    sce.setNrow_graph(getMetadata("sce::nrow_graph").toInt());
    sce.setNrow_coord(getMetadata("sce::nrow_coord").toInt());
    sce.setA_port(getMetadata("sce::aport").toInt());
    sce.setGraph_res(getMetadata("sce::graph_res").toDouble());
    sce.setDyn_alloc_sce(getMetadata("sce::dyn_alloc_sce").split(" "));
    sce.setDyn_pop_sce(getMetadata("sce::dyn_pop_sce").split(" "));
    sce.setBiolsce(getMetadata("sce::biol_sce"));

    return true;
}

bool DbHelper::saveScenario(const Scenario &sce)
{
    setMetadata("sce::graph", QString::number(sce.getGraph()));
    setMetadata("sce::nrow_graph", QString::number(sce.getNrow_graph()));
    setMetadata("sce::nrow_coord", QString::number(sce.getNrow_coord()));
    setMetadata("sce::aport", QString::number(sce.getA_port()));
    setMetadata("sce::graph_res", QString::number(sce.getGraph_res()));
    setMetadata("sce::dyn_alloc_sce", sce.getDyn_alloc_sce().join(" "));
    setMetadata("sce::dyn_pop_sce", sce.getDyn_pop_sce().join(" "));
    setMetadata("sce::biol_sce", sce.getBiolsce());

    return true;
}

bool DbHelper::loadNodes(QList<NodeData *> &nodes, QList<Harbour *> &harbours, DisplaceModel *model)
{
    QSqlQuery q("SELECT _id,x,y,harbour,areacode,landscape,name FROM " + TBL_NODES + " ORDER BY _id");
    bool res = q.exec();

    DB_ASSERT(res,q);

    while (q.next()) {
        int idx = q.value(0).toInt();
        double x = q.value(1).toDouble();
        double y = q.value(2).toDouble();
        int harbour = q.value(3).toInt();
        int areacode = q.value(4).toInt();
        int landscape = q.value(5).toInt();

        int nbpops = model->getNBPops();
        int szgroup = model->getSzGrupsCount();
        QString name = q.value(6).toString();

        /* TODO: a,b */
        multimap<int,double> a;
        map<string,double> b;
        Node *nd;
        Harbour *h;
        if (harbour) {
            nd = h = new Harbour(idx, x, y, harbour,areacode,landscape,nbpops, szgroup, name.toStdString(),a,b);
        } else {
            nd = new Node(idx, x, y, harbour, areacode, landscape, nbpops, szgroup);
        }
        NodeData*n = new NodeData(nd, model);

        while (nodes.size() < idx+1)
            nodes.push_back(0);

        nodes[idx] = n;
        if (n->get_harbour())
            harbours.push_back(h);
    }

    return true;
}

bool DbHelper::loadVessels(const QList<NodeData *> &nodes, QList<VesselData *> &vessels)
{
    QSqlQuery q("SELECT _id,name,node FROM " + TBL_VESSELS + " ORDER BY _id");
    if (!q.exec()) {
        return false;
    }

    while (q.next()) {
        int idx = q.value(0).toInt();
        int nidx = q.value(2).toInt();

        Vessel *vsl = new Vessel(nodes.at(nidx)->mNode, idx, q.value(1).toString().toStdString());
        VesselData *v = new VesselData(vsl);

        while (vessels.size() < idx+1)
            vessels.push_back(0);

        vessels[idx] = v;
    }

    return true;
}

bool DbHelper::updateVesselsToStep(int steps, QList<VesselData *> &vessels)
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

        VesselData *v = vessels.at(idx);
        v->mVessel->set_xy(x,y);
        v->mVessel->set_fuelcons(fuel);
        v->mVessel->set_state(state);
        v->mVessel->set_cumcatches(cum);
        v->mVessel->set_timeatsea(tim);
        v->mVessel->set_reason_to_go_back(r);
        v->mVessel->set_course(course);
    }
    return true;
}

bool DbHelper::updateStatsForNodesToStep(int step, QList<NodeData *> &nodes)
{
    QSqlQuery q;
    bool res =
    q.prepare ("SELECT nodeid,popid,value FROM " + TBL_POPNODES_STATS
               + " WHERE tstep=?");
    DB_ASSERT(res,q);

    q.addBindValue(step);
    res = q.exec();
    while (q.next()) {
        int nid = q.value(0).toInt();
        int pid = q.value(1).toInt();
        double val = q.value(2).toDouble();

        nodes.at(nid)->setPop(pid,val);
    }
    return true;
}

bool DbHelper::loadHistoricalStatsForPops(QList<int> &steps, QList<QVector<PopulationData> > &population)
{
    QSqlQuery q;
    bool res = q.prepare("SELECT tstep,popid,N,F FROM " + TBL_POP_STATS + " ORDER BY tstep");
    DB_ASSERT(res,q);


    int last_tstep = -1;
    population.clear();

    res = q.exec();
    QVector<PopulationData> v;
    while (q.next()) {
        int tstep = q.value(0).toInt();

        if (last_tstep != tstep && last_tstep != -1) {
            steps.push_back(last_tstep);
            population.push_back(v);
        }
        last_tstep = tstep;

        int pid = q.value(1).toInt();
        double n = q.value(2).toDouble();
        double f = q.value(3).toDouble();

        for (int i = v.size(); i <= pid; ++i) {
            PopulationData p(i);
            v.push_back(p);
        }

        v[pid].setAggregate(n);
        v[pid].setMortality(f);
    }

    if (last_tstep != -1) {
        population.push_back(v);
        steps.push_back(last_tstep);
    }

    return true;

}

void DbHelper::beginTransaction()
{
    QMutexLocker locker(&mMutex);
    if (mOngoingTransactionsCount == 0) {
        mDb.transaction();
    }
    ++mOngoingTransactionsCount;
}

void DbHelper::endTransaction()
{
    QMutexLocker locker(&mMutex);
    --mOngoingTransactionsCount;

    if (mOngoingTransactionsCount < 0)
        mOngoingTransactionsCount = 0;
    if (mOngoingTransactionsCount == 0) {
        mDb.commit();
    }
}

void DbHelper::forceEndTransaction()
{
    QMutexLocker locker(&mMutex);
    mOngoingTransactionsCount = 0;
    mDb.commit();
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
               + "name VARCHAR(32)"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (version < 2) {

    }

    return true;
}

bool DbHelper::checkNodesStats(int version)
{
    if (!mDb.tables().contains(TBL_NODES_STATS)) {
        QSqlQuery q;
        bool r =
        q.exec("CREATE TABLE " + TBL_NODES_STATS + "("
               + "statid INTEGER PRIMARY KEY AUTOINCREMENT,"
               + "nodeid INTEGER,"
               + "tstep INTEGER,"
               + "cumftime REAL,"
               + "totpop REAL"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());

        q.exec("CREATE INDEX Idx" + TBL_NODES_STATS + " ON " + TBL_NODES_STATS + "(nodeid)");
        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (!mDb.tables().contains(TBL_POPNODES_STATS)) {
        QSqlQuery q;
        bool r =
        q.exec("CREATE TABLE " + TBL_POPNODES_STATS + "("
               + "statid INTEGER,"
               + "tstep INTEGER,"
               + "nodeid INTEGER,"
               + "popid INTEGER,"
               + "value REAL"
               + ");");
        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());

        q.exec("CREATE INDEX Idx" + TBL_POPNODES_STATS + " ON " + TBL_POPNODES_STATS + "(statid)");
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

bool DbHelper::checkStatsTable(int version)
{
    if (!mDb.tables().contains(TBL_POP_STATS)) {
        QSqlQuery q;
        bool r =
        q.exec("CREATE TABLE " + TBL_POP_STATS + "("
               + "statid INTEGER AUTO_INCREMENT PRIMARY KEY,"
               + "tstep INTEGER,"
               + "popid INTEGER,"
               + "N REAL,"
               + "F REAL"
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
    if (mCounter >= mFlushCount) {
        mHelper->endTransaction();
        mHelper->beginTransaction();
        mCounter = 0;
    }

    /* if there's no transaction ongoing, start it */
    if (mCounter == 0)
        mHelper->beginTransaction();

    /* if it's a new step, commits the insertion and restart */
    ++mCounter;

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
    mHelper->forceEndTransaction();
    mCounter = 0;
}
