#ifndef MEMORYINFO_H
#define MEMORYINFO_H

/* Abstract class - uses IMPL */

class MemoryInfoImpl;

class MemoryInfo
{
public:
    MemoryInfo();

    unsigned long free() const { return mFree_kb; }

    bool update();
protected:
    unsigned long mFree_kb;
};

#endif // MEMORYINFO_H
