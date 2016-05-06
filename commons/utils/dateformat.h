//
// Created by <fuga@studiofuga.com> on 10/03/16.
//

#ifndef AVI_DATEFORMAT_H
#define AVI_DATEFORMAT_H

#include <string>
#include <boost/chrono.hpp>
#include <boost/format.hpp>

namespace utils {

template <typename T>
std::string fmtDate (typename boost::chrono::time_point<T> timepoint) {
    auto t = T::to_time_t(timepoint);
    tm tm;
    gmtime_r(&t, &tm);
    char suf[32];
    strftime(suf, 32, "%Y%m%d%H%M%S", &tm);

    return std::string {suf};
}

}

#endif //AVI_DATEFORMAT_H
