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

#include "scenario.h"
#include <readdata.h>
#include <exceptions.h>
#include <options.h>
#include <comstructs.h>

#include <QFile>
#include <QTextStream>

#include <sstream>
#include <vector>
#include <algorithm>

Scenario::Scenario()
    : dyn_alloc_sce("baseline"),
      dyn_pop_sce("baseline"),
      biolsce("1"),
      freq_do_growth(4),
      graph(0),
      nrow_coord(0),
      nrow_graph(0),
      a_port(0),
      graph_res(10),
      is_individual_vessel_quotas(0),
      tariff_pop(0),
      freq_update_tariff_code(0),
      arbitary_breaks_for_tariff(0),
      total_amount_credited(0)

{
}

QStringList Scenario::getDyn_alloc_sce() const
{
    return dyn_alloc_sce;
}

std::vector<string> Scenario::getDyn_alloc_sce_asVector() const
{
    std::vector<string> vec;
    foreach (QString s, dyn_alloc_sce)
        vec.push_back(s.toStdString());
    return vec;
}

void Scenario::setDyn_alloc_sce(const QStringList &value)
{
    dyn_alloc_sce = value;
}

QStringList Scenario::getDyn_pop_sce() const
{
    return dyn_pop_sce;
}

std::vector<string> Scenario::getDyn_pop_sce_asVector() const
{
    std::vector<string> vec;
    foreach (QString s, dyn_pop_sce)
        vec.push_back(s.toStdString());
    return vec;
}

void Scenario::setDyn_pop_sce(const QStringList &value)
{
    dyn_pop_sce = value;
}

QString Scenario::getBiolsce() const
{
    return biolsce;
}

void Scenario::setBiolsce(const QString &value)
{
    biolsce = value;
}

void Scenario::setFreqDoGrowth(int value)
{
    freq_do_growth = value;
}

int Scenario::getFreqDoGrowth() const
{
return freq_do_growth;
}


int Scenario::getGraph() const
{
    return graph;
}

void Scenario::setGraph(int value)
{
    graph = value;
}

int Scenario::getNrow_coord() const
{
    return nrow_coord;
}

void Scenario::setNrow_coord(int value)
{
    nrow_coord = value;
}

int Scenario::getNrow_graph() const
{
    return nrow_graph;
}

void Scenario::setNrow_graph(int value)
{
    nrow_graph = value;
}

int Scenario::getA_port() const
{
    return a_port;
}

void Scenario::setA_port(int value)
{
    a_port = value;
}

double Scenario::getGraph_res() const
{
    return graph_res;
}

void Scenario::setGraph_res(double value)
{
    graph_res = value;
}

bool Scenario::getIs_individual_vessel_quotas() const
{
    return is_individual_vessel_quotas;
}

void Scenario::setIs_individual_vessel_quotas(bool value)
{
    is_individual_vessel_quotas = value;
}


void Scenario::setTariffPop(const QStringList & value)
{
    tariff_pop = value;
}

vector<int> Scenario::getTariffPop_asVector() const
{
    std::vector<int> vec;
    foreach (QString s, tariff_pop)
        vec.push_back(s.toInt());
    return vec;
}

void Scenario::setFreqUpdateTariffCode(int value)
{
    freq_update_tariff_code = value;
}

int Scenario::getFreqUpdateTariffCode() const
{
return freq_update_tariff_code;
}


void Scenario::setArbitraryBreaksForTariff(const QStringList &value)
{
    arbitary_breaks_for_tariff = value;
}

vector<double> Scenario::getArbitraryBreaksForTariff_asVector() const
{
    std::vector<double> vec;
    foreach (QString s, arbitary_breaks_for_tariff)
        vec.push_back(s.toDouble());
    return vec;
}


void Scenario::setTotalAmountCredited(int value)
{
    total_amount_credited = value;
}

int Scenario::getTotalAmountCredited() const
{
return total_amount_credited;
}




bool Scenario::save(QString path, QString modelname, QString outputname, QString *error)
{
    QString realpath = path + "/simusspe_" + modelname +"/" + outputname + ".dat";
    QFile file (realpath);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        if (error) *error = file.errorString();
        return false;
    }

    QTextStream stream (&file);

    stream << "# dyn_alloc_sce"<< endl;
    foreach (QString a, dyn_alloc_sce)
        stream << a << " ";
    stream << endl;

    stream <<"# dyn_pop_sce"<< endl;
    foreach (QString a, dyn_pop_sce)
        stream << a << " ";
    stream << endl;

    stream << "# biolsce\n" << biolsce << endl;
    stream << "# a_graph\n" << graph << endl;
    stream << "# nrow_coord\n" << nrow_coord << endl;
    stream << "# nrow_graph\n" << nrow_graph << endl;
    stream << "# a_port\n" << a_port << endl;
    stream << "# grid res km\n" << graph_res << endl;
    stream << "# is_individual_vessel_quotas\n" << is_individual_vessel_quotas << endl;

    stream << "# Go Fishing DTree\n" << mDtGoFishing << endl;
    stream << "# Choose Ground DTree\n" << mDtChooseGround<< endl;
    stream << "# Start Fishing DTree\n" << mDtStartFishing<< endl;
    stream << "# Change Ground DTree\n" << mDtChangeGround<< endl;
    stream << "# Stop Fishing DTree\n" << mDtStopFishing<< endl;
    stream << "# Change Port DTree\n" << mDtChangePort<< endl;

    stream << "# Use Dtrees\n" << (use_dtrees ? "1" : "0") << endl;
    file.close();
    return true;
}

Scenario Scenario::readFromFile(QString path, QString modelname, QString outputname)
{
    displace::commons::Scenario scenario;

    if (read_scenario_config_file (        
                path.toStdString(),
                modelname.toStdString(),
                outputname.toStdString(),
                scenario) < 0)
        throw DisplaceException(QString(QObject::tr("Cannot load scenario file: %1 - %2"))
                                .arg(::getLastErrorMessage().c_str())
                                .arg(strerror(errno)));

    Scenario s;

    QStringList alsce = QString::fromStdString(scenario.dyn_alloc_sce.toString()).split(" ", QString::SkipEmptyParts);
    QStringList popsce = QString::fromStdString(scenario.dyn_pop_sce.toString()).split(" ", QString::SkipEmptyParts);

    s.setDyn_alloc_sce(alsce);
    s.setDyn_pop_sce(popsce);
    s.setBiolsce(QString(scenario.biolsce.c_str()));
    s.setFreqDoGrowth(scenario.freq_do_growth);
    s.setGraph(scenario.a_graph);
    s.setNrow_coord(scenario.nrow_coord);
    s.setNrow_graph(scenario.nrow_graph);
    if (scenario.graph_res > 1e-3)
        s.setGraph_res(scenario.graph_res);
    s.setA_port(scenario.a_port);

    s.setDtGoFishing(QString::fromStdString(scenario.dt_go_fishing));
    s.setDtChooseGround(QString::fromStdString(scenario.dt_choose_ground));
    s.setDtStartFishing(QString::fromStdString(scenario.dt_start_fishing));
    s.setDtChangeGround(QString::fromStdString(scenario.dt_change_ground));
    s.setDtStopFishing(QString::fromStdString(scenario.dt_stop_fishing));
    s.setDtChangePort(QString::fromStdString(scenario.dt_change_port));
    s.setIs_individual_vessel_quotas(scenario.is_individual_vessel_quotas);
    s.setDtreesEnabled(scenario.use_dtrees);

    std::stringstream tariff;
    std::copy(scenario.tariff_pop.begin(), scenario.tariff_pop.end(), std::ostream_iterator<int>(tariff, " "));
    QStringList tariff_pops = QString::fromStdString(tariff.str().c_str()).split(" ", QString::SkipEmptyParts);

    std::stringstream breaks_for_tariff;
    std::copy(scenario.arbitary_breaks_for_tariff.begin(), scenario.arbitary_breaks_for_tariff.end(), std::ostream_iterator<int>(breaks_for_tariff, " "));
    QStringList breaks_for_tariffs = QString::fromStdString(breaks_for_tariff.str().c_str()).split(" ", QString::SkipEmptyParts);

    s.setTariffPop(tariff_pops);
    s.setFreqUpdateTariffCode(scenario.freq_update_tariff_code);
    s.setArbitraryBreaksForTariff(breaks_for_tariffs);



    return s;
}

QString Scenario::getDtGoFishing() const
{
    return mDtGoFishing;
}

void Scenario::setDtGoFishing(const QString &dtGoFishing)
{
    mDtGoFishing = dtGoFishing;
}

QString Scenario::getDtChooseGround() const
{
    return mDtChooseGround;
}

void Scenario::setDtChooseGround(const QString &dtChooseGround)
{
    mDtChooseGround = dtChooseGround;
}

QString Scenario::getDtStartFishing() const
{
    return mDtStartFishing;
}

void Scenario::setDtStartFishing(const QString &dtStartFishing)
{
    mDtStartFishing = dtStartFishing;
}

QString Scenario::getDtChangeGround() const
{
    return mDtChangeGround;
}

void Scenario::setDtChangeGround(const QString &dtChangeGround)
{
    mDtChangeGround = dtChangeGround;
}

QString Scenario::getDtStopFishing() const
{
    return mDtStopFishing;
}

void Scenario::setDtStopFishing(const QString &dtStopFishing)
{
    mDtStopFishing = dtStopFishing;
}

QString Scenario::getDtChangePort() const
{
    return mDtChangePort;
}

void Scenario::setDtChangePort(const QString &dtChangePort)
{
    mDtChangePort = dtChangePort;
}
