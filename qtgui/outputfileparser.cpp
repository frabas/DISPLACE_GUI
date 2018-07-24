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

#include "outputfileparser.h"

#include <displacemodel.h>
#include <mainwindow.h>
#include <modelobjects/vesseldata.h>
#include <calendar.h>

#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include <QMessageBox>

OutputFileParser::OutputFileParser(DisplaceModel *model, QObject *parent)
    : QObject(parent),
      mModel(model)
{
}

void OutputFileParser::parse(QString path, int tstep, int period)
{
    QFile file (path);
    QFileInfo info (file);

    QString name = info.fileName();

    mModel->startCollectingStats();

    qDebug() << "Parsing Output file: " << path ;

    if (!file.open(QFile::ReadOnly)) {
        emit error(QString(QObject::tr("The file %1 cannot be read: %2"))
                   .arg(file.fileName())
                   .arg(file.errorString()));
        return;
    }

    if (name.startsWith("popnodes_start_") || name.startsWith("popnodes_inc_")) {
        //parsePopStart(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumftime_")) {
        parsePopCumftime(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumsweptarea_")) {
        parsePopCumsweptarea(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumcatches_with_threshold_")) {
        parsePopCumcatchesWithThreshold(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumcatches_")) {
        parsePopCumcatches(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumdiscardsratio_")) {
        parsePopCumdiscardsratio(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumdiscards_")) {
        parsePopCumdiscards(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_tariffs_")) {
        parsePopTariffs(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_impact_")) {
        //parsePopImpact(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumulcatches_per_pop_")) {
        //parsePopCumcatchesPerPop(&file, tstep, mModel, period);
    } else if (name.startsWith("nodes_envt_")) {
        //parseNodesEnvt(&file, tstep, mModel, period);
    } else if (name.startsWith("benthosnodes_tot_biomasses_")) {
        //parsePopBenthosStats(&file, tstep, mModel, period);
    } else if (name.startsWith("benthosnodes_tot_numbers_")) {
        //parsePopBenthosStats(&file, tstep, mModel, period);
    } else if (name.startsWith("loglike_")) {
        parseVessels(&file, tstep, mModel, period);
    } else if (name.startsWith("fishfarmslogs_")) {
        parseFishfarmslogsStats(&file, tstep, mModel, period);
    } else if (name.startsWith("shipslogs_")) {
        parseShipsStats(&file, tstep, mModel, period);
    } else { /* Don't know how to handle... */
        qDebug() << "File isn't recognized: " << path;
    }

    mModel->endCollectingStats();

    emit parseCompleted();
}


/*
 * PopStart file is a list of space-separated lines containing:
 * nodeid x y population1 population2 ...
 * see Node::export_popnodes for details
 */
void OutputFileParser::parsePopStart(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    QList<double> dataW;
    QList<double> data;
    data.reserve(model->getNBPops());
    for (int i = 0; i < model->getNBPops(); ++i) {
        data.push_back(0.0);
        dataW.push_back(0.0);
    }

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (tstep == -1 || step == tstep) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int id = fields[1].toInt();

            bool ok;
            int i;

            // NOTE: TODO: tot and wtot aren't used.
            double tot = 0.0;
            double wtot = 0.0;
            for (i = 4 ; i < fields.size(); i+=2) {
                data[(i-4) / 2] = fields[i].toDouble(&ok);
                Q_ASSERT(ok);
                dataW[(i-4) / 2] = fields[i+1].toDouble(&ok);
                Q_ASSERT(ok);
            }

            model->collectNodePopStats(tstep, id, data, dataW, tot, wtot);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}

void OutputFileParser::parsePopCumftime(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int id = fields[1].toInt();
            double cumftime = fields[4].toDouble();
            model->collectPopCumftime (step, id, cumftime);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}

void OutputFileParser::parsePopCumsweptarea(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int id = fields[1].toInt();
            double cumsweptarea = fields[4].toDouble();
            model->collectPopCumsweptarea (step, id, cumsweptarea);

            double subsurfacecumsweptarea = fields[5].toDouble();
            model->collectPopCumsubsurfacesweptarea (step, id, subsurfacecumsweptarea);

        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}

void OutputFileParser::parsePopCumdiscards(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int id = fields[1].toInt();
            double cumdiscards = fields[4].toDouble();
            model->collectPopCumdiscards (step, id, cumdiscards);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}

void OutputFileParser::parsePopCumdiscardsratio(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int id = fields[1].toInt();
            double cumdiscardsratio = fields[4].toDouble();
            model->collectPopCumdiscardsratio (step, id, cumdiscardsratio);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}

void OutputFileParser::parsePopCumcatches(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int id = fields[1].toInt();
            double cumcatches = fields[4].toDouble();
            model->collectPopCumcatches (step, id, cumcatches);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}



void OutputFileParser::parsePopCumcatchesWithThreshold(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int id = fields[1].toInt();
            double cumcatches = fields[4].toDouble();
            model->collectPopCumcatchesWithThreshold (step, id, cumcatches);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}


void OutputFileParser::parsePopTariffs(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int id = fields[1].toInt();
            vector<double> tariffs;
            tariffs.push_back(fields[4].toDouble());
            tariffs.push_back(0);
            model->collectPopTariffs (step, id, tariffs);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}


void OutputFileParser::parsePopImpact(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[1].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int popid = fields[0].toInt();
            int nodeid = fields[2].toInt();
            double impact = fields[5].toDouble();
            model->collectPopImpact (step, nodeid, popid, impact);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}


void OutputFileParser::parsePopCumcatchesPerPop(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[1].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int popid = fields[0].toInt();
            int nodeid = fields[2].toInt();
            double cumcatchesthispop = fields[5].toDouble();
            model->collectPopCumcatchesPerPop (step, nodeid, popid, cumcatchesthispop);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}

void OutputFileParser::parseNodesEnvt(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, last_period = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            if (period != -1) {
                int p = (step / period);
                if (last_period < p) {
                    model->commitNodesStatsFromSimu(step, true);
                    last_period = p;
                }
            }
            int nodeid = fields[1].toInt();
            double salinity = fields[3].toDouble();
            model->collectSalinity (step, nodeid, salinity);
            double sst = fields[4].toDouble();
            model->collectSST (step, nodeid, sst);
            double wind = fields[5].toDouble();
            model->collectWind (step, nodeid, wind);
            double nitrogen = fields[6].toDouble();
            model->collectNitrogen (step, nodeid, nitrogen);
            double phosphorus = fields[7].toDouble();
            model->collectPhosphorus (step, nodeid, phosphorus);
            double oxygen = fields[8].toDouble();
            model->collectOxygen (step, nodeid, oxygen);
            double dissolvedcarbon = fields[9].toDouble();
            model->collectDissolvedCarbon( step, nodeid, dissolvedcarbon);
            double bathy = fields[10].toDouble();
            model->collectBathymetry( step, nodeid, bathy);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}


void OutputFileParser::parsePopBenthosStats(QFile *file, int tstep, DisplaceModel *model, int period)
{
    Q_UNUSED(period);
    Q_UNUSED(tstep);

    QTextStream strm (file);
    bool ok;

    int step, last_step = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[1].toInt();

        if (last_step != -1 && last_step != step) {
            model->commitPopBenthosStats(last_step);
        }

        int funcid = fields[0].toInt();
        int nodeid = fields[2].toInt();

        double benthosnumber = fields[5].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong benthosnumber %1").arg(fields[5]).toStdString());
        model->collectPopBenthosNumber (step, nodeid, funcid, benthosnumber);

        double benthosbiomass = fields[6].toDouble(&ok);  // deduced from N*meanw
        if (!ok) throw std::runtime_error(QString("wrong benthosbiomass %1").arg(fields[6]).toStdString());
        model->collectPopBenthosBiomass (step, nodeid, funcid, benthosbiomass);

        double meanweight = fields[7].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong meanweight %1").arg(fields[7]).toStdString());
        model->collectPopBenthosMeanWeight(step, nodeid, funcid, meanweight);

        double benthosbiomassOverK = fields[8].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong benthosbiomassOverK %1").arg(fields[8]).toStdString());
        model->collectPopBenthosBiomassOverK(step, nodeid, funcid, benthosbiomassOverK);

        double benthosnumberOverK = fields[9].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong benthosnumberOverK %1").arg(fields[9]).toStdString());
        model->collectPopBenthosNumberOverK(step, nodeid, funcid, benthosnumberOverK);

        last_step = step;
    }

    model->commitPopBenthosStats(last_step);
}


void OutputFileParser::parseFishfarmslogsStats(QFile *file, int tstep, DisplaceModel *model, int period)
{
    Q_UNUSED(period);
    Q_UNUSED(tstep);

    QTextStream strm (file);
    bool ok;

    int step, last_step = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (last_step != -1 && last_step != step) {
            model->commitFishfarmsStats(last_step);
        }

        int farmid = fields[5].toInt();
        int farmtype = fields[4].toInt();
        int nodeid = fields[1].toInt();

        // tstep(0) / node(1) / long(2) / lat(3) / farmtype(4) / farmid(5) / meanw_kg(6) / fish_harvested_kg / eggs_harvested_kg / fishfarm_annualprofit / fishfarm_netdischargeN  / fishfarm_netdischargeP

        double meanw_kg = fields[6].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong meanw_kg %1").arg(fields[6]).toStdString());
        model->collectFishfarmFishMeanWeight (step, nodeid, farmid, farmtype, meanw_kg);

        double fish_harvested_kg = fields[7].toDouble(&ok);  // deduced from N*meanw
        if (!ok) throw std::runtime_error(QString("wrong fish_harvested_kg %1").arg(fields[7]).toStdString());
        model->collectFishfarmFishHarvestedKg (step, nodeid, farmid,  farmtype, fish_harvested_kg);

        double eggs_harvested_kg = fields[8].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong eggs_harvested_kg %1").arg(fields[8]).toStdString());
        model->collectFishfarmEggsHarvestedKg(step, nodeid, farmid, farmtype, eggs_harvested_kg);

        double fishfarm_annualprofit = fields[9].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong fishfarm_annualprofit %1").arg(fields[9]).toStdString());
        model->collectFishfarmAnnualProfit(step, nodeid,farmid, farmtype, fishfarm_annualprofit);

        double fishfarm_netdischargeN = fields[10].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong fishfarm_netdischargeN %1").arg(fields[10]).toStdString());
        model->collectFishfarmNetDischargeN(step, nodeid, farmid, farmtype, fishfarm_netdischargeN);

        double fishfarm_netdischargeP = fields[11].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong fishfarm_netdischargeP %1").arg(fields[11]).toStdString());
        model->collectFishfarmNetDischargeP(step, nodeid, farmid, farmtype, fishfarm_netdischargeP);

        double fishfarm_cumulnetdischargeN = fields[12].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong fishfarm_cumulnetdischargeN %1").arg(fields[12]).toStdString());
        model->collectFishfarmCumulNetDischargeN(step, nodeid, farmid, farmtype, fishfarm_cumulnetdischargeN);

        double fishfarm_cumulnetdischargeP = fields[13].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong fishfarm_cumulnetdischargeP %1").arg(fields[13]).toStdString());
        model->collectFishfarmCumulNetDischargeP(step, nodeid, farmid, farmtype, fishfarm_cumulnetdischargeP);

        last_step = step;
    }

    model->commitFishfarmsStats(last_step);
}

void OutputFileParser::parsePopStats(QFile *file, int tstep, DisplaceModel *model, int period)
{
    QTextStream strm (file);

    int step, laststep = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (laststep != -1 && step != laststep)
            model->commitNodesStatsFromSimu(laststep);
        laststep = tstep;
    }

    model->commitNodesStatsFromSimu(laststep);
}

void OutputFileParser::parseVessels(QFile *file, int tstep, DisplaceModel *model, int period)
{
    Q_UNUSED(period);

    // to be completed.
    QTextStream strm (file);

    // TODO: last_step isn't used anymore. Demand the management of this to the inner collector.
    int step, last_step = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[1].toInt();

        if (last_step != -1 && last_step != step) {
            model->commitVesselsStats(last_step);
        }

        VesselStats vs = parseVesselStatLine(fields);
        if (vs.vesselId == -1) {
            qWarning() << "Line: " << line;
        } else {
            model->collectVesselStats(step, vs);
        }
        last_step = step;
    }

    model->commitVesselsStats(tstep);
}

VesselStats OutputFileParser::parseVesselStatLine(const QStringList &fields)
{
    VesselStats v;

    try {
        v.tstep = toInt(fields[1]);
        v.reasonToGoBack = toInt(fields[2]);
        v.lastHarbour = toInt(fields[4]);
        v.vesselId = toInt(fields[5]);
        v.name = fields[6].toStdString();
        v.timeAtSea = toDouble(fields[7]);
        v.cumFuelCons = toDouble(fields[8]);

        int pop = mModel->getPopulationsCount();
        for (int i = 0; i < pop; ++i) {
            double value = toDouble(fields[10+i]);
            v.mCatches.push_back(value);
        }

        QRegExp r(R"%(M(\((\d+)\))?.*)%");
        if (r.indexIn(fields[10+pop]) != -1) {
            // metier id
            bool ok;
            v.metierId = r.cap(2).toInt(&ok);
            if (!ok)
                v.metierId = -1;
        }

        v.revenueAV = toDouble(fields[10 + pop + 2]);
        v.revenueExAV = toDouble(fields[10 + pop + 3]);
        v.fuelCost = toDouble(fields[10 + pop + 4]);
        v.vpuf =0.0;
        if(v.fuelCost>1) {
            v.vpuf = toDouble(fields[10 + pop + 5]);
        }
        v.gav = toDouble(fields[10 + pop + 7]);
        v.sweptArea = toDouble(fields[10 + pop + 8]); // in m^2 in the input file
        v.revenuePerSweptArea =0.0;
        if(v.sweptArea>10) { // at least 10 sqr meters
            v.revenuePerSweptArea = toDouble(fields[10 + pop + 9]); // in euro per m^2 in the input file
        }

        int expop = mModel->getInterestingPops().size(); // discards recorded for explicit pop only
        for (int i = 0; i < expop; ++i) {
            double value = toDouble(fields[10+pop+10+i]);
            v.mDiscards.push_back(value);
        }

        v.GVA = toDouble(fields[10+pop+10+expop+1]);
        v.GVAPerRevenue = toDouble(fields[10+pop+10+expop+2]);
        v.LabourSurplus = toDouble(fields[10+pop+10+expop+3]);
        v.GrossProfit = toDouble(fields[10+pop+10+expop+4]);
        v.NetProfit = toDouble(fields[10+pop+10+expop+5]);
        v.NetProfitMargin = toDouble(fields[10+pop+10+expop+6]);
        v.GVAPerFTE = toDouble(fields[10+pop+10+expop+7]);
        v.RoFTA = toDouble(fields[10+pop+10+expop+8]);
        v.BER = toDouble(fields[10+pop+10+expop+9]);
        v.CRBER = toDouble(fields[10+pop+10+expop+10]);
        v.NetPresentValue = toDouble(fields[10+pop+10+expop+11]);
        v.numTrips=toInt(fields[10+pop+10+expop+12]); // not used in stat plots
    
    } catch (std::exception &x) {
        qWarning() << "Error parsing Vessel Stat Line: " << x.what();
        return VesselStats();
    }

    return v;
}



void OutputFileParser::parseShipsStats(QFile *file, int tstep, DisplaceModel *model, int period)
{
    Q_UNUSED(period);
    Q_UNUSED(tstep);

    QTextStream strm (file);
    bool ok;

    int step, last_step = -1;
    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        step = fields[0].toInt();

        if (last_step != -1 && last_step != step) {
            model->commitShipsStats(last_step);
        }

        int shipid = fields[5].toInt();
        int shiptype = static_cast<int>(fields[4].toFloat());
        int nodeid = fields[1].toInt();

        // tstep(0) / node(1) / long(2) / lat(3) /shiptype(4) / shipid(5) / nb_units / fuel_use_h /
        // NOx_emission_gperkW / SOx_emission_percentpertotalfuelmass / GHG_emission_gperkW / PME_emission_gperkW /
        // fuel_use_litre / NOx_emission / SOx_emission / GHG_emissions / PME_emission


        double nb_transported_units = fields[6].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong nb_transported_units %1").arg(fields[6]).toStdString());
        model->collectShipNbTransportedUnits (step, nodeid, shipid, shiptype, nb_transported_units);

        double fuel_use_h = fields[7].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong fuel_use_h %1").arg(fields[7]).toStdString());
        model->collectShipFuelUseHour (step, nodeid, shipid,  shiptype, fuel_use_h);

        double NOx_emission_gperkW = fields[8].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong NOx_emission_gperkW %1").arg(fields[8]).toStdString());
        model->collectShipNOxEmissiongPerkW (step, nodeid, shipid,  shiptype, NOx_emission_gperkW);

        double SOx_emission_percentpertotalfuelmass = fields[9].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong SOx_emission_percentpertotalfuelmass %1").arg(fields[9]).toStdString());
        model->collectShipSOxEmissionPercentPerTotalFuelmass (step, nodeid, shipid,  shiptype, SOx_emission_percentpertotalfuelmass);

        double GHG_emission_gperkW = fields[10].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong GHG_emission_gperkW %1").arg(fields[10]).toStdString());
        model->collectShipGHGemission (step, nodeid, shipid,  shiptype, GHG_emission_gperkW);

        double PME_emission_gperkW = fields[11].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong PME_emission %1").arg(fields[11]).toStdString());
        model->collectShipPMEemission (step, nodeid, shipid,  shiptype, PME_emission_gperkW);

        double fuel_use_litre = fields[12].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong fuel_use_litre %1").arg(fields[12]).toStdString());
        model->collectShipfuelUseLitre (step, nodeid, shipid,  shiptype, fuel_use_litre);

        double NOx_emission = fields[13].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong NOx_emission %1").arg(fields[13]).toStdString());
        model->collectShipNOxEmission (step, nodeid, shipid,  shiptype, NOx_emission);

        double SOx_emission = fields[14].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong SOx_emission %1").arg(fields[14]).toStdString());
        model->collectShipSOxEmission (step, nodeid, shipid,  shiptype, SOx_emission);

        double GHG_emission = fields[15].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong GHG_emission %1").arg(fields[15]).toStdString());
        model->collectShipGHGemission (step, nodeid, shipid,  shiptype, GHG_emission);

        double PME_emission = fields[16].toDouble(&ok);
        if (!ok) throw std::runtime_error(QString("wrong PME_emission %1").arg(fields[16]).toStdString());
        model->collectShipPMEemission (step, nodeid, shipid,  shiptype, PME_emission);

        last_step = step;
    }

    model->commitShipsStats(last_step);
}
