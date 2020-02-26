//
// Created by fuga on 26/02/2020.
//

#include "Calendar.h"

#include <algorithm>

using namespace std;

Calendar::Calendar()
{

}

void Calendar::setCalendar(std::vector<int> months, std::vector<int> quarters, std::vector<int> semesters,
                           std::vector<int> years)
{
    tstepMonths = std::move(months);
    tstepQuarters = std::move(quarters);
    tstepSemesters = std::move(semesters);
    tstepYears = std::move(years);
}

bool Calendar::isFirstDayOfMonth(int tstep) const
{
    return std::binary_search(tstepMonths.begin(), tstepMonths.end(), tstep);
}

bool Calendar::isFirstDayOfQuarter(int tstep) const
{
    return std::binary_search(tstepQuarters.begin(), tstepQuarters.end(), tstep);
}

bool Calendar::isFirstDayOfSemested(int tstep) const
{
    return std::binary_search(tstepSemesters.begin(), tstepSemesters.end(), tstep);
}

bool Calendar::isFirstDayOfYear(int tstep) const
{
    return std::binary_search(tstepYears.begin(), tstepYears.end(), tstep);
}

const vector<int> &Calendar::months() const
{
    return tstepMonths;
}

const vector<int> &Calendar::quarters() const
{
    return tstepQuarters;
}

const vector<int> &Calendar::semesters() const
{
    return tstepSemesters;
}

const vector<int> &Calendar::years() const
{
    return tstepYears;
}
