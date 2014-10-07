#include "outputfileparser.h"

#include <displacemodel.h>
#include <mainwindow.h>

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

    if (!file.open(QFile::ReadOnly)) {
        emit error(QString(QObject::tr("The file %1 cannot be read: %2"))
                   .arg(file.fileName())
                   .arg(file.errorString()));
        return;
    }

    if (name.startsWith("popnodes_start_")) {
        qDebug() << "Parsing " << path << " as population node start";

        parsePopStart(&file, mModel);
    } else if (name.startsWith("popnodes_cumftime_")) {
        qDebug() << "Parsing " << path << " as population cum_ftime";

        parsePopCumftime(&file, tstep, mModel);
    } else if (name.startsWith("popdyn_F_")) {
        parsePopdynF(&file, tstep, mModel);
    } else if (name.startsWith("popdyn_")) {
        parsePopdyn(&file, tstep, mModel);
    } else { /* Don't know how to handle... */

        qDebug() << "File isn't recognized: " << path;
    }

    emit parseCompleted();
}


/*
 * PopStart file is a list of space-separated lines containing:
 * nodeid x y population1 population2 ...
 * see Node::export_popnodes for details
 */
void OutputFileParser::parsePopStart(QFile *file, DisplaceModel *model)
{
    QTextStream strm (file);

    QList<double> data;
    data.reserve(model->getNBPops());
    for (int i = 0; i < model->getNBPops(); ++i)
        data.push_back(0.0);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int id = fields[1].toInt();

        int i;
        for (i = 4 ; i < fields.size()-1; ++i) {
            data[i-4] = fields[i].toDouble();
        }
        double tot = fields[i].toDouble();

        model->collectNodePopStats(0, id, data, tot);
    }
}

void OutputFileParser::parsePopCumftime(QFile *file, int tstep, DisplaceModel *model)
{
    QTextStream strm (file);

    while (!strm.atEnd()) {
        QString line = strm.readLine();
        QStringList fields = line.split(" ", QString::SkipEmptyParts);
        int step = fields[0].toInt();

        if (step == tstep) {
            int id = fields[1].toInt();
            double cumftime = fields[4].toDouble();
            model->collectPopCumftime (tstep, id, cumftime);
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

        if (step == tstep) {
            int id = fields[1].toInt();
            for (int i = 2; i < fields.size(); ++i) {
                model->collectPopdynF(tstep, id, fields[i].toDouble());
            }
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

        if (step == tstep) {
            int id = fields[1].toInt();
            for (int i = 2; i < fields.size(); ++i) {
                model->collectPopdynN(tstep, id, fields[i].toDouble());
            }
        }
    }
}
