#include "defaults.h"
#include <QStandardPaths>

using namespace displace::R;


QString defaults::getRScriptsPath()
{
    return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]
            + "/GitHub/DISPLACE_R_inputs";
}


QString defaults::getScriptFileName(const QString &name)
{
    return name + ".R";
}

