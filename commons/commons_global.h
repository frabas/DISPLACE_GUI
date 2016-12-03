#ifndef COMMONS_GLOBAL_H
#define COMMONS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(COMMONS_LIBRARY)
#  define COMMONSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define COMMONSSHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // COMMONS_GLOBAL_H
