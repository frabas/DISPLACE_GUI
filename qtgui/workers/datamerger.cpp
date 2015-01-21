#include "datamerger.h"

#include <QtConcurrent>
#include <QFile>
#include <QTextStream>
#include <QFutureSynchronizer>

#include <modelobjects/nodedata.h>

#include <GeographicLib/Geodesic.hpp>

using namespace displace::workers;

const char *const DataMerger::MergedField = "PT_GRAPH";
const char *const DataMerger::LatField = "SI_LATI";
const char *const DataMerger::LongField = "SI_LONG";
const char DataMerger::FieldSeparator = ' ';

DataMerger::DataMerger(MergeType type, DisplaceModel *model)
    : mType (type),
      mModel(model),
      mDist(50.0),
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
        mWaitDialog->setProgress(true, 0);
        mWaitDialog->show();

        connect (mWaitDialog, SIGNAL(aborted()), this, SLOT(aborted()));
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

void DataMerger::aborted()
{
    mExit = true;
}

bool DataMerger::doWork(QString in, QString out)
{
    QFile infile (in);
    if (!infile.open(QIODevice::ReadOnly))
        (new Exception(in, infile.errorString()))->raise();

    QTextStream picker(&infile);
    size_t rows = 0;
    while (!picker.atEnd()) {
        picker.readLine();
        ++rows;
    }

    if (mWaitDialog)
        mWaitDialog->setProgress(true, rows);

    infile.seek(0);
    QTextStream instream(&infile);
    QList<QString> data;
    QString line;

    // First: read the headers
    line = instream.readLine();
    QStringList fields = line.split(FieldSeparator, QString::SkipEmptyParts);

    // Search for the proper column
    bool colpresent = true;
    int col_pt_graph = -1;
    int col_lat = -1;
    int col_lon = -1;
    for (int i = 0; i < fields.size(); ++i) {
        if (fields.at(i) == MergedField) {
            col_pt_graph = i;
        } else if(fields.at(i) == LatField) {
            col_lat = i;
        } else if (fields.at(i) == LongField) {
            col_lon = i;
        }
    }

    if (col_lat == -1 || col_lon == -1) {
        (new Exception(in, QString(tr("%1 or %2 fields are missing"))
                       .arg(LatField).arg(LongField)))->raise();
    }

    if (col_pt_graph == -1) {
        colpresent = false;
        fields.push_back(MergedField);
        col_pt_graph = fields.size()-1;
    }

    // Read the input file and calculate the field.
    bool ok;
    double lat,lon;
    int row=1;

    mExit = false;

    QFutureWatcher<void> watcher;

    if (mWaitDialog)
        mWaitDialog->setFormat(tr("Reading file: %p%"));
    QList<ProcessData> processQueue;
    while (!instream.atEnd() && !mExit) {
        line = instream.readLine();
        if (mWaitDialog) {
            mWaitDialog->setProgression(row);
        }
        ++row;

        QStringList entry = line.split(FieldSeparator, QString::SkipEmptyParts);

        lat = entry.at(col_lat).toDouble(&ok);
        if (!ok)
            (new Exception(in, QString(tr("Error parsing field %1 line %2 - not a double")).arg(col_lat).arg(row)))->raise();

        lon = entry.at(col_lon).toDouble(&ok);
        if (!ok)
            (new Exception(in, QString(tr("Error parsing field %1 line %2 - not a double")).arg(col_lon).arg(row)))->raise();

        ProcessData d(&data,entry, QPointF(lon, lat), col_pt_graph, colpresent);
        processQueue.push_back(d);
    }

    infile.close();

    if (mWaitDialog)
        mWaitDialog->setFormat(tr("Processed %v lines of %m"));

    // lambda
    QFuture<void> work = QtConcurrent::map(processQueue, [this](ProcessData data) { this->processLine(data);});
    watcher.setFuture(work);
    if (mWaitDialog) {
        watcher.moveToThread(mWaitDialog->thread());
        connect (&watcher, SIGNAL(progressRangeChanged(int,int)), mWaitDialog, SLOT(setProgress(int,int)));
        connect (&watcher, SIGNAL(progressValueChanged(int)), mWaitDialog, SLOT(setProgression(int)));
    }

    watcher.waitForFinished();

    if (mExit)
        return false;

    if (mWaitDialog) {
        mWaitDialog->setText(tr("Saving file"));
    }

    QFile outfile(out);

    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        (new Exception(out, outfile.errorString()))->raise();

    QTextStream outstream(&outfile);

    outstream << fields.join(FieldSeparator) << endl;

    row = 0;
    if (mWaitDialog) {
        mWaitDialog->setFormat(tr("%p%"));
        mWaitDialog->setProgress(0, data.size());
    }

    foreach (QString line, data) {
        outstream << line << endl;
        ++row;
        if (mWaitDialog)
            mWaitDialog->setProgression(row);
    }

    outfile.close();

    return true;
}

void DataMerger::processLine(ProcessData data)
{
    int idx = -1;

#if GEOGRAPHICLIB_VERSION_MINOR > 25
        const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
        const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

    QList<std::shared_ptr<NodeData>> nodes = mModel->getAllNodesWithin(data.pt, mDist);

    double mindist = 1e90;
    double dist;
    std::shared_ptr<NodeData> nearestNode;
    foreach (std::shared_ptr<NodeData> node, nodes) {
        if (mType == Weights && node->get_is_harbour())
            continue;
        if (mType == Ping && !node->get_is_harbour())
            continue;

        geod.Inverse(node->get_y(), node->get_x(), data.pt.y(), data.pt.x(), dist);
        if (dist < mindist) {
            nearestNode = node;
            mindist = dist;
        }
    }

    if (nearestNode)
        idx = nearestNode->get_idx_node();

    // update the field.
    while (data.entry.size() < data.col_pt_graph)
        data.entry.push_back(".");

    if (!data.colpresent)
        data.entry.insert(data.col_pt_graph, ".");
    data.entry[data.col_pt_graph] = QString::number(idx);

    mutex.lock();
    data.result->push_back(data.entry.join(FieldSeparator));
    mutex.unlock();
}
