#ifndef ENV_H
#define ENV_H

#include <qtcommons_global.h>
#include <QSettings>
#include <QProcessEnvironment>


namespace displace {
namespace R {

class QTCOMMONSSHARED_EXPORT Env
{
public:
    Env();
    ~Env();

    QProcessEnvironment environment() const;
    QString getRScriptExe() const;
    QString getRScriptHome() const;
    void setRScriptHome(QString home);

    bool check() const;
private:
    QSettings set;

    static const QString RSCRIPT_HOME_SETTING;
};

} // R
} // displace


#endif // ENV_H
