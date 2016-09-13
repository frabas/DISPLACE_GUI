#ifndef R_SETTINGS_H
#define R_SETTINGS_H

#include <QString>
#include <QSettings>

namespace displace {
namespace R {

class Settings
{
    QSettings mSettings;
    static const char *const BasePathKey;

public:
    QString getScriptBasePath() const;
    QString getScriptPath(QString scriptName) const;

    void setScriptBasePath(const QString &path);
    void setScriptPath (const QString &scriptName, const QString &path);

    class Scripts {
    public:
        static const QString GenerateVesselsConfigFiles;
        static const QString RunVesselsConfigFiles;
    };
};

}
}

#endif // R_SETTINGS_H
