// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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
#include "utils/safe_strerror.h"

#include <QFile>
#include <QTextStream>

#include <sstream>
#include <vector>
#include <algorithm>

#include <QDebug>

Scenario::Scenario()
    : dyn_alloc_sce("baseline"),
      dyn_pop_sce("baseline"),
      biolsce("1"),
      fleetsce("2"),
      freq_do_growth(4),
      freq_redispatch_the_pop(3),
      graph(0),
      nrow_coord(0),
      nrow_graph(0),
      a_port(0),
      graph_res(0),
      is_individual_vessel_quotas(0),
      check_all_stocks_before_going_fishing(0),
      tariff_pop(0),
      freq_update_tariff_code(0),
      arbitary_breaks_for_tariff(0),
      met_multiplier_on_arbitary_breaks_for_tariff(0),
      total_amount_credited(0),
      tariff_annual_hcr_percent_change(0.0),
      update_tariffs_based_on_lpue_or_dpue_code(1)

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

QString Scenario::getFleetsce() const
{
    return fleetsce;
}

void Scenario::setFleetsce(const QString &value)
{
    fleetsce = value;
}

void Scenario::setFreqDoGrowth(int value)
{
    freq_do_growth = value;
}

int Scenario::getFreqDoGrowth() const
{
return freq_do_growth;
}

void Scenario::setFreqDispatchThePop(int value)
{
    freq_redispatch_the_pop = value;
}

int Scenario::getFreqDispatchThePop() const
{
return freq_redispatch_the_pop;
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

types::NodeId Scenario::getA_port() const
{
    return a_port;
}

void Scenario::setA_port(types::NodeId value)
{
    a_port = value;
}

QStringList Scenario::getGraph_res() const
{
    return graph_res;
}

std::vector<double> Scenario::getGraph_res_asVector() const
{
    std::vector<double> vec;
    foreach (QString s, graph_res)
        vec.push_back(s.toDouble());
    return vec;
}

void Scenario::setGraph_res(const QStringList &value)
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

bool Scenario::getIs_check_all_stocks_before_going_fishing() const
{
    return check_all_stocks_before_going_fishing;
}

void Scenario::setIs_check_all_stocks_before_going_fishing(bool value)
{
    check_all_stocks_before_going_fishing = value;
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


void Scenario::setUpdateTariffsBasedOnLpueOrDpueCode(int value)
{
    update_tariffs_based_on_lpue_or_dpue_code = value;
}

int Scenario::getUpdateTariffsBasedOnLpueOrDpueCode() const
{
return update_tariffs_based_on_lpue_or_dpue_code;
}


void Scenario::setArbitraryBreaksForTariff(const QStringList &value)
{
    arbitary_breaks_for_tariff = value;
}

void Scenario::setMetMultiplierOnArbitraryBreaksForTariff(const QStringList& value)
{
    met_multiplier_on_arbitary_breaks_for_tariff = value;
}


vector<double> Scenario::getArbitraryBreaksForTariff_asVector() const
{
    std::vector<double> vec;
    foreach (QString s, arbitary_breaks_for_tariff)
        vec.push_back(s.toDouble());
    return vec;
}

vector<double> Scenario::getMetMultiplierOnArbitraryBreaksForTariff_asVector() const
{
    std::vector<double> vec;
    foreach(QString s, met_multiplier_on_arbitary_breaks_for_tariff)
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

void Scenario::setTariffAnnualHcrPercentChange(double value)
{
    tariff_annual_hcr_percent_change = value;
}

double Scenario::getTariffAnnualHcrPercentChange() const
{
return tariff_annual_hcr_percent_change;
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
    stream << "# fleetsce\n" << fleetsce << endl;
    stream << "# Frequency to apply growth (0:daily; 1:weekly; 2:monthly; 3:quarterly; 4:semester)\n" << freq_do_growth << endl;
    stream << "# Frequency to redispatch the pop (0:daily; 1:weekly; 2:monthly; 3:quarterly; 4:semester)\n" << freq_redispatch_the_pop << endl;
    stream << "# a_graph\n" << graph << endl;
    stream << "# nrow_coord\n" << nrow_coord << endl;
    stream << "# nrow_graph\n" << nrow_graph << endl;
    stream << "# a_port\n" << a_port.toIndex() << endl;
    stream << "# grid res km\n" << endl;
    foreach (QString a, graph_res)
        stream << a << " ";
    stream << "# is_individual_vessel_quotas\n" << is_individual_vessel_quotas << endl;
    stream << "#  check all stocks before going fishing (otherwise, explicit pops only)\n" << check_all_stocks_before_going_fishing << endl;

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

	// TODO pass the correct db when it will be available.
    if (!read_scenario_config_file(nullptr,
                                   path.toStdString(),
                                   modelname.toStdString(),
                                   outputname.toStdString(),
                                   scenario)) {
        throw DisplaceException(QString(QObject::tr("Cannot load scenario file: %1 - %2"))
                                        .arg(::getLastErrorMessage().c_str())
                                        .arg(safe_strerror(errno).c_str()));
    }

    Scenario s;

    QStringList alsce = QString::fromStdString(scenario.dyn_alloc_sce.toString()).split(" ", QString::SkipEmptyParts);
    QStringList popsce = QString::fromStdString(scenario.dyn_pop_sce.toString()).split(" ", QString::SkipEmptyParts);


    s.setDyn_alloc_sce(alsce);
    s.setDyn_pop_sce(popsce);
    s.setBiolsce(QString(scenario.biolsce.c_str()));
    s.setFleetsce(QString(scenario.fleetsce.c_str()));
    s.setFreqDoGrowth(scenario.freq_do_growth);
    s.setFreqDispatchThePop(scenario.freq_redispatch_the_pop);
    s.setGraph(scenario.a_graph);
    s.setNrow_coord(scenario.nrow_coord);
    s.setNrow_graph(scenario.nrow_graph);
    std::stringstream graphres;
    std::copy(scenario.graph_res.begin(), scenario.graph_res.end(), std::ostream_iterator<int>(graphres, " "));
    QStringList graphresxy = QString::fromStdString(graphres.str().c_str()).split(" ", QString::SkipEmptyParts);
    s.setGraph_res(graphresxy);
    s.setA_port(scenario.a_port);

    s.setDtGoFishing(QString::fromStdString(scenario.dt_go_fishing));
    s.setDtChooseGround(QString::fromStdString(scenario.dt_choose_ground));
    s.setDtStartFishing(QString::fromStdString(scenario.dt_start_fishing));
    s.setDtChangeGround(QString::fromStdString(scenario.dt_change_ground));
    s.setDtStopFishing(QString::fromStdString(scenario.dt_stop_fishing));
    s.setDtChangePort(QString::fromStdString(scenario.dt_change_port));
    s.setIs_individual_vessel_quotas(scenario.is_individual_vessel_quotas);
    s.setIs_check_all_stocks_before_going_fishing(scenario.check_all_stocks_before_going_fishing);
    s.setDtreesEnabled(scenario.use_dtrees);

    std::stringstream tariff;
    std::copy(scenario.tariff_pop.begin(), scenario.tariff_pop.end(), std::ostream_iterator<int>(tariff, " "));
    QStringList tariff_pops = QString::fromStdString(tariff.str().c_str()).split(" ", QString::SkipEmptyParts);

    std::stringstream breaks_for_tariff;
    std::copy(scenario.arbitary_breaks_for_tariff.begin(), scenario.arbitary_breaks_for_tariff.end(), std::ostream_iterator<int>(breaks_for_tariff, " "));
    std::stringstream met_multiplier_on_breaks_for_tariff;
    std::copy(scenario.met_multiplier_on_arbitary_breaks_for_tariff.begin(), scenario.met_multiplier_on_arbitary_breaks_for_tariff.end(), std::ostream_iterator<int>(met_multiplier_on_breaks_for_tariff, " "));

    QStringList breaks_for_tariffs = QString::fromStdString(breaks_for_tariff.str().c_str()).split(" ", QString::SkipEmptyParts);

    s.setTariffPop(tariff_pops);
    s.setFreqUpdateTariffCode(scenario.freq_update_tariff_code);
    s.setUpdateTariffsBasedOnLpueOrDpueCode (scenario.update_tariffs_based_on_lpue_or_dpue_code);
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
