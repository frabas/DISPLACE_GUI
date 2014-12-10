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

QT_BEGIN_NAMESPACE
class QSqlQuery;
QT_END_NAMESPACE

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

class DbHelper : public QObject
{
    Q_OBJECT

    friend class VesselPositionInserter;

    QSqlDatabase mDb;
public:
    DbHelper();
    ~DbHelper();

    bool attachDb(QString file);
    QString lastDbError() const;

    void addNodesDetails(int idx, std::shared_ptr<NodeData> node);
    void removeAllNodesDetails();

    void addNodesStats (int tstep, const QList<std::shared_ptr<NodeData> > &nodes);
    void addPopStats(int tstep, const QVector<PopulationData> &pops);
    void addNationsStats(int tstep, const QVector<NationStats> &nats);
    void addVesselStats(int tstep, const VesselData &vessel, const VesselStats &stats);

    void addVesselPosition (int step, int idx, std::shared_ptr<VesselData> vessel);
    void removeAllVesselsDetails();
    void addVesselDetails (int idx, std::shared_ptr<VesselData> vessel);

    void removeAllStatsData();

    bool loadConfig(Config &);
    bool saveConfig (const Config &);
    bool loadScenario (Scenario &);
    bool saveScenario (const Scenario &);

    bool loadNodes(QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<HarbourData> > &harbours, DisplaceModel *model);
    bool loadVessels(const QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<VesselData> > &vessels);

    /* Update datas for step */
    bool updateVesselsToStep(int steps, QList<std::shared_ptr<VesselData> > &vessels);
    bool updateStatsForNodesToStep(int step, QList<std::shared_ptr<NodeData> > &nodes);
    bool loadHistoricalStatsForPops(QList<int> &steps, QList<QVector<PopulationData> > &population);
    bool loadHistoricalStatsForVessels(const QList<int> &steps, const QList<std::shared_ptr<VesselData> > &vessels, const QList<std::shared_ptr<NodeData> > &nodes, QList<QVector<NationStats> > &nations, QList<QVector<HarbourStats> > &harbour) ;

    HarbourStats getHarbourStatsAtStep(int idx, int step);

    void beginTransaction();
    void endTransaction();
    void forceEndTransaction();
    void flushBuffers();

    /* Creates all indexes on db - to be called at the end of simulation */
    void createIndexes();

    /* Metadata */

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
    bool checkStatsTable (int version);

    void createIndexOnTstepForTable(QString table);

private:
    int mOngoingTransactionsCount;

    QMutex mMutex;
    VesselPositionInserter *mInserter;
    QThread *mInsertThread;
    int mVersion;

    static const int VERSION;

    static const QString TBL_META;
    static const QString TBL_NODES;
    static const QString TBL_NODES_STATS;
    static const QString TBL_POPNODES_STATS;
    static const QString TBL_POP_STATS;
    static const QString TBL_POPSZ_STATS;
    static const QString TBL_VESSELS;
    static const QString TBL_VESSELS_POS;
    static const QString TBL_VESSELS_STATS_TM;
    static const QString TBL_VESSELS_STATS_TMSZ;

    static const QString META_VERSION;
};

#endif // DBHELPER_H
