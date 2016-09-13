#include <defaults.h>

#include <R/settings.h>
#include <R/defaults.h>

#include <QApplication>
#include <QFile>
#include <QStandardPaths>

using namespace displace;

QString vesselsEditor::getScriptsPath()
{
#if !defined( __APPLE__ )
    return QApplication::applicationDirPath() + "/script";
#else
    return QApplication::applicationDirPath() + "/../Resources/scripts";
#endif
}

QString vesselsEditor::getDefaultConfigScriptPath()
{
    return R::defaults::getScriptFileName(R::Settings::Scripts::GenerateVesselsConfigFiles);
}

QString vesselsEditor::getDefaultRunScriptPath()
{
    return R::defaults::getScriptFileName(R::Settings::Scripts::RunVesselsConfigFiles);
}


QString vesselsEditor::defaults::getApplicationName()
{
    return "balticRTI";
}

QString vesselsEditor::defaults::getApplicationPath()
{
    return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]
            + "/GitHub/DISPLACE_input_balticRTI";
}

QString vesselsEditor::defaults::getGisDataPath()
{
    return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]
            + "/GitHub/DISPLACE_input_gis/balticRTI";
}

QString vesselsEditor::defaults::getIGraphValue()
{
    return "56";
}
