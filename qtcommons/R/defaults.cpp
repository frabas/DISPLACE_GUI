#include "defaults.h"
#include <QStandardPaths>

using namespace displace::R;


QString defaults::getRScriptsPath()
{
    return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]
            + "/GitHub/DISPLACE_R_inputs";
}

QString defaults::getGenerateVesselsScript()
{
    return defaults::getRScriptsPath() + "/GenerateVesselsConfigFiles.R";
}

QString defaults::getRunVesselsScript()
{
    return defaults::getRScriptsPath() + "/RunVesselsConfigFiles.R";
}
