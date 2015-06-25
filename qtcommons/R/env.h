#ifndef ENV_H
#define ENV_H

#include <QSettings>
#include <QProcessEnvironment>


namespace displace {
namespace R {

class Env
{
public:
    Env();
    ~Env();

    QProcessEnvironment environment() const;
    QString getRScriptExe() const;
    QString getRScriptHome() const;
    void setRScriptHome(QString home);

private:
    QSettings set;

    static const QString RSCRIPT_HOME_SETTING;
};

} // R
} // displace


#endif // ENV_H
