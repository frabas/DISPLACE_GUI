#include "settings.h"
#include <defaults.h>

#include <R/settings.h>

using namespace displace::vesselsEditor;

QString Settings::getConfigScriptPath()
{
    return R::Settings().getScriptPath(R::Settings::Scripts::GenerateVesselsConfigFiles);
}

void Settings::setConfigScriptPath(const QString &path)
{
    R::Settings().setScriptPath(R::Settings::Scripts::GenerateVesselsConfigFiles, path);
}

QString Settings::getRunScriptPath()
{
    return R::Settings().getScriptPath(R::Settings::Scripts::RunVesselsConfigFiles);
}

void Settings::setRunScriptPath(const QString &path)
{
    R::Settings().setScriptPath(R::Settings::Scripts::RunVesselsConfigFiles, path);
}

