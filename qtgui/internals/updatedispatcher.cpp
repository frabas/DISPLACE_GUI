#include "updatedispatcher.h"

#include <QDebug>

UpdateDispatcher::UpdateDispatcher()
{

}

void UpdateDispatcher::start()
{
    moveToThread(&mThread);

    connect (&mThread, &QThread::started, std::bind(&UpdateDispatcher::worker, this));
    mThread.start();
}

void UpdateDispatcher::stop()
{
    mWaiterMutex.lock();
    mShouldExit = true;
    mWaiter.wakeAll();
    mWaiterMutex.unlock();
}

void UpdateDispatcher::postUpdate(int step, int id, float x, float y, float course, float fuel, int state)
{
    mWaiterMutex.lock();
    if (mVessels.contains(id)) {
        auto b = mVessels[id];
        b->laststep = step;
        b->idx = id;
        b->x = x;
        b->y = y;
        b->course = course;
        b->fuel = fuel;
        b->state = state;
    } else {
        auto b = std::make_shared<UpdateData>();
        b->laststep = step;
        b->idx = id;
        b->x = x;
        b->y = y;
        b->course = course;
        b->fuel = fuel;
        b->state = state;
        mVessels.insert(id, b);
    }
    qDebug() << "Posted, l: " << mVessels.count();
    mWaiter.wakeAll();
    mWaiterMutex.unlock();
}

void UpdateDispatcher::worker()
{
    qDebug() << "Worker started";
    while (true) {
        mWaiterMutex.lock();
        while (!mShouldExit && mVessels.isEmpty())
            mWaiter.wait(&mWaiterMutex);
        if (mShouldExit)
            break;

        qDebug() << "Queue Length: " << mVessels.count();
        auto first = *mVessels.begin();
        mVessels.remove(first->idx);
        mWaiterMutex.unlock();

        emit vesselMoved(first->laststep, first->idx, first->x, first->y, first->course, first->fuel, first->state);
    }

    mWaiterMutex.unlock();
}
