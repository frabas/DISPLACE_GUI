//
// Created by fuga on 26/02/2020.
//

#ifndef DISPLACE_CALENDAR_H
#define DISPLACE_CALENDAR_H

#include <vector>

class Calendar {
    std::vector<int> tstepMonths;
    std::vector<int> tstepQuarters;
    std::vector<int> tstepSemesters;
    std::vector<int> tstepYears;
public:
    Calendar();

    void setCalendar(std::vector<int> months,
                     std::vector<int> quarters,
                     std::vector<int> semesters,
                     std::vector<int> years);

    bool isFirstDayOfMonth(int tstep) const;

    bool isFirstDayOfQuarter(int tstep) const;

    bool isFirstDayOfSemested(int tstep) const;

    bool isFirstDayOfYear(int tstep) const;

    const std::vector<int> &months() const;

    const std::vector<int> &quarters() const;

    const std::vector<int> &semesters() const;

    const std::vector<int> &years() const;
};


#endif //DISPLACE_CALENDAR_H
