#include "env.h"

using namespace displace::R;


const QString Env::RSCRIPT_HOME_SETTING = "R_home";

Env::Env()
{

}

Env::~Env()
{

}

QProcessEnvironment Env::environment() const
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QString home = getRScriptHome();
    if (!home.isEmpty()) {
        QString path = env.value("PATH");
        path += home +";" + path;
        env.insert("PATH", path);
    }

    return env;
}

QString Env::getRScriptExe() const
{
    QString base = getRScriptHome();

#ifdef __unix
    return base + "Rscript";
#else
    return base + "Rscript.exe";
#endif
}

QString Env::getRScriptHome() const
{
    QString base;
    if (set.contains(RSCRIPT_HOME_SETTING)) {
        base = set.value(RSCRIPT_HOME_SETTING, "").toString() + "/";
    }

    return base;
}

void Env::setRScriptHome(QString home)
{
    set.setValue(RSCRIPT_HOME_SETTING, home);
}

