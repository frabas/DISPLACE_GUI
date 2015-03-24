#include "datamerger.h"

#include "populationdistributiondatamergerstrategy.h"

#include <QtConcurrent>
#include <QFile>
#include <QTextStream>
#include <QFutureSynchronizer>

#include <modelobjects/nodedata.h>

#include <GeographicLib/Geodesic.hpp>

using namespace displace::workers;

const char DataMerger::FieldSeparator = ' ';

class MergerStrategy : public DataMerger::Strategy
{
public:
    MergerStrategy (DataMerger *owner, DataMerger::MergeType type);
    bool processHeaderField(QString field, int i) override;
    bool postHeaderProcessed() override;
    void processLine (int linenum, QString line) override;
    bool saveOutput(QString out) override;

    static const char *const MergedField;
    static const char *const LatField;
    static const char *const LongField;

private:
    DataMerger *mOwner;
    DataMerger::MergeType mType;

    bool colpresent = true;
    int col_pt_graph = -1;
    int col_lat = -1;
    int col_lon = -1;

    QStringList mFields;
    QStringList mResults;
    QMutex mutex;
};

const char *const MergerStrategy::MergedField = "PT_GRAPH";
const char *const MergerStrategy::LatField = "SI_LATI";
const char *const MergerStrategy::LongField = "SI_LONG";

DataMerger::DataMerger(MergeType type, DisplaceModel *model)
    : mType (type),
      mModel(model),
      mDist(50.0),
      mSeparator(FieldSeparator),
      mWork(),
      mWatcher(0),
      mInternalWatcher(0),
      mWaitDialog(0),
      mStrategy(0)
{
    switch (mType) {
    case Ping:
    case Weights:
        mStrategy = new MergerStrategy(this, mType);
        break;
    case PopulationDistribution:
        mStrategy = new PopulationDistributionDataMergerStrategy(this, mModel);
        break;
    default:
        (new displace::DisplaceException("Program error - Undefined strategy in DataMerger constructor"))->raise();
    }
}

DataMerger::~DataMerger()
{
    if (mWatcher) delete mWatcher;
    if (mInternalWatcher) delete mInternalWatcher;
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
 * The purpose of this function is simply calling QFuture::result() to check whether an displace::DisplaceException was raised.
 * */
bool DataMerger::checkResult() throw (displace::DisplaceException)
{
    // Note: mWork.result() will throw if an displace::DisplaceException has been raised. So it must be called before
    // any other operation that returns values.
    bool res = mWork.result();
    if (mWork.isCanceled())
        return false;
    return res;
}

QList<std::shared_ptr<NodeData> > DataMerger::getAllNodesWithin(QPointF pt, double dist) const
{
    return mModel->getAllNodesWithin(pt, dist);
}

void DataMerger::workCompleted()
{
    emit completed(this);
}

void DataMerger::aborted()
{
    mExit = true;
    mInternalWatcher->cancel();
}

bool DataMerger::doWork(QString in, QString out)
{
    mExit = false;

    QFile infile (in);
    if (!infile.open(QIODevice::ReadOnly))
        (new displace::DisplaceException(in, infile.errorString()))->raise();

    if (mWaitDialog)
        mWaitDialog->setFormat(tr("Reading file: %p%"));

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
    QString line;

    try {
    // First: read the headers
        line = instream.readLine();
        QStringList fields = line.split(mSeparator, QString::SkipEmptyParts);

        // Search for the proper column
        for (int i = 0; i < fields.size(); ++i) {
            mStrategy->processHeaderField(fields[i], i);
        }

        mStrategy->postHeaderProcessed();
    } catch (displace::DisplaceException ex) {
        throw new displace::DisplaceException (in, ex.what());
    }

    if (mExit) {
        infile.close();
        return false;
    }

    // Read the input file and calculate the field.
    int row=1;

    struct Line {
        int n;
        QString str;
    };

    QList<Line> processQueue;
    while (!instream.atEnd() && !mExit) {
        line = instream.readLine();
        ++row;

        Line l;
        l.n = row;
        l.str = line;
        processQueue.push_back(l);
    }

    if (mExit) {
        infile.close();
        return false;
    }

    infile.close();

    if (mWaitDialog)
        mWaitDialog->setFormat(tr("Processed %v lines of %m"));

    mInternalWatcher = new QFutureWatcher<void>;

    // lambda
    QFuture<void> work = QtConcurrent::map(processQueue, [this](Line line) { this->mStrategy->processLine(line.n, line.str);});
    mInternalWatcher->setFuture(work);
    if (mWaitDialog) {
        mInternalWatcher->moveToThread(mWaitDialog->thread());
        connect (mInternalWatcher, SIGNAL(progressRangeChanged(int,int)), mWaitDialog, SLOT(setProgress(int,int)));
        connect (mInternalWatcher, SIGNAL(progressValueChanged(int)), mWaitDialog, SLOT(setProgression(int)));
    }

    mInternalWatcher->waitForFinished();

    if (mExit) {
        return false;
    }

    if (mWaitDialog) {
        mWaitDialog->setText(tr("Saving file..."));
    }
    try {
        mStrategy->saveOutput(out);
    } catch (displace::DisplaceException ex) {
        (new displace::DisplaceException (out, ex.what()))->raise();
    }

    return true;
}

MergerStrategy::MergerStrategy(DataMerger *owner, DataMerger::MergeType type)
    : Strategy(),
      mOwner(owner),
      mType(type)
{
}

bool MergerStrategy::processHeaderField(QString field, int i)
{
    if (field == MergedField) {
        col_pt_graph = i;
    } else if(field == LatField) {
        col_lat = i;
    } else if (field == LongField) {
        col_lon = i;
    }

    mFields.push_back(field);
    return true;
}

bool MergerStrategy::postHeaderProcessed()
{
    if (col_lat == -1 || col_lon == -1) {
        (new displace::DisplaceException(QString(QObject::tr("%1 or %2 fields are missing"))
                       .arg(LatField).arg(LongField)))->raise();
    }

    if (col_pt_graph == -1) {
        colpresent = false;
        mFields.push_back(MergedField);
        col_pt_graph = mFields.size()-1;
    }

    return true;
}

bool MergerStrategy::saveOutput(QString out)
{
    QFile outfile(out);

    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        (new displace::DisplaceException(outfile.errorString()))->raise();

    QTextStream outstream(&outfile);

    outstream << mFields.join(mOwner->separator()) << endl;

    int row = 0;
    foreach (QString line, mResults) {
        outstream << line << endl;
        ++row;

        if (mOwner->mustExit()) {
            outfile.close();
            return false;
        }
    }

    outfile.close();

    return true;
}

void MergerStrategy::processLine (int linenum, QString line)
{
    bool ok;

    QStringList entry = line.split(mOwner->separator(), QString::SkipEmptyParts);
    if (entry.size() < col_lat || entry.size() < col_lon)  // Skip empty / incorrect lines
        return;

    double lat = entry.at(col_lat).toDouble(&ok);
    if (!ok)
        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();

    double lon = entry.at(col_lon).toDouble(&ok);
    if (!ok)
        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();

    int idx = -1;

#if GEOGRAPHICLIB_VERSION_MINOR > 25
        const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
        const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

    QList<std::shared_ptr<NodeData>> nodes = mOwner->getAllNodesWithin(QPointF(lon,lat), mOwner->distance());

    double mindist = 1e90;
    double dist;
    std::shared_ptr<NodeData> nearestNode;
    foreach (std::shared_ptr<NodeData> node, nodes) {
        if (mType == DataMerger::Weights && node->get_is_harbour())
            continue;
        if (mType == DataMerger::Ping && !node->get_is_harbour())
            continue;

        geod.Inverse(node->get_y(), node->get_x(), lat, lon, dist);
        if (dist < mindist) {
            nearestNode = node;
            mindist = dist;
        }
    }

    if (nearestNode)
        idx = nearestNode->get_idx_node();

    // update the field.
    while (entry.size() < col_pt_graph)
        entry.push_back(".");

    if (!colpresent)
        entry.insert(col_pt_graph, ".");
    entry[col_pt_graph] = QString::number(idx);

    mutex.lock();
    mResults.push_back(entry.join(mOwner->separator()));
    mutex.unlock();

}
