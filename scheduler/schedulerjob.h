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

    std::list<SimulationRun::DataAsTuple> getDataAsTuple() const {
        std::list<SimulationRun::DataAsTuple> d;
        for (auto s : mList) {
            d.push_back(s.dataAsTuple());
        }
        return d;
    }
};

#endif // SCHEDULERJOB_H
