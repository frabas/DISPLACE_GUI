#ifndef COMMONS_GLOBAL_H
#define COMMONS_GLOBAL_H

#if defined (_WIN32)
#include <QtCore/qglobal.h>

#if defined(COMMONS_LIBRARY)
#  define COMMONSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define COMMONSSHARED_EXPORT Q_DECL_IMPORT
#endif

#else
# define COMMONSSHARED_EXPORT
#endif




#endif // COMMONS_GLOBAL_H
