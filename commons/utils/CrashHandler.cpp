//
// Created by <fuga@studiofuga.com> on 24/02/16.
//

#define BOOST_CHRONO_VERSION 2

#include "CrashHandler.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>
#include <sstream>

#include <boost/format.hpp>
#include <iostream>

#include <cxxabi.h>
#include <fstream>

#include <stdio.h>

CrashHandler::CrashHandler()
{

}

bool CrashHandler::initialize()
{
    std::set_terminate(CrashHandler::UncaughtExceptionHandler);

    struct sigaction sa;
    sa.sa_sigaction = CrashHandler::CritErrHandler;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    if (sigaction(SIGSEGV, &sa, nullptr) != 0) {
        std::cerr << "Cannot set signal handler for SIGSEGV." << std::endl;
        return false;
    }
    if (sigaction(SIGABRT, &sa, nullptr) != 0) {
        std::cerr << "Cannot set signal handler for SIGABRT." << std::endl;
        return false;
    }
    return true;
}

void CrashHandler::UncaughtExceptionHandler()
{
    void *             array[50];
    char **            messages;
    size_t size;
    std::ostringstream ss;

    size = backtrace(array, 50);

    messages = backtrace_symbols(array, size);
    ss << "*** Unhandled exception detected " << std::endl;

    DoBacktrace(messages, size, ss);

    DoSaveCrashdump(ss.str());
    std::cerr << ss.str() << std::endl;

    free(messages);

    exit(EXIT_FAILURE);
}

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
typedef struct _sig_ucontext {
    unsigned long     uc_flags;
    struct ucontext   *uc_link;
    stack_t           uc_stack;
    struct sigcontext uc_mcontext;
    sigset_t          uc_sigmask;
} sig_ucontext_t;

void CrashHandler::CritErrHandler(int sig_num, siginfo_t * info, void * ucontext)
{
    void *             array[50];
    void *             caller_address;
    char **            messages;
    int                size;
    sig_ucontext_t *   uc;

    uc = (sig_ucontext_t *)ucontext;

    /* Get the address at the time the signal was raised */
#if defined(__i386__) // gcc specific
    caller_address = (void *) uc->uc_mcontext.eip; // EIP: x86 specific
#elif defined(__x86_64__) // gcc specific
    caller_address = (void *) uc->uc_mcontext.rip; // RIP: x86_64 specific
#else
    caller_address = (void *) uc->uc_mcontext.arm_cpsr; // TODO: Check this.
#endif

    std::ostringstream ss;

    ss << "*** Critical Signal detected " << std::endl;

    ss << boost::str(boost::format {"signal %d (%s), address is %p from %p\n" }
            % sig_num % strsignal(sig_num) % info->si_addr
            % (void *)caller_address);

    size = backtrace(array, 50);

    /* overwrite sigaction with caller's address */
    array[1] = caller_address;

    messages = backtrace_symbols(array, size);

    DoBacktrace(messages, size, ss);

    DoSaveCrashdump(ss.str());
    std::cerr << ss.str() << std::endl;

    free(messages);

    // Logger::logInfo("exit program");

    _exit(EXIT_FAILURE);
}

void CrashHandler::DoBacktrace(char **messages, size_t size, std::ostream &ss)
{
    /* skip first stack frame (points here) */
    for (size_t i = 1; i < size && messages != NULL; ++i)
    {
        char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

        // find parantheses and +address offset surrounding mangled name
        for (char *p = messages[i]; *p; ++p)
        {
            if (*p == '(')
            {
                mangled_name = p;
            }
            else if (*p == '+')
            {
                offset_begin = p;
            }
            else if (*p == ')')
            {
                offset_end = p;
                break;
            }
        }

        // if the line could be processed, attempt to demangle the symbol
        if (mangled_name && offset_begin && offset_end &&
            mangled_name < offset_begin)
        {
            *mangled_name++ = '\0';
            *offset_begin++ = '\0';
            *offset_end++ = '\0';

            int status;
            char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

            // if demangling is successful, output the demangled function name
            if (status == 0)
            {
                ss << "   (" << i << ") " << messages[i] << " : "
                << real_name << "+" << offset_begin << offset_end
                << std::endl;

            }
                // otherwise, output the mangled function name
            else
            {
                ss << "   (" << i << ") " << messages[i] << " : "
                << mangled_name << "+" << offset_begin << offset_end
                << std::endl;
            }
            free(real_name);
        }
            // otherwise, print the whole line
        else
        {
            ss << boost::str(boost::format{"   (%d) %s\n"} % i % messages[i]);
        }
    }
}

void CrashHandler::DoSaveCrashdump(const std::string &msg)
{
    auto logfilename = boost::str(boost::format{"crashdump-%l.log"} % time(0));
    auto logpath1 = logfilename;

    std::ofstream fs (logpath1, std::ios::out | std::ios::app);
    fs << msg << std::endl;
    fs.close();
}
