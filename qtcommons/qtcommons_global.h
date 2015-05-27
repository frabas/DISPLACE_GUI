#ifndef QTCOMMONS_GLOBAL_H
#define QTCOMMONS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTCOMMONS_LIBRARY)
#  define QTCOMMONSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTCOMMONSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTCOMMONS_GLOBAL_H