#include "defaults.h"

#include <QApplication>
#include <QFile>

QString displace::vesselsEditor::getScriptsPath()
{
#if !defined( __APPLE__ )
    return QApplication::applicationDirPath() + "/script";
#else
    return QApplication::applicationDirPath() + "/../Resources/scripts";
#endif
}

QString displace::vesselsEditor::getDefaultConfigScriptPath()
{
    return getScriptsPath() + "/GenerateVesselsConfigFile.R";
}

QString displace::vesselsEditor::getDefaultRunScriptPath()
{
    return getScriptsPath() + "/RunVesselsConfigFiles.R";
}
