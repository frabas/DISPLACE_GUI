//
// Created by <fuga@studiofuga.com> on 01/03/16.
//

#ifndef DISPLACE_CALENDAR_H
#define DISPLACE_CALENDAR_H

namespace displace {
    namespace env {

        class Calendar {
        public:
            Calendar() { }

            virtual ~Calendar() noexcept = default;

            virtual bool isMonth(int tstep) const = 0;

            virtual bool isSemester(int tstep) const = 0;

            virtual bool isQuarted(int tstep) const = 0;

            virtual bool isYear(int tstep) const = 0;
        };

    }
}

#endif //DISPLACE_CALENDAR_H
