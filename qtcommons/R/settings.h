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
    void resetScriptPath (const QString &scriptName);

    class Scripts {
    public:
        static const QString GenerateVesselsConfigFiles;
        static const QString RunVesselsConfigFiles;
        static const QString GenerateMetiersVariousFiles;
        static const QString GenerateMetiersSelectivityPerStockFiles;
    };
};

}
}

#endif // R_SETTINGS_H
