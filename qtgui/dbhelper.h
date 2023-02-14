// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2023 Francois Bastardie <fba@aqua.dtu.dk>

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
class MetierData;
class VesselStats;
class PopulationData;
class NationStats;
class HarbourStats;
class DbHelper;
class Config;
class Scenario;
class FishfarmData;

class DbHelper : public QObject
{
    struct Impl;
    std::unique_ptr<Impl> p;

    Q_OBJECT

public:
    DbHelper();
    ~DbHelper() noexcept;

    bool attachDb(std::shared_ptr<SQLiteOutputStorage> storage);
    QString lastDbError() const;

    bool Q_DECL_DEPRECATED loadConfig(Config &);
    bool Q_DECL_DEPRECATED saveConfig (const Config &);
    bool Q_DECL_DEPRECATED loadScenario (Scenario &);
    bool Q_DECL_DEPRECATED saveScenario (const Scenario &);

    bool loadNodes(QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<HarbourData> > &harbours, DisplaceModel *model);
    bool loadVessels(const QList<std::shared_ptr<NodeData> > &nodes, QList<std::shared_ptr<VesselData> > &vessels);
    bool loadFishFarms (const QList<std::shared_ptr<NodeData> > &nodes,QList<std::shared_ptr<FishfarmData>> &fishfarms);

    bool loadMetiers(QList<std::shared_ptr<MetierData> >& metiers);

    /* Update datas for step */
    bool updateVesselsToStep(int steps, QList<std::shared_ptr<VesselData> > &vessels);
    bool updateStatsForNodesToStep(int step, QList<std::shared_ptr<NodeData> > &nodes);
    bool updatePopValuesForNodesToStep (int step, QList<std::shared_ptr<NodeData> > &nodes);

    HarbourStats getHarbourStatsAtStep(int idx, int step);

    /* Metadata */

    void setMetadata (QString key, QString value);
    QString getMetadata (QString key);

    int getLastKnownStep();

private:
    QMutex mMutex;
};

#endif // DBHELPER_H
