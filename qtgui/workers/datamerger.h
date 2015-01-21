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
        Weights, Ping
    };

    explicit DataMerger(MergeType type, DisplaceModel *model);
    ~DataMerger();

    void setWaitDialog (WaitDialog *dlg) {
        mWaitDialog = dlg;
    }
    void setDistance (double km) {
        mDist = km;
    }

    void start(QString in, QString out);
    bool checkResult();

    class Exception : public QException {
    public:
        explicit Exception (QString orffile, QString what)
            : mFile(orffile), mWhat(what) {
        }

        void raise() const { throw *this; }
        Exception *clone() const { return new Exception(*this); }

        QString message() const {
            return mWhat;
        }
        QString file() const {
             return mFile;
        }
    protected:
        QString mFile, mWhat;
    };

    static const char *const MergedField;
    static const char *const LatField;
    static const char *const LongField;
    static const char FieldSeparator;

signals:
    void completed (DataMerger *merger);

private slots:
    void workCompleted();
    void aborted();

private:
    MergeType mType;
    DisplaceModel *mModel;
    double mDist;
    bool mExit;

    QFuture<bool> mWork;
    QFutureWatcher<bool> *mWatcher;
    WaitDialog *mWaitDialog;
    QMutex mutex;

    bool doWork(QString in, QString out);

    struct ProcessData {
        QList<QString> *result;
        QStringList entry;
        QPointF pt;
        int col_pt_graph;
        bool colpresent;

        ProcessData(QList<QString> *_result, QStringList _entry,
            QPointF _pt, int _col_pt_graph, bool _colpresent)
            : result(_result), entry(_entry), pt(_pt), col_pt_graph(_col_pt_graph), colpresent(_colpresent) {
        }
    };

    void processLine (ProcessData data);
};

} // ns workers
} // ns displace

#endif // DATAMERGER_H
