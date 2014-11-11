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

signals:
    void workStarted();
    void workEnded();

public slots:
    void process();

protected:
    MainWindow *mMain;
};

#endif // BACKGROUDWORKER_H
