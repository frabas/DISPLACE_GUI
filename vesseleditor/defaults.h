#ifndef VESSELS_DEFAULTS_H
#define VESSELS_DEFAULTS_H

#include <QString>

namespace displace {
namespace vesselsEditor {

    QString __attribute_deprecated__ getScriptsPath();
    QString __attribute_deprecated__ getDefaultConfigScriptPath();
    QString __attribute_deprecated__ getDefaultRunScriptPath();

    namespace defaults {
        QString getApplicationName();
        QString getApplicationPath();
        QString getGisDataPath();
        QString getIGraphValue();
    }
}
}

#endif // VESSELS_DEFAULTS_H
