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

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef DBHELPER_H
#define DBHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QThread>
#include <QMutex>

#include <memory>

class SQLiteOutputStorage;

class DisplaceModel;
class NodeData;
class HarbourData;
class VesselData;
class VesselStats;
class PopulationData;
class NationStats;
class HarbourStats;
class DbHelper;
class Config;
class Scenario;

#if 0
class VesselPositionInserter : public QObject {
    Q_OBJECT

    friend class DbHelper;

    DbHelper *mHelper;
    QSqlDatabase *mDb;
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
#endif

class DbHelper : public QObject
{
    struct Impl;
    std::unique_ptr<Impl> p;

    Q_OBJECT

#if 0
    friend class VesselPositionInserter;
#endif

public:
    DbHelper();
    ~DbHelper() noexcept;

    bool attachDb(std::shared_ptr<SQLiteOutputStorage> storage);
    QString lastDbError() const;

    void Q_DECL_DEPRECATED addNodesDetails(int idx, std::shared_ptr<NodeData> node);
    void Q_DECL_DEPRECATED removeAllNodesDetails();

    void Q_DECL_DEPRECATED addNodesStats (int tstep, const QList<std::shared_ptr<NodeData> > &nodes);
    void Q_DECL_DEPRECATED addPopStats(int tstep, const QVector<PopulationData> &pops);
    void Q_DECL_DEPRECATED addNationsStats(int tstep, const QVector<NationStats> &nats);
    void Q_DECL_DEPRECATED addVesselStats(int tstep, const VesselData &vessel, const VesselStats &stats);

    void Q_DECL_DEPRECATED addVesselPosition (int step, int idx, std::shared_ptr<VesselData> vessel);
    void Q_DECL_DEPRECATED removeAllVesselsDetails();
    void Q_DECL_DEPRECATED addVesselDetails (int idx, std::shared_ptr<VesselData> vessel);

    void Q_DECL_DEPRECATED removeAllStatsData();

    bool Q_DECL_DEPRECATED loadConfig(Config &);
    bool Q_DECL_DEPRECATED saveConfig (const Config &);
    bool Q_DECL_DEPRECATED loadScenario (Scenario &);
    bool Q_DECL_DEPRECATED saveScenario (const Scenario &);

    bool loadNodes(QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<HarbourData> > &harbours, DisplaceModel *model);
    bool loadVessels(const QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<VesselData> > &vessels);

    /* Update datas for step */
    bool updateVesselsToStep(int steps, QList<std::shared_ptr<VesselData> > &vessels);
    bool updateStatsForNodesToStep(int step, QList<std::shared_ptr<NodeData> > &nodes);
    bool loadHistoricalStatsForPops(QList<int> &steps, QList<QVector<PopulationData> > &population);
    bool loadHistoricalStatsForVessels(const QList<int> &steps, const QList<std::shared_ptr<VesselData> > &vessels, const QList<std::shared_ptr<NodeData> > &nodes, QList<QVector<NationStats> > &nations, QList<QVector<HarbourStats> > &harbour) ;

    HarbourStats getHarbourStatsAtStep(int idx, int step);

    void Q_DECL_DEPRECATED beginTransaction();
    void Q_DECL_DEPRECATED endTransaction();
    void Q_DECL_DEPRECATED forceEndTransaction();
    void Q_DECL_DEPRECATED flushBuffers();

    /* Creates all indexes on db - to be called at the end of simulation */
    void Q_DECL_DEPRECATED createIndexes();

    /* Metadata */

    void setMetadata (QString key, QString value);
    QString getMetadata (QString key);

    int getLastKnownStep();
signals:
    void Q_DECL_DEPRECATED postVesselInsertion (int step, int idx , double x, double y, double course, double fuel, int state);
    void Q_DECL_DEPRECATED flush();

protected:
    void Q_DECL_DEPRECATED createIndexOnTstepForTable(QString table);

private:
    QMutex mMutex;
//    VesselPositionInserter *mInserter;
//    QThread *mInsertThread;
};

#endif // DBHELPER_H
