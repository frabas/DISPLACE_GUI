#include "settings.h"
#include <R/defaults.h>

using namespace displace::R;

const char *const Settings::BasePathKey = "R::basePath";

const QString Settings::Scripts::GenerateVesselsConfigFiles = "GenerateVesselsConfigFiles.R";
const QString Settings::Scripts::RunVesselsConfigFiles = "RunVesselsConfigFiles.R";
const QString Settings::Scripts::GenerateMetiersVariousFiles = "GenerateMetiersVariousFiles.R";
const QString Settings::Scripts::GenerateMetiersSelectivityPerStockFiles = "GenerateMetiersSelectivityPerStockFiles.R";
const QString Settings::Scripts::GenerateOtherCatchesOnNodes = "GenerateOtherCatchesOnNodes.R";
const QString Settings::Scripts::GenerateVesselFishingEffort = "GenerateVesselsFishingEffortGISLayers.R";

const QString Settings::Scripts::GeneratePopFeatures = "GeneratePopulationsFeatures.R";
const QString Settings::Scripts::GeneratePopAvailFromSurveys = "GeneratePopulationsStockAvaiFromTrawlSurveys.R";
const QString Settings::Scripts::GeneratePopAvailGisLayers = "GeneratePopulationsAvaiGISLayersFromStockAvai.R";
const QString Settings::Scripts::GeneratePopulationsConfigFiles = "GeneratePopulationsConfigFiles.R";
const QString Settings::Scripts::RunPopulationsConfigFiles = "RunPopulationsConfigFiles.R";
const QString Settings::Scripts::GeneratePopulationsInteractionMatrixFile = "GeneratePopulationsInteractionMatrixFile.R";
const QString Settings::Scripts::GeneratePopulationsVariousFiles = "GeneratePopulationsVariousFiles.R";
const QString Settings::Scripts::OverwriteCatchEquationParameters = "OverwriteCatchEquationParameters.R";
const QString Settings::Scripts::GenerateSimulationsConfigFiles = "GenerateSimulationsConfigFiles.R";

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

void Settings::resetScriptPath(const QString &scriptName)
{
    mSettings.remove("R::" + scriptName);
}

