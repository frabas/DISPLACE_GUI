#ifndef VES_SETTINGS_H
#define VES_SETTINGS_H

#include <QSettings>
#include <QString>

namespace displace {
namespace vesselsEditor {

// TODO remove this class.

    class Settings {
        QSettings settings;
    public:
        QString getConfigScriptPath();
        void setConfigScriptPath(const QString &path);

        QString getRunScriptPath();
        void setRunScriptPath(const QString &path);
    };

}
}

#endif // SETTINGS_H
