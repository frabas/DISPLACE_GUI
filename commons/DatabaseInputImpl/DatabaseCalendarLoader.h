//
// Created by Federico Fuga on 25/07/2020.
//

#ifndef DISPLACE_DATABASECALENDARLOADER_H
#define DISPLACE_DATABASECALENDARLOADER_H

#include "utils/spimpl.h"

#include <memory>

namespace msqlitecpp { namespace v2 { class Storage; }}

class Calendar;

class DatabaseCalendarLoader {
    struct Impl;
    spimpl::unique_impl_ptr<Impl> p;

public:
    DatabaseCalendarLoader(std::shared_ptr<msqlitecpp::v2::Storage> db);

    std::unique_ptr<Calendar> loadCalendar();
};


#endif //DISPLACE_DATABASECALENDARLOADER_H
