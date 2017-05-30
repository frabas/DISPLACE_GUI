#ifndef MEMSTATS_H
#define MEMSTATS_H

#include <cstddef>

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
