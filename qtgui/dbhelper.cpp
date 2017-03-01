// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

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
const QString DbHelper::TBL_BENTHOSPOPNODES_STATS = "BenthosPopNodesStats";
const QString DbHelper::TBL_POP_STATS = "PopStats";
const QString DbHelper::TBL_POPSZ_STATS = "PopStatsSz";
const QString DbHelper::TBL_VESSELS = "VesselsNames";
const QString DbHelper::TBL_VESSELS_POS = "VesselsPos";
const QString DbHelper::TBL_VESSELS_STATS_TM = "VesselsStatsTimed";
const QString DbHelper::TBL_VESSELS_STATS_TMSZ = "VesselsStatsPerPop";

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
    static int num = 0;

    if (mDb.isOpen()) {
        mDb.close();
    }

    mDb = QSqlDatabase::addDatabase("QSQLITE", QString::number(num++));
    mDb.setDatabaseName(file);

    qDebug() << "Database" << mDb.connectionName();

    if (!mDb.open()) {
        qWarning() << "Cannot open database: " << mDb.lastError().text();
        return false;
    }

    // prepare helpers query
    // check tables
    checkMetadataTable();
    checkNodesTable(mVersion);
    checkNodesStats(mVersion);
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

void DbHelper::addVesselPosition(int step, int idx, std::shared_ptr<VesselData> vessel)
{
    emit postVesselInsertion(step, idx, vessel->mVessel->get_x(), vessel->mVessel->get_y(), vessel->mVessel->get_course(), vessel->mVessel->get_fuelcons(), vessel->mVessel->get_state());
}

void DbHelper::removeAllNodesDetails()
{
    QSqlQuery q(mDb);
    bool res = q.exec("DELETE FROM " + TBL_NODES);
    DB_ASSERT(res,q);
}

void DbHelper::addNodesStats(int tstep, const QList<std::shared_ptr<NodeData> > &nodes)
{
    QSqlQuery q(mDb), sq(mDb), sq2(mDb);


    bool r =
    q.prepare("INSERT INTO " + TBL_NODES_STATS
              + "(nodeid,tstep,cumftime,cumsweptarea,cumcatches, totpop,totpopw, tariffs) "
              + "VALUES (?,?,?,?,?,?,?,?)");

    DB_ASSERT(r,q);

    r = sq.prepare("INSERT INTO " + TBL_POPNODES_STATS
        + "(statid,tstep,nodeid,popid,pop,popw,impact,cumcatchesthispop) VALUES(?,?,?,?,?,?,?,?)");
    DB_ASSERT(r,sq);

    r = sq2.prepare("INSERT INTO " + TBL_BENTHOSPOPNODES_STATS
        + "(statid,tstep,nodeid,funcid, benthosbiomass, benthosnumber) VALUES(?,?,?,?,?,?)");
    DB_ASSERT(r,sq2);

    foreach (std::shared_ptr<NodeData> n, nodes) {
        q.addBindValue(n->get_idx_node());
        q.addBindValue(tstep);
        q.addBindValue(n->get_cumftime());
        q.addBindValue(n->get_cumsweptarea());
        q.addBindValue(n->get_cumcatches());
        q.addBindValue(n->getPopTot());
        q.addBindValue(n->getPopWTot());
        q.addBindValue(n->get_tariffs().at(0));

        bool res = q.exec();
        DB_ASSERT(res, q);

        int statid = q.lastInsertId().toInt();

        for (int i = 0; i < n->getPopCount(); ++i) {
            sq.addBindValue(statid);
            sq.addBindValue(tstep);
            sq.addBindValue(n->get_idx_node());
            sq.addBindValue(i);
            sq.addBindValue(n->getPop(i));
            sq.addBindValue(n->getPopW(i));
            sq.addBindValue(n->getImpact(i));
            sq.addBindValue(n->getCumcatchesPerPop(i));


            res = sq.exec();
            DB_ASSERT(res,sq);
        }


        for (int j = 0; j < n->getBenthosPopCount(); ++j) {
            sq2.addBindValue(statid);
            sq2.addBindValue(tstep);
            sq2.addBindValue(n->get_idx_node());
            sq2.addBindValue(j);
            sq2.addBindValue(n->getBenthosBiomass(j));
            sq2.addBindValue(n->getBenthosNumber(j));

            res = sq2.exec();
            DB_ASSERT(res,sq2);
        }
    }
}

void DbHelper::addPopStats(int tstep, const QVector<PopulationData > &pops)
{
    QSqlQuery q(mDb),qn(mDb);

    bool r =
    q.prepare("INSERT INTO " + TBL_POP_STATS
              + "(tstep,popid,N,F,SSB) "
              + "VALUES (?,?,?,?, ?)");
    DB_ASSERT(r,q);

    r =
    qn.prepare("INSERT INTO " + TBL_POPSZ_STATS
              + "(tstep,popid,szgroup,N,F,SSB) "
              + "VALUES (?,?,?,?,?, ?)");
    DB_ASSERT(r,qn);


    foreach (PopulationData p, pops) {
        q.addBindValue(tstep);
        q.addBindValue(p.getId());
        q.addBindValue(p.getAggregateTot());
        q.addBindValue(p.getMortalityTot());
        q.addBindValue(p.getSSBTot());

        bool res = q.exec();
        DB_ASSERT(res, q);

        int n = p.getAggregate().size();
        for (int i = 0; i < n; ++i) {
            qn.addBindValue(tstep);
            qn.addBindValue(p.getId());
            qn.addBindValue(i);
            qn.addBindValue(p.getAggregate().at(i));
            qn.addBindValue(p.getMortality().at(i));
            qn.addBindValue(i < p.getSSB().size() ? p.getSSB().at(i) : 0);
            res = qn.exec();
            DB_ASSERT(res,qn);
        }
    }
}

void DbHelper::addNationsStats(int tstep, const QVector<NationStats> &nats)
{
    Q_UNUSED(tstep);
    Q_UNUSED(nats);
}

void DbHelper::addVesselStats(int tstep, const VesselData &vessel, const VesselStats &stats)
{
    QSqlQuery q(mDb),qn(mDb);

    bool r = q.prepare("INSERT INTO " + TBL_VESSELS_STATS_TM
                       + "(tstep,vid,timeatsea,harbour,reason,revenue_av,explicit_revenue_av,cumfuel,fuelcost,gav)"
                       + " VALUES(?,?,?,?,?,?,?,?,?,?)");
    DB_ASSERT(r,q);

    r = qn.prepare("INSERT INTO " + TBL_VESSELS_STATS_TMSZ
                   + "(tstep,vid,harbour,sz,cum)"
                   + " VALUES(?,?,?,?,?)");
    DB_ASSERT(r,qn);

    q.addBindValue(tstep);
    q.addBindValue(vessel.mVessel->get_idx());
    q.addBindValue(stats.timeAtSea);
    q.addBindValue(vessel.getLastHarbour());
    q.addBindValue(stats.reasonToGoBack);
    q.addBindValue(stats.revenueAV);
    q.addBindValue(stats.revenueExAV);
    q.addBindValue(stats.cumFuelCons);
    q.addBindValue(stats.fuelCost);
    q.addBindValue(stats.gav);

    r = q.exec();
    DB_ASSERT(r,q);

    int n = vessel.getCatchesListSize();
    for (int i =0; i < n; ++i) {
        qn.addBindValue(tstep);
        qn.addBindValue(vessel.mVessel->get_idx());
        qn.addBindValue(vessel.getLastHarbour());
        qn.addBindValue(i);
        qn.addBindValue(vessel.getCatch(i));

        r = qn.exec();
        DB_ASSERT(r,qn);
    }
}

void DbHelper::removeAllVesselsDetails()
{
    QSqlQuery q(mDb);
    bool res = q.exec("DELETE FROM " + TBL_VESSELS);
    DB_ASSERT(res,q);
}

void DbHelper::addNodesDetails(int idx, std::shared_ptr<NodeData> node)
{
    Q_UNUSED(idx);
    bool res;
    QSqlQuery q(mDb);

    res = q.prepare("INSERT INTO " + TBL_NODES
                + "(_id,x,y,harbour,areacode,landscape,benthosbiomass,benthosnumber, name) "
                + "VALUES (?,?,?,?,?,?,?,?,?)");

    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());

    q.addBindValue(node->get_idx_node());
    q.addBindValue(node->get_x());
    q.addBindValue(node->get_y());
    q.addBindValue(node->get_harbour());
    q.addBindValue(node->get_code_area());
    q.addBindValue(node->get_marine_landscape());
    q.addBindValue(node->get_init_benthos_biomass());
    q.addBindValue(node->get_init_benthos_number());
    if (node->get_harbour()) {
        q.addBindValue(QString::fromStdString(node->get_name()));
    } else {
        q.addBindValue(QVariant());
    }


    res = q.exec();
    Q_ASSERT_X(res, __FUNCTION__, q.lastError().text().toStdString().c_str());
}

void DbHelper::addVesselDetails(int idx, std::shared_ptr<VesselData> vessel)
{
    bool res;

    QSqlQuery q(mDb);
    res = q.prepare("INSERT INTO " +TBL_VESSELS
                    + "(_id,name,node) VALUES(?,?,?)");

    DB_ASSERT(res,q);

    q.addBindValue(idx);
    q.addBindValue(QString::fromUtf8(vessel->mVessel->get_name().c_str()));
    q.addBindValue(vessel->mVessel->get_loc()->get_idx_node());
    res = q.exec();

    DB_ASSERT(res,q);
}

void DbHelper::removeAllStatsData()
{
    QSqlQuery q(mDb);
    bool res = q.exec("DELETE FROM " +TBL_NODES_STATS);
    DB_ASSERT(res,q);
    res = q.exec("DELETE FROM " + TBL_POPNODES_STATS);
    DB_ASSERT(res,q);
    res = q.exec("DELETE FROM " + TBL_BENTHOSPOPNODES_STATS);
    DB_ASSERT(res,q);
    res = q.exec("DELETE FROM " + TBL_POP_STATS);
    DB_ASSERT(res,q);
    res = q.exec("DELETE FROM " + TBL_POPSZ_STATS);
    DB_ASSERT(res,q);
    res = q.exec("DELETE FROM " + TBL_VESSELS_POS);
    DB_ASSERT(res,q);
    res = q.exec("DELETE FROM " + TBL_VESSELS_STATS_TM);
    DB_ASSERT(res,q);
    res = q.exec("DELETE FROM " + TBL_VESSELS_STATS_TMSZ);
    DB_ASSERT(res,q);
}


bool DbHelper::loadConfig(Config &cfg)
{
    cfg.setNbpops(getMetadata("config::nbpops").toInt());
    cfg.setNbbenthospops(getMetadata("config::nbbenthospops").toInt());
    cfg.setSzGroups(getMetadata("config::szgroups").toInt());

    QList<int> ipops;
    QStringList lsi = getMetadata("config::ipops").split(" ");
    foreach (QString i, lsi) {
        ipops.push_back(i.toInt());
    }
    cfg.setImplicit_pops(ipops);

    QList<int> ipops_level2;
    QStringList lsi2 = getMetadata("config::ipops_level2").split(" ");
    foreach (QString i2, lsi2) {
        ipops_level2.push_back(i2.toInt());
    }
    cfg.setImplicit_pops_level2(ipops_level2);

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
    lsi = getMetadata("config::calib_cpue_multiplier").split(" ");
    foreach (QString i, lsi) {
        vl.push_back(i.toDouble());
    }
    cfg.setCalib_cpue_multiplier(vl);

    ipops.clear();
    lsi = getMetadata("config::int_harbours").split(" ");
    foreach (QString i, lsi) {
        ipops.push_back(i.toInt());
    }
    cfg.m_interesting_harbours = ipops;

    return true;
}

bool DbHelper::saveConfig(const Config &cfg)
{
    setMetadata("config::nbpops", QString::number(cfg.getNbpops()));
    setMetadata("config::szgroups", QString::number(cfg.getSzGroups()));

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
    setMetadata("config::calib_cpue_multiplier", str.join(" "));

    str.clear();
    il = cfg.m_interesting_harbours;
    foreach (int d, il)
        str.push_back(QString::number(d));
    setMetadata("config::int_harbours", str.join(" "));

    return true;
}

bool DbHelper::loadScenario(Scenario &sce)
{
    sce.setGraph(getMetadata("sce::graph").toInt());
    sce.setNrow_graph(getMetadata("sce::nrow_graph").toInt());
    sce.setNrow_coord(getMetadata("sce::nrow_coord").toInt());
    sce.setA_port(getMetadata("sce::aport").toInt());
    sce.setGraph_res(getMetadata("sce::graph_res").toDouble());
    sce.setIs_individual_vessel_quotas(getMetadata("sce::is_individual_vessel_quotas").toDouble());
    sce.setIs_check_all_stocks_before_going_fishing(getMetadata("sce::check_all_stocks_before_going_fishing").toDouble());
    sce.setDyn_alloc_sce(getMetadata("sce::dyn_alloc_sce").split(" "));
    sce.setDyn_pop_sce(getMetadata("sce::dyn_pop_sce").split(" "));
    sce.setBiolsce(getMetadata("sce::biol_sce"));
    sce.setFleetsce(getMetadata("sce::fleet_sce"));
    sce.setFreqDoGrowth(getMetadata("sce::freq_do_growth").toInt());
    sce.setFreqDispatchThePop(getMetadata("sce::freq_redispatch_the_pop").toInt());

    return true;
}

bool DbHelper::saveScenario(const Scenario &sce)
{
    setMetadata("sce::graph", QString::number(sce.getGraph()));
    setMetadata("sce::nrow_graph", QString::number(sce.getNrow_graph()));
    setMetadata("sce::nrow_coord", QString::number(sce.getNrow_coord()));
    setMetadata("sce::aport", QString::number(sce.getA_port()));
    setMetadata("sce::graph_res", QString::number(sce.getGraph_res()));
    setMetadata("sce::is_individual_vessel_quotas", (sce.getIs_individual_vessel_quotas() ? "1" : "0"));
    setMetadata("sce::check_all_stocks_before_going_fishing", (sce.getIs_check_all_stocks_before_going_fishing() ? "1" : "0"));
    setMetadata("sce::dyn_alloc_sce", sce.getDyn_alloc_sce().join(" "));
    setMetadata("sce::dyn_pop_sce", sce.getDyn_pop_sce().join(" "));
    setMetadata("sce::biol_sce", sce.getBiolsce());
    setMetadata("sce::fleet_sce", sce.getFleetsce());
    setMetadata("sce::freq_do_growth", QString::number(sce.getFreqDoGrowth()));
    setMetadata("sce::freq_redispatch_the_pop", QString::number(sce.getFreqDispatchThePop()));

    return true;
}

bool DbHelper::loadNodes(QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<HarbourData> > &harbours, DisplaceModel *model)
{
    QSqlQuery q("SELECT _id,x,y,harbour,areacode,landscape,benthosbiomass,benthosnumber, name FROM " + TBL_NODES + " ORDER BY _id", mDb);
    bool res = q.exec();

    DB_ASSERT(res,q);

    while (q.next()) {
        int idx = q.value(0).toInt();
        double x = q.value(1).toDouble();
        double y = q.value(2).toDouble();
        int harbour = q.value(3).toInt();
        int areacode = q.value(4).toInt();
        int landscape = q.value(5).toInt();
        int benthosbiomass = q.value(6).toInt();
        int benthosnumber = q.value(7).toInt();

        int nbpops = model->getNBPops();
        int nbbenthospops = model->getNBBenthosPops();
        int szgroup = model->getSzGrupsCount();
        QString name = q.value(8).toString();

        /* TODO: a,b,c,d */
        multimap<int,double> a;
        map<int,double> b;
        vector<int> c;
        vector<double> d;

        std::shared_ptr<Node> nd;
        std::shared_ptr<Harbour> h;
        if (harbour) {
            nd = h = std::shared_ptr<Harbour> (new Harbour(idx, x, y, harbour,areacode,landscape,benthosbiomass, benthosnumber, nbpops, nbbenthospops, szgroup, name.toStdString(),a,b,c,d));
        } else {
            nd = std::shared_ptr<Node>(new Node(idx, x, y, harbour, areacode, landscape,benthosbiomass, benthosnumber, nbpops, nbbenthospops,  szgroup));
        }
        std::shared_ptr<NodeData> n(new NodeData(nd, model));

        while (nodes.size() < idx+1)
            nodes.push_back(0);

        nodes[idx] = n;
        if (n->get_harbour()) {
            n->setHarbourId(harbours.size());
            std::shared_ptr<HarbourData> hdt (new HarbourData(h));
            harbours.push_back(hdt);
        }
    }

    return true;
}

bool DbHelper::loadVessels(const QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<VesselData> > &vessels)
{
    QSqlQuery q("SELECT _id,name,node FROM " + TBL_VESSELS + " ORDER BY _id", mDb);
    if (!q.exec()) {
        return false;
    }

    while (q.next()) {
        int idx = q.value(0).toInt();
        int nidx = q.value(2).toInt();

        std::shared_ptr<Vessel> vsl (new Vessel(nodes.at(nidx)->mNode.get(), idx, q.value(1).toString().toStdString()));
        std::shared_ptr<VesselData> v(new VesselData(vsl));

        while (vessels.size() < idx+1)
            vessels.push_back(0);

        vessels[idx] = v;
    }

    return true;
}

bool DbHelper::updateVesselsToStep(int steps, QList<std::shared_ptr<VesselData> > &vessels)
{
    QSqlQuery q(mDb);
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

        if (idx < vessels.size()) {
            std::shared_ptr<VesselData> v (vessels.at(idx));
            v->mVessel->set_xy(x,y);
            v->mVessel->set_fuelcons(fuel);
            v->mVessel->set_state(state);
            v->mVessel->set_cumcatches(cum);
            v->mVessel->set_timeatsea(tim);
            v->mVessel->set_reason_to_go_back(r);
            v->mVessel->set_course(course);
        }
    }
    return true;
}

bool DbHelper::updateStatsForNodesToStep(int step, QList<std::shared_ptr<NodeData> > &nodes)
{
    QSqlQuery q(mDb);
    bool res = q.prepare("SELECT nodeid,cumftime,cumsweptarea,cumcatches,totpop,totpopw,tariffs FROM " + TBL_NODES_STATS + " WHERE tstep<=? GROUP BY nodeid");
    DB_ASSERT(res,q);

    q.addBindValue(step);
    res = q.exec();
    while (q.next()) {
        int nid = q.value(0).toInt();
        double cum = q.value(1).toDouble();
        double cumsw = q.value(2).toDouble();
        double cumca = q.value(3).toDouble();
        double tot = q.value(4).toDouble();
        double totw = q.value(5).toDouble();
        double tariff = q.value(6).toDouble();
        vector<double> tariffs;
        tariffs.push_back(tariff);
        tariffs.push_back(0);

        if (nid < nodes.size()) {
            nodes.at(nid)->set_cumftime(cum);
            nodes.at(nid)->set_cumsweptarea(cumsw);
            nodes.at(nid)->set_cumcatches(cumca);
            nodes.at(nid)->setPopTot(tot);
            nodes.at(nid)->setPopWTot(totw);
            nodes.at(nid)->set_tariffs(tariffs);
        }
    }

    q.prepare ("SELECT nodeid,popid,pop,popw,impact,cumcatchesthispop FROM " + TBL_POPNODES_STATS
               + " WHERE tstep=?");
    DB_ASSERT(res,q);

    q.addBindValue(step);
    res = q.exec();
    while (q.next()) {
        int nid = q.value(0).toInt();
        int pid = q.value(1).toInt();
        double val = q.value(2).toDouble();
        double valw = q.value(3).toDouble();
        double impact = q.value(4).toDouble();
        double cumcatchesthispop = q.value(5).toDouble();

        if (nid < nodes.size()) {
            nodes.at(nid)->setPop(pid,val);
            nodes.at(nid)->setPopW(pid,valw);
            nodes.at(nid)->setImpact(pid,impact);
            nodes.at(nid)->setCumcatchesPerPop(pid,cumcatchesthispop);
        }
    }

    q.prepare ("SELECT nodeid,funcid,benthosbiomass,benthosnumber FROM " + TBL_BENTHOSPOPNODES_STATS
               + " WHERE tstep=?");
    DB_ASSERT(res,q);

    q.addBindValue(step);
    res = q.exec();
    while (q.next()) {
        int nid = q.value(0).toInt();
        int funcid = q.value(1).toInt();
        double benthosbiomass = q.value(2).toDouble();
        if (nid < nodes.size()) {
            nodes.at(nid)->setBenthosBiomass(funcid,benthosbiomass);
        }
        double benthosnumber = q.value(3).toDouble();
        if (nid < nodes.size()) {
            nodes.at(nid)->setBenthosNumber(funcid,benthosnumber);
        }
    }


    return true;
}

bool DbHelper::loadHistoricalStatsForPops(QList<int> &steps, QList<QVector<PopulationData> > &population)
{
    QSqlQuery q(mDb);
    bool res = q.prepare("SELECT tstep,popid,N,F,SSB FROM " + TBL_POP_STATS + " ORDER BY tstep");
    DB_ASSERT(res,q);

    QSqlQuery qn(mDb);
    res = qn.prepare("SELECT szgroup,N,F,SSB FROM " + TBL_POPSZ_STATS + " WHERE tstep=? AND popid=?");
    DB_ASSERT(res,qn);

    int last_tstep = -1;
    population.clear();

    res = q.exec();
    QVector<PopulationData> v;
    while (q.next()) {
        int tstep = q.value(0).toInt();

        if (last_tstep != tstep && last_tstep != -1) {
            steps.push_back(last_tstep);
            population.push_back(v);            
            v.clear();
        }
        last_tstep = tstep;

        int pid = q.value(1).toInt();
        double n = q.value(2).toDouble();
        double f = q.value(3).toDouble();
        double ssb = q.value(4).toDouble();

        for (int i = v.size(); i <= pid; ++i) {
            PopulationData p(i);
            v.push_back(p);
        }

        /* Load size groups */

        qn.addBindValue(tstep);
        qn.addBindValue(pid);

        res = qn.exec();
        DB_ASSERT(res, qn);

        QVector<double> nv;
        QVector<double> fv;
        QVector<double> ssbv;
        while (qn.next()) {
            int sz = qn.value(0).toInt();
            double agg = qn.value(1).toDouble();
            double mor = qn.value(2).toDouble();
            double ssbb = qn.value(2).toDouble();

            while (nv.size() <= sz)
                nv.push_back(0);
            nv[sz] = agg;
            while (fv.size() <= sz)
                fv.push_back(0);
            fv[sz] = mor;
            while (ssbv.size() <= sz)
                ssbv.push_back(0);
            ssbv[sz] = ssbb;
        }

        v[pid].setAggregate(nv);
        v[pid].setMortality(fv);
        v[pid].setSSB(ssbv);
        v[pid].setAggregateTot(n);
        v[pid].setMortalityTot(f);
        v[pid].setSSBTot(ssb);

    }

    if (last_tstep != -1) {
        population.push_back(v);
        steps.push_back(last_tstep);
    }

    return true;

}

bool DbHelper::loadHistoricalStatsForVessels(const QList<int> &steps, const QList<std::shared_ptr<VesselData> > &vessels,
                                             const QList<std::shared_ptr<NodeData> >&nodes,
                                             QList<QVector<NationStats> > &nations,
                                             QList<QVector<HarbourStats> > &harbour)
{
    QSqlQuery q(mDb);
    bool res = q.prepare("SELECT vid,sz,SUM(cum),harbour FROM "+ TBL_VESSELS_STATS_TMSZ + " WHERE tstep<=? GROUP BY vid,sz"); /*,harbour  */
    DB_ASSERT(res,q);

    QSqlQuery q2(mDb);
    res = q2.prepare("SELECT vid,SUM(timeatsea),SUM(revenue_av),SUM(explicit_revenue_av),harbour,SUM(gav),SUM(revenue_av)/SUM(cumfuel) FROM " + TBL_VESSELS_STATS_TM + " WHERE tstep<=? GROUP BY vid");
    DB_ASSERT(res,q2);

    foreach(int tstep, steps) {
        QVector<NationStats> curnationsdata;
        QVector<HarbourStats> curHarbourData;

        q.addBindValue(tstep);
        q2.addBindValue(tstep);

        res = q.exec();
        DB_ASSERT(res,q);

        /* Nations cum catches */
        while (q.next()) {
            int vid = q.value(0).toInt();
            int sz = q.value(1).toInt();
            int nid = vessels.at(vid)->getNationality();
            int catches = q.value(2).toDouble();
            int hid = q.value(3).toInt();
            int hidx = nodes.at(hid)->getHarbourId();

            while (curnationsdata.size() <= nid)
                curnationsdata.push_back(NationStats());

            while (curHarbourData.size() <= hidx)
                curHarbourData.push_back(HarbourStats());

            QVector<double> &g = curnationsdata[nid].szGroups; // alias
            while (g.size() <= sz)
                g.push_back(0.0);
            g[sz] += catches;

            curnationsdata[nid].mTotCatches += catches;
            curHarbourData[hidx].mCumCatches += catches;
        }

        res = q2.exec();
        DB_ASSERT(res,q2);
        while (q2.next()) {
            int vid = q2.value(0).toInt();
            int nid = vessels.at(vid)->getNationality();
            double timeatsea = q2.value(1).toDouble();
            double rev = q2.value(2).toDouble();
            double exrev = q2.value(3).toDouble();
            int hid = q2.value(4).toInt();
            int hidx = nodes.at(hid)->getHarbourId();
            double gav = q2.value(5).toDouble();
            double vpuf = q2.value(6).toDouble();

            while (curnationsdata.size() <= nid)
                curnationsdata.push_back(NationStats());
            while (curHarbourData.size() <= hidx)
                curHarbourData.push_back(HarbourStats());

            curnationsdata[nid].mRevenues += rev;
            curnationsdata[nid].mExRevenues += exrev;
            curnationsdata[nid].mTimeAtSea += timeatsea;
            curnationsdata[nid].mGav += gav;
            curnationsdata[nid].mVpuf = vpuf;
            //curHarbourData[hidx].mCumCatches += catches;
            curHarbourData[hidx].mCumProfit += rev;
            curHarbourData[hidx].mGav += gav;
            curHarbourData[hidx].mVpuf = vpuf;
        }

        nations.push_back(curnationsdata);
        harbour.push_back(curHarbourData);
    }

    return true;
}

HarbourStats DbHelper::getHarbourStatsAtStep(int idx, int step)
{
    QSqlQuery q(mDb);
    bool res = q.prepare("SELECT sz,SUM(cum) FROM "+ TBL_VESSELS_STATS_TMSZ + " WHERE tstep<=? AND harbour=? GROUP BY vid,sz");
    DB_ASSERT(res,q);

    QSqlQuery q2(mDb);
    res = q2.prepare("SELECT SUM(timeatsea),SUM(revenue_av),SUM(revenue_av)/SUM(cumfuel),SUM(gav) FROM " + TBL_VESSELS_STATS_TM + " WHERE tstep<=? AND harbour=? GROUP BY vid");
    DB_ASSERT(res,q2);

    HarbourStats curHarbourData;

    q.addBindValue(step);
    q.addBindValue(idx);
    q2.addBindValue(step);
    q2.addBindValue(idx);

    res = q.exec();
    DB_ASSERT(res,q);

    while (q.next()) {
        int sz = q.value(0).toInt();
        int catches = q.value(1).toDouble();

        QVector<double> &g = curHarbourData.szCatches; // alias
        while (g.size() <= sz)
            g.push_back(0.0);
        g[sz] += catches;

        curHarbourData.mCumCatches += catches;
    }

    res = q2.exec();
    DB_ASSERT(res,q2);
    while (q2.next()) {
        // double timeatsea = q2.value(0).toDouble();
        double rev = q2.value(1).toDouble();

        curHarbourData.mCumProfit += rev;
        curHarbourData.mVpuf += q2.value(2).toDouble();
        curHarbourData.mGav += q2.value(3).toDouble();
    }

    return curHarbourData;
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

void DbHelper::createIndexes()
{
    qDebug() << "Create Indexes";
    Profiler pr;
    createIndexOnTstepForTable(TBL_NODES_STATS);
    createIndexOnTstepForTable(TBL_BENTHOSPOPNODES_STATS);
    createIndexOnTstepForTable(TBL_POPNODES_STATS);
    createIndexOnTstepForTable(TBL_VESSELS_POS);
    qDebug() << "Indexes created in " << pr.elapsed_ms() << " ms";
}

void DbHelper::createIndexOnTstepForTable(QString table)
{
    QSqlQuery q(mDb);
    bool res = q.exec("CREATE INDEX IF NOT EXISTS idx_" + table + " ON " + table + "(tstep)");

    DB_ASSERT(res,q);
}

void DbHelper::setMetadata(QString key, QString value)
{
    QSqlQuery q(mDb);
    q.prepare("INSERT OR REPLACE INTO " +TBL_META
              + "(key,value) VALUES(?,?)");

    q.addBindValue(key);
    q.addBindValue(value);
    bool res = q.exec();
    DB_ASSERT(res,q);
}

QString DbHelper::getMetadata(QString key)
{
    QSqlQuery q(mDb);
    q.prepare("SELECT value FROM " + TBL_META + " WHERE key=?");
    q.addBindValue(key);
    q.exec();
    if (q.next())
        return q.value(0).toString();
    return QString();
}

int DbHelper::getLastKnownStep()
{
    QSqlQuery q(mDb);
    q.exec("SELECT MAX(tstep) FROM " + TBL_VESSELS_POS);
    if (q.next())
        return q.value(0).toInt();
    return -1;
}

bool DbHelper::checkMetadataTable()
{
    if (!mDb.tables().contains(TBL_META)) {
        QSqlQuery q(mDb);
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
        QSqlQuery q(mDb);
        bool r =
        q.exec("CREATE TABLE " + TBL_NODES + "("
               + "_id INTEGER PRIMARY KEY,"
               + "x REAL,"
               + "y REAL,"
               + "harbour INTEGER,"
               + "areacode INTEGER,"
               + "landscape INTEGER,"
               + "benthosbiomass REAL,"
               + "benthosnumber REAL,"
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
        QSqlQuery q(mDb);
        bool r =
        q.exec("CREATE TABLE " + TBL_NODES_STATS + "("
               + "statid INTEGER PRIMARY KEY AUTOINCREMENT,"
               + "nodeid INTEGER,"
               + "tstep INTEGER,"
               + "cumftime REAL,"
               + "cumsweptarea REAL,"
               + "cumcatches REAL,"
               + "totpop REAL,"
               + "totpopw REAL,"
               + "tariffs REAL"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (!mDb.tables().contains(TBL_POPNODES_STATS)) {
        QSqlQuery q(mDb);
        bool r =
        q.exec("CREATE TABLE " + TBL_POPNODES_STATS + "("
               + "statid INTEGER,"
               + "tstep INTEGER,"
               + "nodeid INTEGER,"
               + "popid INTEGER,"
               + "pop REAL,"
               + "popw REAL,"
               + "impact REAL,"
               + "cumcatchesthispop REAL"
               + ");");
        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }


    if (!mDb.tables().contains(TBL_BENTHOSPOPNODES_STATS)) {
        QSqlQuery q(mDb);
        bool r =
        q.exec("CREATE TABLE " + TBL_BENTHOSPOPNODES_STATS + "("
               + "statid INTEGER,"
               + "tstep INTEGER,"
               + "nodeid INTEGER,"
               + "funcid INTEGER,"
               + "benthosbiomass REAL,"
               + "benthosnumber REAL"
               + ");");
        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (version < 2) {

    }

    return true;

}

bool DbHelper::checkVesselsPosTable(int version)
{
    if (!mDb.tables().contains(TBL_VESSELS_POS)) {
        QSqlQuery q(mDb);
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
        QSqlQuery q(mDb);
        bool r =
        q.exec("CREATE TABLE " + TBL_POP_STATS + "("
               + "statid INTEGER AUTO_INCREMENT PRIMARY KEY,"
               + "tstep INTEGER,"
               + "popid INTEGER,"
               + "N REAL,"
               + "F REAL,"
               + "SSB REAL"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }
    if (!mDb.tables().contains(TBL_POPSZ_STATS)) {
        QSqlQuery q(mDb);
        bool r =
        q.exec("CREATE TABLE " + TBL_POPSZ_STATS + "("
               + "statid INTEGER AUTO_INCREMENT PRIMARY KEY,"
               + "tstep INTEGER,"
               + "popid INTEGER,"
               + "szgroup INTEGER,"
               + "N REAL,"
               + "F REAL,"
               + "SSB REAL"
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
        QSqlQuery q(mDb);
        r =
        q.exec("CREATE TABLE " + TBL_VESSELS + "("
               + "_id INTEGER PRIMARY KEY,"
               + "name VARCHAR(16),"
               + "node INTEGER"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (!mDb.tables().contains(TBL_VESSELS_STATS_TM)) {
        QSqlQuery q(mDb);
        r =
        q.exec("CREATE TABLE " + TBL_VESSELS_STATS_TM + "("
               + "_id INTEGER PRIMARY KEY,"
               + "tstep INTEGER,"
               + "vid INTEGER,"
               + "timeatsea INTEGER,"
               + "harbour INTEGER,"
               + "reason INTEGER,"
               + "revenue_av REAL,"
               + "explicit_revenue_av REAL,"
               + "cumfuel REAL,"
               + "fuelcost REAL,"
               + "gav REAL"
               + ");"
               );

        Q_ASSERT_X(r, __FUNCTION__, q.lastError().text().toStdString().c_str());
    }

    if (!mDb.tables().contains(TBL_VESSELS_STATS_TMSZ)) {
        QSqlQuery q(mDb);
        r =
        q.exec("CREATE TABLE " + TBL_VESSELS_STATS_TMSZ + "("
               + "_id INTEGER PRIMARY KEY,"
               + "tstep INTEGER,"
               + "vid INTEGER,"
               + "harbour INTEGER,"
               + "sz INTEGER,"
               + "cum REAL"
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
      mDb(db),
      mVesselInsertionQuery(0),
      mFlushCount(10000),
      mCounter(0),
      mLastStep(-1)
{
    mVesselInsertionQuery = new QSqlQuery(*mDb);
    mVesselInsertionQuery->prepare(
                "INSERT INTO " + DbHelper::TBL_VESSELS_POS
                + "(vesselid,tstep,x,y,course,fuel,state) VALUES (?,?,?,?,?,?,?)"
                );
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

    bool r = mVesselInsertionQuery->exec();
    DB_ASSERT(r,*mVesselInsertionQuery);
}

void VesselPositionInserter::flush()
{
    mHelper->forceEndTransaction();
    mCounter = 0;
}
