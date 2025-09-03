#ifndef MEMSTATS_H
#define MEMSTATS_H

#define _HAS_STD_BYTE 0
#include <cstddef>
#undef _HAS_STD_BYTE


/** @brief Memory Statistics Collector
 * */
class MemStats
{
private:
    size_t mVM;
    size_t mRSS;

public:
    MemStats();

    bool collect();

    size_t vm() const { return mVM; }
    size_t rss() const { return mRSS; }
};

#endif // MEMSTATS_H
