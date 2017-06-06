#ifndef R_DEFAULTS_H
#define R_DEFAULTS_H

#include <qtcommons_global.h>
#include <QString>

namespace displace {
namespace R {
namespace defaults {

    QString QTCOMMONSSHARED_EXPORT getRScriptsPath();
    QString QTCOMMONSSHARED_EXPORT getScriptFileName(const QString &name);

}
}
}

#endif // R_DEFAULTS_H
