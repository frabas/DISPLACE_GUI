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

#include "inputfileparser.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

#include <Harbour.h>
#include <modelobjects/harbourdata.h>
#include <utils/displaceexception.h>

#include <QRegExp>
#include <QDebug>

InputFileParser::InputFileParser()
{
}

bool InputFileParser::parseHarbourFile(const QString &path, QList<std::shared_ptr<HarbourData> > &list, QString *error)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        if (error)
            *error = f.errorString();
        return false;
    }

    bool ok1, ok2, ok3;
    QTextStream strm(&f);
    QString line;

    int linenum = 0;
    while (!(line = strm.readLine()).isNull()) {
        ++linenum;
        if (linenum == 1)
            continue;

        QStringList fields = line.split(";");
        double x = fields[1].toDouble(&ok1);
        double y = fields[2].toDouble(&ok2);
        int hid = fields[3].toInt(&ok3);

        if (!ok1 || !ok2 || !ok3) {
            if (error)
                *error = QString(QObject::tr("Error parsing coordinates at line %1")).arg(linenum);
            return false;
        }

        std::shared_ptr<Harbour> h (new Harbour(fields[0].toStdString()));
        std::shared_ptr<HarbourData> dt (new HarbourData(h));

        h->set_xy(x,y);
        h->set_is_harbour(hid);

        list.push_back(dt);
    }


    return true;
}

bool InputFileParser::parseShortestPath(const QString & path, int nodeid, QList<QPair<int, int> > &graph)
{
    Q_UNUSED(path);
    Q_UNUSED(nodeid);
    Q_UNUSED(graph);

    // TODO: must be implemented.
    return false;
}

bool InputFileParser::isShortestPathPresent(const QString &path, types::NodeId nodeid)
{
    QString filename = QString("%1/previous_%2.dat").arg(path).arg(nodeid.toIndex());
    return QFile(filename).exists();
}

bool InputFileParser::parseGraph(const QString &graphpath, const QString &coordsPath, QList<GraphBuilder::Node> &nodes, QString *error)
{
    QFile cfile(coordsPath);
    if (!cfile.open(QIODevice::ReadOnly)) {
        if (error)
            *error = QString(QObject::tr("Error loading coords file %1: %2")).arg(coordsPath).arg(cfile.errorString());
        return false;
    }

    bool ok;
    QTextStream cstrm(&cfile);
    QString line;

    int linenum = 0;
    while (!(line = cstrm.readLine()).isNull()) {
        ++linenum;
    }

    if ((linenum % 3) != 0) {
        if (error)
            *error = QObject::tr("The number of lines in the Coords file must be a multiple of 3.");
        return false;
    }

    int nodesnum = linenum / 3;
    cstrm.seek(0);
    linenum = 0;
    for (int i = 0; i < nodesnum; ++i) {
        line = cstrm.readLine();

        GraphBuilder::Node nd;
        nd.good = true;

        double x = line.toDouble(&ok);
        if (!ok) {
            if (error)
                *error = QString(QObject::tr("Parse of line %1 of file %2 failed.")).arg(linenum).arg(coordsPath);
        }
        nd.point.setX(x);

        nodes.push_back(nd);

        ++linenum;
    }

    for (int i = 0; i < nodesnum; ++i) {
        line = cstrm.readLine();
        double y = line.toDouble(&ok);

        if (!ok) {
            if (error)
                *error = QString(QObject::tr("Parse of line %1 of file %2 failed.")).arg(linenum).arg(coordsPath);
        }

        nodes[i].point.setY(y);

        ++linenum;
    }

    for (int i = 0; i < nodesnum; ++i) {
        line = cstrm.readLine();
        int h = line.toInt(&ok);

        if (!ok) {
            if (error)
                *error = QString(QObject::tr("Parse of line %1 of file %2 failed.")).arg(linenum).arg(coordsPath);
        }

        nodes[i].harbour = h;

        ++linenum;
    }

    /* --- */

    if (!graphpath.isEmpty()) {
        QFile gfile(graphpath);
        if (!gfile.open(QIODevice::ReadOnly)) {
            if (error)
                *error = QString(QObject::tr("Error loading %1: %2")).arg(coordsPath).arg(cfile.errorString());
            return false;
        }

        QTextStream gstrm(&gfile);
        QString line;

        int linenum = 0;
        while (!(line = gstrm.readLine()).isNull()) {
            ++linenum;
        }

        if ((linenum % 3) != 0) {
            if (error)
                *error = QObject::QObject::tr("The number of lines in the Graph file must be a multiple of 3.");
            return false;
        }

        int graphedges = linenum / 3;
        gstrm.seek(0);
        linenum = 0;
        QList<int> srcs;
        for (int i = 0; i < graphedges; ++i) {
            line = gstrm.readLine();

            int src = line.toInt(&ok);
            if (!ok) {
                if (error)
                    *error = QString(QObject::tr("Parse of line %1 of file %2 failed.")).arg(linenum).arg(graphpath);
            }

            srcs.push_back(src);
            ++linenum;
        }

        QList<int> dsts;
        for (int i = 0; i < graphedges; ++i) {
            line = gstrm.readLine();
            int dst = line.toInt(&ok);

            if (!ok) {
                if (error)
                    *error = QString(QObject::tr("Parse of line %1 of file %2 failed.")).arg(linenum).arg(graphpath);
            }

            dsts.push_back(dst);
            ++linenum;
        }

        for (int i = 0; i < graphedges; ++i) {
            line = gstrm.readLine();
            double w = line.toDouble(&ok);

            if (!ok) {
                if (error)
                    *error = QString(QObject::tr("Parse of line %1 of file %2 failed.")).arg(linenum).arg(graphpath);
            }

            Q_ASSERT(w > 0.0001);
            nodes[srcs[i]].adiacencies.push_back(dsts[i]);
            nodes[srcs[i]].weight.push_back(w);

            ++linenum;
        }
    }

    return true;
}

bool InputFileParser::pathParseRelevantNodes(const QString &refpath, QString &fnodePath, QString &harbPath)
{
    QRegExp regexp("(.*)/vesselsspe_([^/_]+)_([^/]+).dat");

    if (regexp.indexIn(refpath) == -1) {
        return false;
    }

    fnodePath = QString("%1/vesselsspe_fgrounds_quarter%2.dat")
            .arg(regexp.cap(1)).arg("%1");
    harbPath = QString("%1/vesselsspe_harbours_quarter%2.dat")
            .arg(regexp.cap(1)).arg("%1");

    return true;

}

bool InputFileParser::pathParseRelevantInterNodes(const QString &refpath, QString &fnodePath, QString &harbPath)
{

    return true;

}

bool InputFileParser::parseRelevantNodes(const QString &file, QSet<int> &nodes)
{
    QFile infile (file);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't read" << file <<": " << infile.errorString();
        return false;
    }

    QTextStream stream(&infile);
    QString line;

    line = stream.readLine();  // ignore the first line

    bool ok;
    int linenum = 1;
    while (!(line = stream.readLine()).isNull()) {
        QStringList fields = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (fields.size() < 2) {
            qDebug() << "Cannot read relevant nodes. " << file << "at line " << linenum;
            return false;
        }

        int nd = fields[1].toInt(&ok);
        if (!ok) {
            qWarning() << "Error parsing file" << file << " at line " << linenum;
        } else {
            nodes.insert(nd);
        }
        ++linenum;
    }

    infile.close();
    return true;
}

bool InputFileParser::parseRelevantInterNodes(const QString &file, QVector<int> &nodeids)
{
    QFile infile (file);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't read" << file <<": " << infile.errorString();
        return false;
    }

    QTextStream stream(&infile);
    QString line;

    line = stream.readLine();  // ignore the first line

    bool ok;
    int linenum = 1;
    while (!(line = stream.readLine()).isNull()) {
        QStringList fields = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (fields.size() < 2) {
            qDebug() << "Cannot read relevant nodes. " << file << "at line " << linenum;
            return false;
        }

        int nd = fields[1].toInt(&ok);
        if (!ok) {
            qWarning() << "Error parsing file" << file << " at line " << linenum;
        } else {
            nodeids.push_back(nd);
        }
        ++linenum;
    }

    infile.close();
    return true;
}


bool InputFileParser::parseStockNamesFile(const QString &path, QMap<QString, int> &names)
{
    QFile infile (path);
    if (!infile.open(QIODevice::ReadOnly)) {
        (new displace::DisplaceException(infile.errorString(), path));
        qDebug() << "Can't read" << path <<": " << infile.errorString();
        return false;
    }

    QTextStream stream(&infile);
    QString line;

    line = stream.readLine();  // ignore the first line

    bool ok;
    int linenum = 1;
    while (!(line = stream.readLine()).isNull()) {
        QStringList fields = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
       // int nd = fields[1].toInt(&ok);
        int nd = fields[0].toInt(&ok);
        if (!ok) {
            (new displace::DisplaceException(QObject::tr("Failed to parse field #2"), path, linenum))->raise();
        } else {
           // names.insert(fields[0], nd);
             names.insert(fields[1], nd);
        }
        ++linenum;
    }

    infile.close();
    return true;
}
