// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef CALENDAR_H
#define CALENDAR_H

#include <QString>
#include <QMap>
#include <memory>


class SQLiteOutputStorage;

class Calendar
{
public:
    Calendar();

    bool isMonth(int tstep) const {
        return mMonths.contains(tstep);
    }
    int getMonth (int tstep) const {
        return mMonths.lowerBound(tstep).value();
    }

    bool isSemester(int tstep) const {
        return mSemesters.contains(tstep);
    }
    int getSemester(int tstep) const {
        return mSemesters.lowerBound(tstep).value();
    }

    bool isQuarted(int tstep) const {
        return mQuarters.contains(tstep);
    }
    int getQuarter (int tstep) const {
        return mQuarters.lowerBound(tstep).value();
    }

    bool isYear(int tstep) const {
        return mYears.contains(tstep);
    }
    int getYear(int tstep) const {
        return mYears.lowerBound(tstep).value();
    }

    static Calendar *load(QString basepath, QString name);
    static Calendar *build (std::shared_ptr<SQLiteOutputStorage> db);
    static QString dayToString (int day) {
        if (day < 0 || day > 6)
            return "?";
        return days[day];
    }

private:
    QMap<int, int> mMonths;
    QMap<int, int> mQuarters;
    QMap<int, int> mSemesters;
    QMap<int, int> mYears;

    static const QString days[];
    static const QString months[];
};

#endif // CALENDAR_H
