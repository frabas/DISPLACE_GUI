//
// Created by Federico Fuga on 17/03/16.
//

#ifndef DISPLACE_LEGACYCALENDAR_H
#define DISPLACE_LEGACYCALENDAR_H

#include <env/Calendar.h>

#include <map>
#include <string>

namespace displace {
    namespace env {

        class LegacyCalendar : public Calendar {

        public:
            LegacyCalendar() { }

            virtual bool isMonth(int tstep) const override {
                return mMonths.find(tstep) != mMonths.end();
            }

            virtual bool isSemester(int tstep) const override {
                return mSemesters.find(tstep) != mSemesters.end();
            }

            virtual bool isQuarted(int tstep) const override {
                return mQuarters.find(tstep) != mQuarters.end();
            }

            virtual bool isYear(int tstep) const override {
                return mYears.find(tstep) != mYears.end();
            }

            void insertMonth(int tstep, int n) {
                mMonths.insert(std::make_pair(tstep, n));
            }

            void insertQuarter(int tstep, int n) {
                mQuarters.insert(std::make_pair(tstep, n));
            }

            void insertSemester(int tstep, int n) {
                mSemesters.insert(std::make_pair(tstep, n));
            }

            void insertYear(int tstep, int n) {
                mYears.insert(std::make_pair(tstep, n));
            }

        private:
            std::map<int, int> mMonths;
            std::map<int, int> mQuarters;
            std::map<int, int> mSemesters;
            std::map<int, int> mYears;
        };

    }
}

#endif //DISPLACE_LEGACYCALENDAR_H
