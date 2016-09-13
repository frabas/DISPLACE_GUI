#include "settings.h"
#include <R/defaults.h>

using namespace displace::R;

QString Settings::getScriptPath(QString scriptName) const
{
    return mSettings.value("R::" + scriptName, defaults::getRScriptsPath() + "/" + scriptName).toString();
}

