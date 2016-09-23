#ifndef VESSELS_DEFAULTS_H
#define VESSELS_DEFAULTS_H

#include <QString>

namespace displace {
namespace vesselsEditor {

    QString __attribute__((deprecated)) getScriptsPath();
    QString __attribute__((deprecated)) getDefaultConfigScriptPath();
    QString __attribute__((deprecated)) getDefaultRunScriptPath();

    namespace defaults {
        QString getApplicationName();
        QString getApplicationPath();
        QString getGisDataPath();
        QString getIGraphValue();
    }
}
}

#endif // VESSELS_DEFAULTS_H
