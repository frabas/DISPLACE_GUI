#ifndef VESSELCALENDAR_H
#define VESSELCALENDAR_H

struct VesselCalendar {
    int weekEndStartDay = 5;        // Saturday
    int weekEndEndDay = 6;          // Sunday
    int workStartHour = 4;          // 4am
    int workEndHour = 22;           // 10pm

    explicit VesselCalendar () {}
    explicit VesselCalendar (int westart,int  weend, int wrkstart, int wrkend)
        : weekEndStartDay(westart), weekEndEndDay(weend),
          workStartHour(wrkstart), workEndHour(wrkend) {}

    friend bool operator == (const VesselCalendar &cal1, const VesselCalendar &cal2) {
        return cal1.weekEndStartDay == cal2.weekEndStartDay &&
               cal1.weekEndEndDay == cal2.weekEndEndDay &&
               cal1.workStartHour == cal2.workStartHour &&
               cal1.workEndHour == cal2.workEndHour;
    }

    friend bool operator != (const VesselCalendar &cal1, const VesselCalendar &cal2) {
        return !(cal1 == cal2);
    }
};

#endif // VESSELCALENDAR_H
