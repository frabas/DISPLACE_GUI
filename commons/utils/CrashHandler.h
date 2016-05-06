//
// Created by <fuga@studiofuga.com> on 24/02/16.
//

#ifndef AVI_CRASHHANDLER_H
#define AVI_CRASHHANDLER_H

#include <signal.h>
#include <iosfwd>

class siginfo_t;

class CrashHandler {
public:
    CrashHandler();
    bool initialize();

private:
    static void CritErrHandler(int sig_num, siginfo_t * info, void * ucontext);
    static void UncaughtExceptionHandler();

    static void DoBacktrace (char **messages, size_t size, std::ostream &ss);
    static void DoSaveCrashdump (const std::string &msg);
};


#endif //AVI_CRASHHANDLER_H
