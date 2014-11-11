#ifndef CALENDAR_H
#define CALENDAR_H

#include <QString>
#include <QMap>

class Calendar
{
public:
    Calendar();

    bool isMonth(int tstep) const {
        return mMonths.contains(tstep);
    }

    bool isSemester(int tstep) const {
        return mSemesters.contains(tstep);
    }

    bool isQuarted(int tstep) const {
        return mQuarters.contains(tstep);
    }

    bool isYear(int tstep) const {
        return mYears.contains(tstep);
    }

    static Calendar *load(QString basepath, QString name);
private:
    QMap<int, int> mMonths;
    QMap<int, int> mQuarters;
    QMap<int, int> mSemesters;
    QMap<int, int> mYears;
};

#endif // CALENDAR_H
