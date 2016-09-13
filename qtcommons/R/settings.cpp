#include "settings.h"
#include <R/defaults.h>

using namespace displace::R;

const char *const Settings::BasePathKey = "R::basePath";

const QString Settings::Scripts::GenerateVesselsConfigFiles = "GenerateVesselsConfigFiles.R";
const QString Settings::Scripts::RunVesselsConfigFiles = "RunVesselsConfigFiles.R";

QString Settings::getScriptBasePath() const
{
    return mSettings.value(BasePathKey, defaults::getRScriptsPath()).toString();
}

QString Settings::getScriptPath(QString scriptName) const
{
    return mSettings.value("R::" + scriptName, getScriptBasePath() + "/" + scriptName).toString();
}

void Settings::setScriptBasePath(const QString &path)
{
    mSettings.setValue(BasePathKey, path);
}

void Settings::setScriptPath(const QString &scriptName, const QString &path)
{
    mSettings.setValue("R::" + scriptName, path);
}

