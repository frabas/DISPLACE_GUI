//
// Created by fuga on 21/12/2019.
//

#ifndef DISPLACE_SAFE_STRERROR_H
#define DISPLACE_SAFE_STRERROR_H

#include <string>
#include <cstring>

#if defined(_WIN32)

inline std::string safe_strerror(int errnumber)
{
    constexpr size_t MaxBuffer = 128;
    std::string theString(MaxBuffer, '\0');
    strerror_s(&theString[0], MaxBuffer, errnumber);
    return theString;
}

#else
inline std::string safe_strerror(int errnumber)
{
    constexpr size_t MaxBuffer = 128;
    std::string theString(MaxBuffer, '\0');
    strerror_r(errnumber, &theString[0], MaxBuffer);
    return theString;
}
#endif

#endif //DISPLACE_SAFE_STRERROR_H
