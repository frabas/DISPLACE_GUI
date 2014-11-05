#ifndef MEMORYINFO_H
#define MEMORYINFO_H

#include <stddef.h>

class MemoryInfoImpl;

class MemoryInfo
{
public:
    MemoryInfo();

    size_t rss() const { return mRss_kb; }
    size_t peakRss() const { return mPeakRss_kb; }

    bool update();
protected:
    size_t mRss_kb;
    size_t mPeakRss_kb;
};

#endif // MEMORYINFO_H
