#ifndef FORMATS_GLOBALS_H
#define FORMATS_GLOBALS_H

#if defined (_WIN32)

#include <QtCore/qglobal.h>

#if defined(FORMATS_LIBRARY)
#  define FORMATSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FORMATSSHARED_EXPORT Q_DECL_IMPORT
#endif

#else
# define FORMATSSHARED_EXPORT
#endif

#endif // FORMATS_GLOBALS_H
