#ifndef DATAMERGER_H
#define DATAMERGER_H

#include <QObject>
#include <QException>
#include <QFuture>
#include <QFutureWatcher>
#include <waitdialog.h>

#include <displacemodel.h>

namespace displace {
namespace workers {

class DataMerger : public QObject
{
    Q_OBJECT
public:
    enum MergeType {
        Weights, Ping, PopulationDistribution
    };

    explicit DataMerger(MergeType type, DisplaceModel *model);
    ~DataMerger();

    void setWaitDialog (WaitDialog *dlg) {
        mWaitDialog = dlg;
    }
    void setDistance (double km) {
        mDist = km;
    }
    double distance() const { return mDist; }

    void start(QString in, QString out);
    bool checkResult();

    void setSeparator(QChar sep) { mSeparator = sep; }
    QChar separator() const { return mSeparator; }

    bool mustExit() const { return mExit; }

    QList<std::shared_ptr<NodeData>> getAllNodesWithin(QPointF pt, double dist) const;

    class Exception : public QException {
    public:
        explicit Exception (QString what)
            : mWhat(what) {
        }

        explicit Exception (QString orffile, QString what)
            : mFile(orffile), mWhat(what) {
        }

        void raise() const { throw *this; }
        Exception *clone() const { return new Exception(*this); }

        QString message() const {
            return mWhat;
        }
        const char *what() const _GLIBCXX_USE_NOEXCEPT {
            return mWhat.toStdString().c_str();
        }
        QString file() const {
             return mFile;
        }
    protected:
        QString mFile, mWhat;
    };

    static const char FieldSeparator;

signals:
    void completed (DataMerger *merger);

private slots:
    void workCompleted();
    void aborted();

public:
    class Strategy {
    public:
        virtual ~Strategy() {}
        virtual bool processHeaderField(QString field, int i) = 0;
        virtual bool postHeaderProcessed() = 0;
        /** \brief process a single line of the file
         * \note this is executed from within multiple thread, so it must be thread-safe.
         * */
        virtual void processLine (QString line) = 0;
        virtual bool saveOutput(QString out) = 0;
    };

private:
    MergeType mType;
    DisplaceModel *mModel;
    double mDist;
    QChar mSeparator;
    bool mExit;

    QFuture<bool> mWork;
    QFutureWatcher<bool> *mWatcher;
    QFutureWatcher<void> *mInternalWatcher;
    WaitDialog *mWaitDialog;

    Strategy *mStrategy;

    bool doWork(QString in, QString out);
};

} // ns workers
} // ns displace

#endif // DATAMERGER_H
