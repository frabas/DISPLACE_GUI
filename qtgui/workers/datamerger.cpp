#include "datamerger.h"

#include "populationdistributiondatamergerstrategy.h"

#include <QtConcurrent>
#include <QFile>
#include <QTextStream>
#include <QFutureSynchronizer>


using namespace displace::workers;

const char DataMerger::FieldSeparator = ' ';


DataMerger::DataMerger(Strategy *strategy, DisplaceModel *model)
    : mModel(model),
      mDist(50.0),
      mSeparator(FieldSeparator),
      mWork(),
      mWatcher(0),
      mInternalWatcher(0),
      mWaitDialog(0),
      mStrategy(strategy)
{
}

DataMerger::~DataMerger()
{
    if (mWatcher) delete mWatcher;
    if (mInternalWatcher) delete mInternalWatcher;
}

void DataMerger::start(QString in, QString out)
{
    mStrategy->attach(this);

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
bool DataMerger::checkResult()
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
