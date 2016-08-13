#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>

namespace displace {
namespace vesselsEditor {

    class Settings {
        QSettings settings;
    public:
        static const QString KeyConfigScriptPath;
        static const QString KeyRunScriptPath;

        QString getConfigScriptPath();
        void setConfigScriptPath(const QString &path);

        QString getRunScriptPath();
        void setRunScriptPath(const QString &path);
    };

}
}

#endif // SETTINGS_H
