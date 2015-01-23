#include "csvimporter.h"

#include <QFile>
#include <QTextStream>


CsvImporter::CsvImporter()
    : mSeparator(" "),
      mSplitBehaviour(QString::SkipEmptyParts)
{
}

QList<QStringList> CsvImporter::import(QString filename)
{
    QFile file (filename);

    if (!file.open(QIODevice::ReadOnly))
        (new Exception(file.errorString()))->raise();

    QTextStream stream(&file);

    QList<QStringList> list;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.isNull())
            break;
        list.push_back(line.split(mSeparator, mSplitBehaviour));
    }

    file.close();
    return list;
}
