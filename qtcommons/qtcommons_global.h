#ifndef QTCOMMONS_GLOBAL_H
#define QTCOMMONS_GLOBAL_H

#if defined (_WIN32)

#include <QtCore/qglobal.h>

#if defined(QTCOMMONS_LIBRARY)
#  define QTCOMMONSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTCOMMONSSHARED_EXPORT Q_DECL_IMPORT
#endif

#else   // Not Win32

# define QTCOMMONSSHARED_EXPORT

#endif

#endif // QTCOMMONS_GLOBAL_H
