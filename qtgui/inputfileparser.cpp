#include "inputfileparser.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

#include <Harbour.h>
#include <modelobjects/harbourdata.h>

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

    bool ok1, ok2;
    QTextStream strm(&f);
    QString line;

    int linenum = 1;
    while (!(line = strm.readLine()).isNull()) {
        QStringList fields = line.split(";");
        double x = fields[1].toDouble(&ok1);
        double y = fields[2].toDouble(&ok2);

        if (!ok1 || !ok2) {
            // Process error, but ignore for line 1
            if (linenum == 1)
                continue;

            if (error)
                *error = QString(QObject::tr("Error parsing coordinates at line %1")).arg(linenum);
            return false;
        }

        std::shared_ptr<Harbour> h (new Harbour(fields[0].toStdString()));
        std::shared_ptr<HarbourData> dt (new HarbourData(h));

        h->set_xy(x,y);

        list.push_back(dt);

        ++linenum;
    }


    return true;
}

bool InputFileParser::parseShortestPath(const QString &path, int nodeid, QList<QPair<int, int> > &graph)
{

}

bool InputFileParser::isShortestPathPresent(const QString &path, int nodeid)
{
    QString filename = QString("%1/previous_%2.dat").arg(path).arg(nodeid);
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
            int w = line.toInt(&ok);

            if (!ok) {
                if (error)
                    *error = QString(QObject::tr("Parse of line %1 of file %2 failed.")).arg(linenum).arg(graphpath);
            }

            nodes[srcs[i]].adiacencies.push_back(dsts[i]);
            nodes[srcs[i]].weight.push_back(w);
            nodes[dsts[i]].adiacencies.push_back(srcs[i]);
            nodes[dsts[i]].weight.push_back(w);

            ++linenum;
        }
    }

    return true;
}
