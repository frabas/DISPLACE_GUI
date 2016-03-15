#ifndef SCHEDULERJOB_H
#define SCHEDULERJOB_H

#include <simulationrun.h>

#include <QList>

class SchedulerJob
{
    QList<SimulationRun> mList;

public:
    SchedulerJob();

    int jobsCount() const { return mList.size(); }
    SimulationRun &job(int n) { return mList[n]; }

    SimulationRun &addAt(int position);
};

#endif // SCHEDULERJOB_H
