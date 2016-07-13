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

bool Env::check() const
{
    QProcess p;
    QStringList args;
    args << "--version";

    // If process starts regularly, it will return error()==5 "unknown error".
    // If it cannot start, it will return error() == 0 "Failed to start".

    p.setEnvironment(environment().toStringList());
    p.start(getRScriptExe(), args);
    p.waitForFinished(-1);

    if (p.error() != 5 || p.exitCode() == -1) {
        return false;
    }

    return true;
}

