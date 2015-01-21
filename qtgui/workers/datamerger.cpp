#include "datamerger.h"

#include <QtConcurrent>
#include <QFile>
#include <QTextStream>

using namespace displace::workers;

const char *const DataMerger::MergedField = "pt_graph";
const char DataMerger::FieldSeparator = ' ';

DataMerger::DataMerger(MergeType type)
    : mType (type),
      mWork(),
      mWatcher(0),
      mWaitDialog(0)
{
}

DataMerger::~DataMerger()
{
    if (mWatcher) delete mWatcher;
}

void DataMerger::start(QString in, QString out)
{
    if (mWaitDialog) {
        mWaitDialog->setText(tr("Merging data file... please wait."));
        mWaitDialog->enableAbort(true);
        mWaitDialog->show();
    }

    mWork = QtConcurrent::run(this, &DataMerger::doWork, in, out);
    mWatcher = new QFutureWatcher<bool>();

    mWatcher->setFuture(mWork);

    connect (mWatcher, SIGNAL(finished()), this, SLOT(workCompleted()));
}

/** \brief this function returns true.
 * The purpose of this function is simply calling QFuture::result() to check whether an exception was raised.
 * */
bool DataMerger::checkResult()
{
    return mWork.result();
}

void DataMerger::workCompleted()
{
    emit completed(this);
}

bool DataMerger::doWork(QString in, QString out)
{
    QFile infile (in);
    if (!infile.open(QIODevice::ReadOnly))
        (new Exception(in, infile.errorString()))->raise();

    QTextStream instream(&infile);
    QList<QString> data;
    QString line;

    // First: read the headers
    line = instream.readLine();
    QStringList fields = line.split(FieldSeparator, QString::SkipEmptyParts);

    // Search for the proper column
    bool colpresent = true;
    int col = -1;
    for (int i = 0; i < fields.size(); ++i) {
        if (fields.at(i) == MergedField) {
            col = i;
            break;
        }
    }

    if (col == -1) {
        colpresent = false;
        fields.push_back(MergedField);
        col = fields.size()-1;
    }

    // Read the input file and calculate the field.
    while (!instream.atEnd()) {
        line = instream.readLine();
        QStringList entry = line.split(FieldSeparator, QString::SkipEmptyParts);

        int idx = 0;
        // TODO: Calculate idx here

        // update the field.
        while (entry.size() < col)
            entry.push_back(".");

        if (!colpresent)
            entry.insert(col, ".");
        entry[col] = QString::number(idx);

        data.push_back(entry.join(FieldSeparator));
    }

    infile.close();
    QFile outfile(out);

    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        (new Exception(out, outfile.errorString()))->raise();

    QTextStream outstream(&outfile);

    outstream << fields.join(FieldSeparator) << endl;
    foreach (QString line, data)
        outstream << line << endl;

    outfile.close();

    return true;
}
