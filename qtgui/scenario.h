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
#ifndef SCENARIO_H
#define SCENARIO_H

#include <QString>
#include <QList>
#include <QStringList>

class Scenario
{
private:
    QStringList dyn_alloc_sce;
    QStringList dyn_pop_sce;
    QString biolsce;
    int graph;
    int nrow_coord;
    int nrow_graph;
    int a_port;
    double graph_res;

    QString mDtGoFishing;
    QString mDtChooseGround;
    QString mDtStartFishing;
    QString mDtChangeGround;
    QString mDtStopFishing;
    QString mDtChangePort;

public:
    Scenario();

    QStringList getDyn_alloc_sce() const;
    std::vector<std::string> getDyn_alloc_sce_asVector() const;

    void setDyn_alloc_sce(const QStringList &value);
    QStringList getDyn_pop_sce() const;
    std::vector<std::string> getDyn_pop_sce_asVector() const;

    void setDyn_pop_sce(const QStringList &value);
    QString getBiolsce() const;
    void setBiolsce(const QString &value);
    int getGraph() const;
    void setGraph(int value);
    int getNrow_coord() const;
    void setNrow_coord(int value);
    int getNrow_graph() const;
    void setNrow_graph(int value);
    int getA_port() const;
    void setA_port(int value);
    double getGraph_res() const;
    void setGraph_res(double value);

    bool save (QString path, QString modelname, QString outputname, QString *error);

    static Scenario readFromFile (QString path, QString modelname, QString outputname);

    QString getDtGoFishing() const;
    void setDtGoFishing(const QString &dtGoFishing);
    QString getDtChooseGround() const;
    void setDtChooseGround(const QString &dtChooseGround);
    QString getDtStartFishing() const;
    void setDtStartFishing(const QString &dtStartFishing);
    QString getDtChangeGround() const;
    void setDtChangeGround(const QString &dtChangeGround);
    QString getDtStopFishing() const;
    void setDtStopFishing(const QString &dtStopFishing);
    QString getDtChangePort() const;
    void setDtChangePort(const QString &dtChangePort);
};

#endif // SCENARIO_H
