#ifndef VESSELCALENDAR_H
#define VESSELCALENDAR_H

struct VesselCalendar {
    int weekEndStartDay = 5;        // Saturday
    int weekEndEndDay = 6;          // Sunday
    int workStartHour = 4;          // 4am
    int workEndHour = 22;           // 10pm
};

#endif // VESSELCALENDAR_H
