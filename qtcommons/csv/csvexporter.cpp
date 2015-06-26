#include "csvexporter.h"

#include <QFile>
#include <QTextStream>

CsvExporter::CsvExporter()
    : mSeparator(',')
{
}

bool CsvExporter::exportFile(QString filename, const QList<QStringList> &list) throw (Exception)
{
    QFile file (filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        (new Exception(file.errorString()))->raise();
    }

    QTextStream stream (&file);
    foreach (QStringList l, list) {
        stream << l.join(mSeparator) << endl;
    }

    file.close();
    return true;
}
