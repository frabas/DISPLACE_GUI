#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSettings>

namespace displace {
namespace R {

class Settings
{
    QSettings mSettings;

public:
    QString getScriptPath(QString scriptName) const;
};

}
}

#endif // SETTINGS_H
