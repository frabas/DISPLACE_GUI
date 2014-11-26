#ifndef BACKGROUDWORKER_H
#define BACKGROUDWORKER_H

#include <QObject>

class MainWindow;

class BackgroundWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundWorker(MainWindow *main);

    virtual void execute() = 0;

    QString getError() const { return mError; }
    virtual bool success() const { return mResult; }

signals:
    void workStarted();
    void workEnded();
    void progress(int n);
    void warning(QString,QString);

public slots:
    void process();

protected:
    void setFail(QString msg) { mResult = false; mError = msg; }
    void setSuccess() { mResult = true; }

    MainWindow *mMain;
    bool mResult;
    QString mError;
};

#endif // BACKGROUDWORKER_H
