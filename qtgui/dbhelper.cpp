// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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

#include <exceptions.h>
#include <config.h>
#include <scenario.h>

#include <modelobjects/nodedata.h>
#include <modelobjects/vesseldata.h>
#include <Harbour.h>

#include <displacemodel.h>

#include <profiler.h>

#include <storage/sqliteoutputstorage.h>
#include <storage/tables/nodesdeftable.h>
#include <storage/tables/popstattable.h>
#include <storage/tables/metadatatable.h>
#include <storage/tables/vesseldeftable.h>
#include <storage/tables/vesselvmsliketable.h>
#include <storage/tables/vesselvmslikefpingsonlytable.h>
#include <storage/tables/poptable.h>
#include <storage/tables/funcgroupstable.h>
#include <sqlitestorage.h>
#include <storage/modelmetadataaccessor.h>

#include <utils/make_unique.h>

#include <QStringList>
#include <QDebug>

struct DbHelper::Impl
{
    std::shared_ptr<SQLiteOutputStorage> db;
    std::unique_ptr<ModelMetadataAccessor> accessor;

    QString lastError;

    void catchException (sqlite::SQLiteException &ex) {
        lastError = ex.what();
    }
};

DbHelper::DbHelper()
{
}

DbHelper::~DbHelper() noexcept = default;

bool DbHelper::attachDb(std::shared_ptr<SQLiteOutputStorage> storage)
{
    // first reset yourself
    p = utils::make_unique<Impl>();
    p->db = storage;
    p->accessor = utils::make_unique<ModelMetadataAccessor>(p->db->metadata());

    return true;
}

QString DbHelper::lastDbError() const
{
    return p->lastError;
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

    QList<types::NodeId> ihbs;
    lsi = getMetadata("config::int_harbours").split(" ");
    foreach (QString i, lsi) {
        ihbs.push_back(types::NodeId(i.toInt()));
    }
    cfg.m_interesting_harbours = ihbs;

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
    auto ih = cfg.m_interesting_harbours;
    foreach (auto d, ih)
        str.push_back(QString::number(d.toIndex()));
    setMetadata("config::int_harbours", str.join(" "));

    return true;
}

bool DbHelper::loadScenario(Scenario &sce)
{
    sce.setGraph(getMetadata("sce::graph").toInt());
    sce.setNrow_graph(getMetadata("sce::nrow_graph").toInt());
    sce.setNrow_coord(getMetadata("sce::nrow_coord").toInt());
    sce.setA_port(types::NodeId(getMetadata("sce::aport").toInt()));
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
    setMetadata("sce::aport", QString::number(sce.getA_port().toIndex()));
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
    auto nodesTab = p->db->getNodesDefTable();

    nodesTab->queryAllNodes([&nodes, &model, &harbours](std::shared_ptr<Node> nodeData, std::shared_ptr<Harbour> harbour) {
        auto n = std::make_shared<NodeData>(nodeData, model);

        auto idx = n->get_idx_node().toIndex();
        while (nodes.size() < idx+1)
            nodes.push_back(0);
        nodes[idx] = n;

        if (nodeData->get_is_harbour() && harbour != nullptr) {
            harbours.push_back(std::make_shared<HarbourData>(harbour));
        }
    });

    return true;
}

bool DbHelper::loadVessels(const QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<VesselData> > &vessels)
{
    auto vtab = p->db->getVesselDefTable();
    vtab->queryAllVessels(
                [&nodes](int id) {
        return nodes[id]->mNode.get();
    },
                [&vessels](std::shared_ptr<Vessel>v) {
        auto idx = v->get_idx();
        while (vessels.size() < idx+1)
            vessels.push_back(0);

        vessels[idx] = std::make_shared<VesselData>(v);
        return true;
    });

    return true;
}

bool DbHelper::updateVesselsToStep(int tstep, QList<std::shared_ptr<VesselData> > &vessels)
{
    auto vtab = p->db->getVesselVmsLikeTable();
    vtab->queryAllVesselsAtStep (tstep, [&vessels](const VesselVmsLikeTable::Log & log){
        if (log.id < vessels.size()) {
            std::shared_ptr<VesselData> v (vessels.at(log.id));
            v->mVessel->set_xy(log.p_long,log.p_lat);
            v->mVessel->set_fuelcons(log.cum_fuel);
            v->mVessel->set_state(log.state);
            //v->mVessel->set_cumcatches(...);
            //v->mVessel->set_timeatsea(...);
            //v->mVessel->set_reason_to_go_back(..);
            v->mVessel->set_course(log.p_course);
        }

        return true;
    });

    return true;
}

bool DbHelper::updateStatsForNodesToStep(int step, QList<std::shared_ptr<NodeData> > &nodes)
{
    auto ntab = p->db->getNodesStatTable();

    ntab->queryAllNodesAtStep (step, [&nodes](const NodesStatTable::NodeStat &stat){
        auto nid = stat.nodeId.toIndex();
        auto &node = nodes.at(nid);
        node->set_cumftime(stat.cumftime);
        node->set_cumsweptarea(stat.cumswa);
        node->set_cumsubsurfacesweptarea(stat.cumsubsurfswa);
        node->set_cumcatches(stat.cumcatches);
        node->set_cumdiscards(stat.cumdisc);
        //nodes.at(nid)->setPopTot(tot);
        //nodes.at(nid)->setPopWTot(totw);
        //nodes.at(nid)->set_tariffs(tariffs);
        return true;
    });

    auto ptab = p->db->getPopTable();
    ptab->queryAllNodesAtStep (step, [&nodes](const PopTable::Stat &stat) {
        auto nid = stat.nodeId.toIndex();
        if (nid < nodes.size()) {
            nodes.at(nid)->setPop(stat.popId,stat.totNid);
            nodes.at(nid)->setPopW(stat.popId, stat.totWid);
            nodes.at(nid)->setImpact(stat.popId, stat.impact);
            nodes.at(nid)->setCumcatchesPerPop(stat.popId, stat.cumC);
            //TODO: Set Discards!
            //nodes.at(nid)->set_cumdiscards(stat.popId, stat.cumD);
        }
        return true;
    });

    auto ftab = p->db->getFuncGroupsTable();
    ftab->queryAllNodesAtStep(step, [&nodes](const FuncGroupsTable::Stat &stat) {
        int nid = stat.nodeId.toIndex();
        if (nid < nodes.size()) {
            nodes.at(nid)->setBenthosBiomass(stat.funcId,stat.bio);
            nodes.at(nid)->setBenthosNumber(stat.funcId,stat.numTot);
            nodes.at(nid)->setBenthosMeanweight(stat.funcId,stat.bioMean);
            nodes.at(nid)->setBenthosBiomassOverK(stat.funcId, stat.bioK);
            nodes.at(nid)->setBenthosNumberOverK(stat.funcId, stat.numK);
        }
        return true;
    });

    return true;

#if 0
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

    q.prepare ("SELECT nodeid,funcid,benthosbiomass,benthosnumber,benthosmeanweight FROM " + TBL_BENTHOSPOPNODES_STATS
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
        double benthosmeanweight = q.value(4).toDouble();
        if (nid < nodes.size()) {
            nodes.at(nid)->setBenthosMeanweight(funcid,benthosmeanweight);
        }
    }
#endif

    return true;
}

bool DbHelper::loadHistoricalStatsForPops(QList<int> &steps, QList<QVector<PopulationData> > &population)
{
    Q_UNUSED(steps); Q_UNUSED(population);

#if 0
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
#endif

    return true;
}

bool DbHelper::loadHistoricalStatsForVessels(const QList<int> &steps, const QList<std::shared_ptr<VesselData> > &vessels,
                                             const QList<std::shared_ptr<NodeData> >&nodes,
                                             QList<QVector<NationStats> > &nations,
                                             QList<QVector<HarbourStats> > &harbour)
{
    Q_UNUSED(steps); Q_UNUSED(vessels);
    Q_UNUSED(nodes); Q_UNUSED(nations);
    Q_UNUSED(harbour);

#if 0
    QSqlQuery q(mDb);
    bool res = q.prepare("SELECT vid,sz,SUM(cum),SUM(cumdisc),harbour FROM "+ TBL_VESSELS_STATS_TMSZ + " WHERE tstep<=? GROUP BY vid,sz"); /*,harbour  */
    DB_ASSERT(res,q);

    QSqlQuery q2(mDb);
    res = q2.prepare("SELECT vid,SUM(timeatsea),SUM(revenue_av),SUM(explicit_revenue_av),harbour,SUM(gav),SUM(vpuf) FROM " + TBL_VESSELS_STATS_TM + " WHERE tstep<=? GROUP BY vid");
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
            int discards = q.value(3).toDouble();
            int hid = q.value(4).toInt();
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
            curnationsdata[nid].mTotDiscards += discards;
            curHarbourData[hidx].mCumDiscards += discards;
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
            double sweptarea = q2.value(7).toDouble();
            double revenuepersweptarea = q2.value(8).toDouble();

            while (curnationsdata.size() <= nid)
                curnationsdata.push_back(NationStats());
            while (curHarbourData.size() <= hidx)
                curHarbourData.push_back(HarbourStats());

            curnationsdata[nid].mRevenues += rev;
            curnationsdata[nid].mExRevenues += exrev;
            curnationsdata[nid].mTimeAtSea += timeatsea;
            curnationsdata[nid].mGav += gav;
            curnationsdata[nid].mVpuf = vpuf;
            curnationsdata[nid].mSweptArea = sweptarea;
            curnationsdata[nid].mRevenuePerSweptArea = revenuepersweptarea;
            //curHarbourData[hidx].mCumCatches += catches;
            curHarbourData[hidx].mCumProfit += rev;
            curHarbourData[hidx].mGav += gav;
            curHarbourData[hidx].mVpuf = vpuf;
            curHarbourData[hidx].mSweptArea = sweptarea;
            curHarbourData[hidx].mRevenuePerSweptArea = revenuepersweptarea;
            
        }

        nations.push_back(curnationsdata);
        harbour.push_back(curHarbourData);
    }
#endif

    return true;
}

HarbourStats DbHelper::getHarbourStatsAtStep(int idx, int step)
{
    Q_UNUSED(idx); Q_UNUSED(step);
#if 0
    QSqlQuery q(mDb);
    bool res = q.prepare("SELECT sz,SUM(cum),SUM(cumdisc) FROM "+ TBL_VESSELS_STATS_TMSZ + " WHERE tstep<=? AND harbour=? GROUP BY vid,sz");
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
        int discards = q.value(2).toDouble();

        QVector<double> &g = curHarbourData.szCatches; // alias
        while (g.size() <= sz)
            g.push_back(0.0);
        g[sz] += catches;

        curHarbourData.mCumCatches += catches;
        curHarbourData.mCumDiscards += discards;
    }

    res = q2.exec();
    DB_ASSERT(res,q2);
    while (q2.next()) {
        // double timeatsea = q2.value(0).toDouble();
        double rev = q2.value(1).toDouble();

        curHarbourData.mCumProfit += rev;
        curHarbourData.mVpuf += q2.value(2).toDouble();
        curHarbourData.mGav += q2.value(3).toDouble();
        curHarbourData.mSweptArea += q2.value(4).toDouble();
        curHarbourData.mRevenuePerSweptArea += q2.value(5).toDouble();
    }

    return curHarbourData;
#endif

    return HarbourStats();
}

void DbHelper::setMetadata(QString key, QString value)
{
    try {
        p->db->metadata()->setMetadata(key.toStdString(), value.toStdString());
    } catch (sqlite::SQLiteException &x) {
        p->catchException(x);
    }
}

QString DbHelper::getMetadata(QString key)
{
    try {
        return QString::fromStdString(p->db->metadata()->getMetadata(key.toStdString()));
    } catch (sqlite::SQLiteException &x) {
        p->catchException(x);
    }

    return QString();
}

int DbHelper::getLastKnownStep()
{
    return p->accessor->lastTStep();
}
