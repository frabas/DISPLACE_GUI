#include "schedulerjob.h"

SchedulerJob::SchedulerJob()
{

}

void SchedulerJob::clear()
{
    mList.clear();
}

SimulationRun &SchedulerJob::addAt(int position)
{
    mList.insert(position, SimulationRun());
    return mList[position];
}

void SchedulerJob::remove(int position)
{
    mList.removeAt(position);
}
