#ifndef COMMONS_GLOBAL_H
#define COMMONS_GLOBAL_H

#ifdef _WIN32

// This is only relevant for Windows. Though commons.dll shouldn't depend by Qt, it is harmless because on Windows
// we do require user to provide qt.
#if defined(_WIN32) || defined(_WIN64)
    #define Q_DECL_EXPORT __declspec(dllexport)
    #define Q_DECL_IMPORT __declspec(dllimport)
#else
    #define Q_DECL_EXPORT
    #define Q_DECL_IMPORT
#endif

#if defined(COMMONS_LIBRARY)
#  define COMMONSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define COMMONSSHARED_EXPORT Q_DECL_IMPORT
#endif
#else

#define
#endif


#endif // COMMONS_GLOBAL_H
