#include "memoryinfo.h"

#include <QString>
#include <QStringList>
#include <QProcess>

MemoryInfo::MemoryInfo()
{
}

bool MemoryInfo::update()
{
#if defined(__win32)
    MEMORYSTATUSEX memory_status;
    ZeroMemory(&memory_status, sizeof(MEMORYSTATUSEX));
    memory_status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memory_status)) {
        mFree_kb = memory_status.ullTotalPhys / 1024;
    } else {
        mFree_kb = 0;
    }

    return true;
#elif defined (__linux)
    QProcess p;
    p.start("awk", QStringList() << "/MemTotal/ { print $2 }" << "/proc/meminfo");
    p.waitForFinished();
    QString memory = p.readAllStandardOutput();
    mFree_kb = memory.toLong();
    p.close();

    return true;
#else
    mFree_kb = 0;
    return false;
#endif
}
