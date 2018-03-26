// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "calendar.h"

#include <readdata.h>
#include <storage/sqliteoutputstorage.h>
#include <QDebug>

const QString Calendar::days[] = {
    "mon", "tue", "wed", "thu",
    "fri", "sat", "sun"
};

const QString Calendar::months[] = {
    "Jan", "Feb", "Mar", "Apr",
    "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

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
        calendar->mQuarters.insert(quarters[i], i);

    std::vector<int> sems = read_tsteps_semesters(name.toStdString(), basepath.toStdString());
    for(size_t i = 0; i < sems.size(); ++i)
        calendar->mSemesters.insert(sems[i], i);

    std::vector<int> yrs = read_tsteps_years(name.toStdString(), basepath.toStdString());
    for(size_t i = 0; i < yrs.size(); ++i)
        calendar->mYears.insert(yrs[i], i);

    Q_ASSERT(calendar->mMonths.size() > 0);
    Q_ASSERT(calendar->mQuarters.size() > 0);
    Q_ASSERT(calendar->mSemesters.size() > 0);
    Q_ASSERT(calendar->mYears.size() > 0);

    return calendar;
}

Calendar *Calendar::build(std::shared_ptr<SQLiteOutputStorage> db)
{
    Calendar *calendar = new Calendar();

    std::vector<int> months, yrs,quarters,sems;
    db->importCalendar(months, quarters, sems, yrs);

    for(size_t i = 0; i < months.size(); ++i)
        calendar->mMonths.insert(months[i], i);

    for(size_t i = 0; i < quarters.size(); ++i)
        calendar->mQuarters.insert(quarters[i], i);

    for(size_t i = 0; i < sems.size(); ++i)
        calendar->mSemesters.insert(sems[i], i);

    for(size_t i = 0; i < yrs.size(); ++i)
        calendar->mYears.insert(yrs[i], i);

    return calendar;
}
