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

    void clear();
    SimulationRun &addAt(int position);
    void remove(int position);

    std::list<SimulationRun::DataAsTuple> getDataAsTuple() const {
        std::list<SimulationRun::DataAsTuple> d;
        for (auto s : mList) {
            d.push_back(s.dataAsTuple());
        }
        return d;
    }

    void appendData (const std::list<SimulationRun> &dt) {
        for (auto d : dt) {
            mList.push_back(d);
        }
    }
};

#endif // SCHEDULERJOB_H
