#include "vesseleditormainwindow.h"
#include "ui_vesseleditormainwindow.h"

#include <scriptselectionform.h>
#include <settings.h>
#include <csv/csvtablemodel.h>
#include <csv/csvimporter.h>
#include <csv/csvexporter.h>

#include <fstream>

#include <Layer.h>
#include <LayerMapAdapter.h>
#include <MapAdapterOSM.h>
#include <MapAdapterOpenSeaMap.h>

#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QDebug>

#include <defaults.h>
#include <R/settings.h>

using namespace displace;

const QString VesselEditorMainWindow::VesselsSpecFilename = "/FISHERIES/vessels_specifications_per_harbour_metiers.csv";

// TODO assign more generic names...
const QString VesselEditorMainWindow::Pop1SpecFilename = "/POPULATIONS/Stock_abundances_at_age.csv";
const QString VesselEditorMainWindow::Pop2SpecFilename = "/POPULATIONS/Stock_spatial_research_survey_vessel_data.csv";
const QString VesselEditorMainWindow::Pop3SpecFilename = "/POPULATIONS/Stock_biological_traits.csv";

VesselEditorMainWindow::VesselEditorMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VesselEditorMainWindow)
{
    ui->setupUi(this);

    QSettings s;
    ui->scriptsPath->setText(R::Settings().getScriptBasePath());

    ui->iGraph->setText(s.value("Vessel_LastIGraphValue", vesselsEditor::defaults::getIGraphValue()).toString());
    ui->inputPath->setText(s.value("Vessel_LastInputPath", vesselsEditor::defaults::getApplicationPath()).toString());
    ui->gisPath->setText(s.value("Vessel_LastGisPath", vesselsEditor::defaults::getGisDataPath()).toString());
    ui->applicationName->setText(s.value("Vessel_LastAppName", vesselsEditor::defaults::getApplicationName()).toString());

    ui->tabWidget->setCurrentIndex(0);
    ui->popSpecsTab->setCurrentIndex(0);

    ui->popSpecs1->setSeparator(QChar(';'));
    ui->popSpecs2->setSeparator(QChar(';'));
    ui->popSpecs3->setSeparator(QChar(';'));

    auto func = [this](QStringList &args) {
        fillRScriptsArgs(args);
    };

    ui->vesselsScriptsPage->addScriptButton(tr("Generate Config Files"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateVesselsConfigFiles), func);
    ui->vesselsScriptsPage->addScriptButton(tr("Generate Data Files"), R::Settings().getScriptPath(R::Settings::Scripts::RunVesselsConfigFiles), func);
    ui->vesselsScriptsPage->addScriptButton(tr("Generate Metiers Files"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateMetiersVariousFiles), func);
    ui->vesselsScriptsPage->addScriptButton(tr("Generate Metiers Selectivity per Stock Files"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateMetiersSelectivityPerStockFiles), func);

    ui->popScriptsPage->addScriptButton(tr("Generate Vessels Fishing Effort GIS layers"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateVesselFishingEffort), func);
    ui->popScriptsPage->addScriptButton(tr("Generate Pop Avail from Surveys"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopAvailFromSurveys), func);
    ui->popScriptsPage->addScriptButton(tr("Run Populations Config Files"), R::Settings().getScriptPath(R::Settings::Scripts::RunPopulationsConfigFiles), func);
    ui->popScriptsPage->addScriptButton(tr("Generate Populations Avail GIS layer"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopAvailGisLayers), func);
    ui->popScriptsPage->addScriptButton(tr("Generate Populations Features"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopFeatures), func);

    // setup map
    ui->popShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("OpenStreetMap", std::make_shared<qmapcontrol::MapAdapterOSM>()));
    ui->popShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("Seamark", std::make_shared<qmapcontrol::MapAdapterOpenSeaMap>()));

    ui->popShapefilesMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(13.7507,43.7282));
    ui->popShapefilesMap->setZoom(7);

}

VesselEditorMainWindow::~VesselEditorMainWindow()
{
    delete ui;
}

#if 0
void VesselEditorMainWindow::on_run_clicked()
{
    displace::vesselsEditor::Settings s;
    auto script = s.getRunScriptPath();

    if (script.isEmpty() || !QFile(script).exists()) {
        QMessageBox::warning(this, tr("Run R Scrpt"),
                             tr("Please select an R Script on the 'script' field above."));
        return;
    }
    runScript(script);
}

void VesselEditorMainWindow::on_genConfig_clicked()
{
    displace::vesselsEditor::Settings s;
    auto script = s.getConfigScriptPath();

    if (script.isEmpty() || !QFile(script).exists()) {
        QMessageBox::warning(this, tr("Run R Scrpt"),
                             tr("Please select an R Script on the 'script' field above."));
        return;
    }
    runScript(script);
}
#endif

#if 0
void VesselEditorMainWindow::on_actionRscript_location_triggered()
{
    displace::R::Env env;

    QString dir = env.getRScriptHome();
    if (dir.isEmpty())
        dir = qApp->applicationDirPath();

    QString exe = QFileDialog::getOpenFileName(this, tr("Location of Rscript installation"), dir);
    if (!exe.isEmpty()) {
        QFileInfo info(exe);

        env.setRScriptHome(info.absolutePath());
        checkEnv();
    }
}
#endif


#if 0
void VesselEditorMainWindow::on_actionScripts_location_triggered()
{
    ScriptSelectionForm f;
    f.exec();
}
#endif

void VesselEditorMainWindow::on_browseInputPath_clicked()
{
    QSettings s;
    QString dir = s.value("Vessel_LastInputPath").toString();
    QFileInfo idir(dir);

    QString path = QFileDialog::getExistingDirectory(this, tr("Input Path"), idir.absolutePath());
    if (!path.isEmpty()) {
        s.setValue("Vessel_LastInputPath", path);
        ui->inputPath->setText(path);
    }
}


void VesselEditorMainWindow::on_browseGISPath_clicked()
{
    QSettings s;
    QString dir = s.value("Vessel_LastGisPath").toString();
    QFileInfo idir(dir);

    QString path = QFileDialog::getExistingDirectory(this, tr("Gis Data Path"), idir.absolutePath());
    if (!path.isEmpty()) {
        s.setValue("Vessel_LastGisPath", path);
        ui->gisPath->setText(path);
    }
}

#if 0
void VesselEditorMainWindow::on_genMetVar_clicked()
{
    auto script = R::Settings().getScriptPath(R::Settings::Scripts::GenerateMetiersVariousFiles);
    runScript(script);
}

void VesselEditorMainWindow::on_genMetSelectivity_clicked()
{
    auto script = R::Settings().getScriptPath(R::Settings::Scripts::GenerateMetiersSelectivityPerStockFiles);
    runScript(script);
}
#endif

void VesselEditorMainWindow::on_browseBasePath_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Select Base Scripts Path"), ui->scriptsPath->text());
    if (!file.isEmpty()) {
        ui->scriptsPath->setText(file);
    }
}

void VesselEditorMainWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    default:
        break;
    case 1:
        ui->vesselsCsvPage->setFilename(ui->gisPath->text() + VesselsSpecFilename);
        ui->vesselsCsvPage->load();
        break;
    case 3:
        ui->popSpecs1->setFilename(ui->gisPath->text() + Pop1SpecFilename);
        ui->popSpecs1->load();
        ui->popSpecs2->setFilename(ui->gisPath->text() + Pop2SpecFilename);
        ui->popSpecs2->load();
        ui->popSpecs3->setFilename(ui->gisPath->text() + Pop3SpecFilename);
        ui->popSpecs3->load();
        break;
    }
}

void VesselEditorMainWindow::fillRScriptsArgs(QStringList &args)
{
    /*
     * Dest_path application input_raw_path gis_path input_application_path
     *
     * Scripts are called with the following arguments:
     * # 1: Application name ("adriatic")
     * # 2: GIS Path, point to DISPLACE_input_gis
     * # 3: Application Path (IBM) i.e. pointing to displace_input_(application)
     * # 4: iGraph Parameter.
     */

    args << ui->applicationName->text();
    args << ui->gisPath->text() << ui->inputPath->text();
    args << ui->iGraph->text();
}
