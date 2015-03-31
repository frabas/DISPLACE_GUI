#include "outputfileparser.h"

#include <displacemodel.h>
#include <mainwindow.h>
#include <modelobjects/vesseldata.h>


#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include <QMessageBox>

OutputFileParser::OutputFileParser(DisplaceModel *model, QObject *parent)
    : QObject(parent),
      mModel(model)
{
}

void OutputFileParser::parse(QString path, int tstep)
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
        parsePopStart(&file, tstep, mModel);
    } else if (name.startsWith("popnodes_cumftime_")) {
        parsePopCumftime(&file, tstep, mModel);
    } else if (name.startsWith("popnodes_cumsweptarea_")) {
        parsePopCumsweptarea(&file, tstep, mModel);
    } else if (name.startsWith("popnodes_impact_")) {
        parsePopImpact(&file, tstep, mModel);
    } else if (name.startsWith("benthosnodes_tot_biomasses_")) {
        parsePopBenthosBiomass(&file, tstep, mModel);
    } else if (name.startsWith("popdyn_F_")) {
        parsePopdynF(&file, tstep, mModel);
    } else if (name.startsWith("popdyn_")) {
        parsePopdyn(&file, tstep, mModel);
    } else if (name.startsWith("loglike_")) {
        //parseVessels(&file, tstep, mModel);
        qDebug() << "File ignored...";
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
void OutputFileParser::parsePopStart(QFile *file, int tstep, DisplaceModel *model)
{
    QTextStream strm (file);

    QList<double> dataW;
    QList<double> data;
    data.reserve(model->getNBPops());
    for (int i = 0; i < model->getNBPops(); ++i) {
        data.push_back(0.0);
        dataW.push_back(0.0);
    }

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[0].toInt();

        if (tstep == -1 || step == tstep) {
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
}

void OutputFileParser::parsePopCumftime(QFile *file, int tstep, DisplaceModel *model)
{
    QTextStream strm (file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            int id = fields[1].toInt();
            double cumftime = fields[4].toDouble();
            model->collectPopCumftime (step, id, cumftime);
        }
    }
}

void OutputFileParser::parsePopCumsweptarea(QFile *file, int tstep, DisplaceModel *model)
{
    QTextStream strm (file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            int id = fields[1].toInt();
            double cumsweptarea = fields[4].toDouble();
            model->collectPopCumsweptarea (step, id, cumsweptarea);
        }
    }
}


void OutputFileParser::parsePopImpact(QFile *file, int tstep, DisplaceModel *model)
{
    QTextStream strm (file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[1].toInt();

        if (step == tstep || tstep == -1) {
            int popid = fields[0].toInt();
            int nodeid = fields[2].toInt();
            double impact = fields[5].toDouble();
            model->collectPopImpact (step, nodeid, popid, impact);
        }
    }

}

void OutputFileParser::parsePopBenthosBiomass(QFile *file, int tstep, DisplaceModel *model)
{
    QTextStream strm (file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[1].toInt();

        if (step == tstep || tstep == -1) {
            int funcid = fields[0].toInt();
            int nodeid = fields[2].toInt();
            double benthosbiomass = fields[5].toDouble();
            model->collectPopBenthosBiomass (step, nodeid, funcid, benthosbiomass);
        }
    }

}


void OutputFileParser::parsePopdynF(QFile *file, int tstep, DisplaceModel *model)
{
    QTextStream strm (file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
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
}

void OutputFileParser::parsePopdyn(QFile *file, int tstep, DisplaceModel *model)
{
    QTextStream strm (file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[0].toInt();

        if (step == tstep || tstep == -1) {
            QVector<double> pop(model->getSzGrupsCount());
            int id = fields[1].toInt();

            double tot = 0;
            for (int i = 2; i < fields.size(); ++i) {
                double v = fields[i].toDouble();
                tot += v;
                pop[i-2] = v;
            }
            model->collectPopdynN(step, id, pop, tot);
        }
    }
}

void OutputFileParser::parseVessels(QFile *file, int tstep, DisplaceModel *model)
{
    // to be completed.
    qDebug() << "PARSING: " << file->fileName();
    QTextStream strm (file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[1].toInt();

        if (step == tstep || tstep == -1) {
            QVector<double> pop(model->getSzGrupsCount());
            int id = fields[5].toInt();

//            model->collectVesselStats(step, 0);
        }
    }
}

VesselStats OutputFileParser::parseVesselStatLine(const QStringList &fields)
{
    VesselStats v;

    try {
        v.tstep = toInt(fields[1]);
        v.reasonToGoBack = toInt(fields[2]);
        v.lastHarbour = toInt(fields[4]);
        v.vesselId = toInt(fields[5]);
        v.timeAtSea = toDouble(fields[7]);
        v.cumFuelCons = toDouble(fields[8]);

        int pop = fields.size() - 16;
        for (int i = 0; i < pop; ++i) {
            double value = toDouble(fields[10+i]);
            v.mCatches.push_back(value);
        }
        v.revenueAV = toDouble(fields[10 + pop + 2]);
        v.fuelCost = toDouble(fields[10 + pop + 3]);
        v.gav = toDouble(fields[10 + pop + 5]);

    } catch (std::exception &x) {
        Q_UNUSED(x);
        return VesselStats();
    }

    return v;
}
