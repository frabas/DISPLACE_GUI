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
        parsePopStart(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumftime_")) {
        parsePopCumftime(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumsweptarea_")) {
        parsePopCumsweptarea(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumcatches_")) {
        parsePopCumcatches(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_tariffs_")) {
        parsePopTariffs(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_impact_")) {
        parsePopImpact(&file, tstep, mModel, period);
    } else if (name.startsWith("popnodes_cumulcatches_per_pop_")) {
        parsePopCumcatchesPerPop(&file, tstep, mModel, period);
    } else if (name.startsWith("benthosnodes_tot_biomasses_")) {
        parsePopBenthosStats(&file, tstep, mModel, period);
    } else if (name.startsWith("benthosnodes_tot_numbers_")) {
        parsePopBenthosStats(&file, tstep, mModel, period);
    } else if (name.startsWith("popdyn_F_")) {
        parsePopdynF(&file, tstep, mModel, period);
    } else if (name.startsWith("popdyn_SSB_")) {
        parsePopdynSSB(&file, tstep, mModel, period);
    } else if (name.startsWith("popdyn_")) {
        parsePopdyn(&file, tstep, mModel, period);
    } else if (name.startsWith("loglike_")) {
        parseVessels(&file, tstep, mModel, period);
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

void OutputFileParser::parsePopBenthosStats(QFile *file, int tstep, DisplaceModel *model, int period)
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
            int funcid = fields[0].toInt();
            int nodeid = fields[2].toInt();
            double benthosnumber = fields[5].toDouble();
            model->collectPopBenthosNumber (step, nodeid, funcid, benthosnumber);
            double benthosbiomass = fields[6].toDouble();  // deduced from N*meanw
            model->collectPopBenthosBiomass (step, nodeid, funcid, benthosbiomass);
            double meanweight = fields[7].toDouble();
            model->collectPopBenthosMeanWeight(step, nodeid, funcid, meanweight);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}



void OutputFileParser::parsePopdynF(QFile *file, int tstep, DisplaceModel *model, int period)
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
            QVector<double> pop(model->getSzGrupsCount());
            int id = fields[1].toInt();

            double tot = 0;
            for (int i = 2; i < fields.size(); ++i) {
                double v = fields[i].toDouble();
                tot += v;
                pop[i-2] = v;
            }
            model->collectPopdynF(step, id, pop, tot);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}

void OutputFileParser::parsePopdynSSB(QFile *file, int tstep, DisplaceModel *model, int period)
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
            QVector<double> pop(model->getSzGrupsCount());
            int id = fields[1].toInt();

            double tot = 0;
            for (int i = 2; i < fields.size(); ++i) {
                double v = fields[i].toDouble();
                tot += v;
                pop[i-2] = v;
            }
            model->collectPopdynSSB(step, id, pop, tot);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
}


void OutputFileParser::parsePopdyn(QFile *file, int tstep, DisplaceModel *model, int period)
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

            QVector<double> pop(model->getSzGrupsCount());
            int id = fields[1].toInt();

            double tot = 0;
            for (int i = 2; i < fields.size() && (i-2) < pop.size(); ++i) {
                double v = fields[i].toDouble();
                tot += v;
                pop[i-2] = v;
            }
            model->collectPopdynN(step, id, pop, tot);
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step, true);
}

void OutputFileParser::parseVessels(QFile *file, int tstep, DisplaceModel *model, int period)
{
    // to be completed.
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

            VesselStats vs = parseVesselStatLine(fields);
            if (vs.vesselId == -1) {
                qWarning() << "Line: " << line;
            } else {
                model->collectVesselStats(step, vs);
            }
        }
    }

    if (tstep == -1)
        model->commitNodesStatsFromSimu(step);
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
        v.gav = toDouble(fields[10 + pop + 6]);

    } catch (std::exception &x) {
        qWarning() << "Error parsing Vessel Stat Line: " << x.what();
        return VesselStats();
    }

    return v;
}
