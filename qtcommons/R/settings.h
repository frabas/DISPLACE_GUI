#ifndef R_SETTINGS_H
#define R_SETTINGS_H

#include <QString>
#include <QSettings>

namespace displace {
namespace R {

class Settings
{
    QSettings mSettings;
    static const char *const BasePathKey;

public:
    Settings() : mSettings() {
    }
    QString getScriptBasePath() const;
    QString getScriptPath(QString scriptName) const;

    void setScriptBasePath(const QString &path);
    void setScriptPath (const QString &scriptName, const QString &path);
    void resetScriptPath (const QString &scriptName);

    class Scripts {
    public:
        static const QString GenerateVesselsConfigFiles;
        static const QString RunVesselsConfigFiles;
        static const QString GenerateMetiersVariousFiles;
        static const QString GenerateMetiersSelectivityPerStockFiles;
        static const QString GenerateOtherCatchesOnNodes;

        static const QString GenerateVesselFishingEffort;
        static const QString GeneratePopAvailFromSurveys;
        static const QString RunPopulationsConfigFiles;
        static const QString GeneratePopAvailGisLayers;
        static const QString GeneratePopFeatures;
        static const QString OverwriteCatchEquationParameters;
        static const QString GeneratePopulationsConfigFiles;
        static const QString GeneratePopulationsInteractionMatrixFile;
        static const QString GeneratePopulationsVariousFiles;

        static const QString GenerateSimulationsConfigFiles;

        static const QString GenerateHarboursFiles;

        static const QString GenerateShippingFiles;

        static const QString GenerateFishfarmsFiles;

        static const QString GenerateWindmillsFiles;

        static const QString GenerateBenthosFiles;
    };
};

}
}

#endif // R_SETTINGS_H
