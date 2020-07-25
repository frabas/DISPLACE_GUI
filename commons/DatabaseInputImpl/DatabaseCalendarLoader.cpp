//
// Created by Federico Fuga on 25/07/2020.
//

#include "DatabaseCalendarLoader.h"

#include "Calendar.h"
#include "db/ConfigTable.h"

#include <string>
#include <vector>

namespace sql = msqlitecpp::v2;

struct DatabaseCalendarLoader::Impl {
    std::shared_ptr<sql::Storage> db;
    displace::in::ConfigTable config;

    Impl(std::shared_ptr<sql::Storage> _db)
            : db(std::move(_db))
    {
        config.query(*db);
    }

    std::vector<int> load(std::string name);
};


DatabaseCalendarLoader::DatabaseCalendarLoader(std::shared_ptr<sql::Storage> db)
        : p(std::make_unique<Impl>(std::move(db)))
{
}

std::unique_ptr<Calendar> DatabaseCalendarLoader::loadCalendar()
{
    auto calendar = std::make_unique<Calendar>();

    auto days = p->load("days_2009_2015");
    auto months = p->load("months_2009_2015");
    auto quarters = p->load("quarters_2009_2015");
    auto semesters = p->load("semesters_2009_2015");
    auto years = p->load("years_2009_2015");

    calendar->setCalendar(std::move(months), std::move(quarters), std::move(semesters), std::move(years));

    return calendar;
}

std::vector<int> DatabaseCalendarLoader::Impl::load(std::string name)
{
    return config.getSteps(name);
}
