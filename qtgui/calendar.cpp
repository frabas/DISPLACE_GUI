#include "calendar.h"

#include <readdata.h>
#include <QDebug>

Calendar::Calendar()
{
}

Calendar *Calendar::load(QString basepath, QString name)
{
    Calendar *calendar = new Calendar();

    std::vector<int> months = read_tsteps_months(name.toStdString(), basepath.toStdString());
    for(size_t i = 0; i < months.size(); ++i)
        calendar->mMonths.insert(months[i], i);

    std::vector<int> quarters = read_tsteps_quarters(name.toStdString(), basepath.toStdString());
    for(size_t i = 0; i < quarters.size(); ++i)
        calendar->mQuarters.insert(months[i], i);

    std::vector<int> sems = read_tsteps_semesters(name.toStdString(), basepath.toStdString());
    for(size_t i = 0; i < sems.size(); ++i)
        calendar->mSemesters.insert(months[i], i);

    std::vector<int> yrs = read_tsteps_years(name.toStdString(), basepath.toStdString());
    for(size_t i = 0; i < yrs.size(); ++i)
        calendar->mYears.insert(months[i], i);

    Q_ASSERT(calendar->mMonths.size() > 0);
    Q_ASSERT(calendar->mQuarters.size() > 0);
    Q_ASSERT(calendar->mSemesters.size() > 0);
    Q_ASSERT(calendar->mYears.size() > 0);

    return calendar;
}
