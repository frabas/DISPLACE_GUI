#ifndef SETTINGS_H
#define SETTINGS_H

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
};

}
}

#endif // SETTINGS_H
