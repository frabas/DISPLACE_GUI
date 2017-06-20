#ifndef UPDATEDISPATCHER_H
#define UPDATEDISPATCHER_H

#include <QObject>
#include <QMap>
#include <QWaitCondition>
#include <QThread>

#include <memory>

class UpdateDispatcher : public QObject
{
    Q_OBJECT
public:
    UpdateDispatcher();

    void start();
    void stop();
    void postUpdate (int step, int id, float x, float y, float course, float fuel, int state);

signals:
    void vesselMoved (int laststep, int idx, float x, float y, float course, float fuel, int state);

protected:
    void worker();

    struct UpdateData {
        int laststep;
        int idx;
        float x,y,course, fuel;
        int state;
    };

    QMutex mWaiterMutex;
    QWaitCondition mWaiter;
    bool mShouldExit = false;
    QMap<int, std::shared_ptr<UpdateData>> mVessels;

    QThread mThread ;
};

#endif // UPDATEDISPATCHER_H
