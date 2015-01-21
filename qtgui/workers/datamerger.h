#ifndef DATAMERGER_H
#define DATAMERGER_H

#include <QObject>
#include <QException>
#include <QFuture>
#include <QFutureWatcher>
#include <waitdialog.h>

namespace displace {
namespace workers {

class DataMerger : public QObject
{
    Q_OBJECT
public:
    enum MergeType {
        Weights, Ping
    };

    explicit DataMerger(MergeType type);
    ~DataMerger();

    void setWaitDialog (WaitDialog *dlg) {
        mWaitDialog = dlg;
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
    static const char FieldSeparator;

signals:
    void completed (DataMerger *merger);

private slots:
    void workCompleted();

private:
    MergeType mType;

    QFuture<bool> mWork;
    QFutureWatcher<bool> *mWatcher;
    WaitDialog *mWaitDialog;

    bool doWork(QString in, QString out);
};

} // ns workers
} // ns displace

#endif // DATAMERGER_H
