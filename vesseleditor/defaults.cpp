#include "defaults.h"

#include <QApplication>
#include <QFile>
#include <QStandardPaths>

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


QString displace::vesselsEditor::defaults::getApplicationName()
{
    return "balticRTI";
}

QString displace::vesselsEditor::defaults::getApplicationPath()
{
    return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]
            + "/GitHub/DISPLACE_input_balticRTI";
}

QString displace::vesselsEditor::defaults::getGisDataPath()
{
    return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]
            + "/GitHub/DISPLACE_input_gis/balticRTI";
}

QString displace::vesselsEditor::defaults::getIGraphValue()
{
    return "56";
}
