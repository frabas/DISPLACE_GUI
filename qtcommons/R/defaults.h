#ifndef R_DEFAULTS_H
#define R_DEFAULTS_H

#include <QString>

namespace displace {
namespace R {
namespace defaults {

    QString getRScriptsPath();
    QString getScriptFileName(const QString &name);

}
}
}

#endif // R_DEFAULTS_H
