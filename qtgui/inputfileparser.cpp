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
