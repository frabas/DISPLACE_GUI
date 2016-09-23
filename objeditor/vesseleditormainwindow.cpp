#include "vesseleditormainwindow.h"
#include "ui_vesseleditormainwindow.h"

#include <scriptselectionform.h>
#include <settings.h>
#include <maplistadapter.h>
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
#include <R/env.h>

#include <appsettings.h>

using namespace displace;

const QString VesselEditorMainWindow::VesselsSpecFilename = "/FISHERIES/vessels_specifications_per_harbour_metiers.csv";

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

    auto onPushed = [this](QPushButton *button) {
        auto id = button->property("id").toInt();
        auto next = button->property("next").toInt();

        lightup (id, false);
        lightup (next, true);
    };

    mButtons.resize(30);

    mButtons[0] = ui->popScriptsPage->addScriptButton(tr("Generate Populations Features"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopFeatures), func, onPushed);
    mButtons[1] = ui->popScriptsPage->addScriptButton(tr("Generate Pop Avail from Surveys [optional]"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopAvailFromSurveys), func, onPushed);
    mButtons[2] = ui->popScriptsPage->addScriptButton(tr("Generate Pop Avail GIS layer [optional]"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopAvailGisLayers), func, onPushed);
    mButtons[3] = ui->popScriptsPage->addScriptButton(tr("Generate Populations Config Files [optional]"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopulationsConfigFiles), func, onPushed);
    mButtons[4] = ui->popScriptsPage->addScriptButton(tr("Run Populations Config Files"), R::Settings().getScriptPath(R::Settings::Scripts::RunPopulationsConfigFiles), func, onPushed);
    mButtons[5] = ui->popScriptsPage->addScriptButton(tr("Generate Populations Interaction Matrix File"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopulationsInteractionMatrixFile), func, onPushed);
    mButtons[6] = ui->popScriptsPage->addScriptButton(tr("Generate Populations Various Files"), R::Settings().getScriptPath(R::Settings::Scripts::GeneratePopulationsVariousFiles), func, onPushed);

    mButtons[7] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Vessels Fishing Effort GIS layers [optional]"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateVesselFishingEffort), func, onPushed);
    mButtons[8] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Config Files"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateVesselsConfigFiles), func, onPushed);
    mButtons[9] = ui->vesselsScriptsPage->addScriptButton(tr("Run Vessels Config Files"), R::Settings().getScriptPath(R::Settings::Scripts::RunVesselsConfigFiles), func, onPushed);
    mButtons[10] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Metiers Selectivity per Stock Files"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateMetiersSelectivityPerStockFiles), func, onPushed);
    mButtons[11] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Metiers Various Files"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateMetiersVariousFiles), func, onPushed);
    mButtons[12] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Other Catches On Nodes"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateOtherCatchesOnNodes), func, onPushed);
    mButtons[13] = ui->vesselsScriptsPage->addScriptButton(tr("Overwrite Catch Equation Parameters [optional]"), R::Settings().getScriptPath(R::Settings::Scripts::OverwriteCatchEquationParameters), func, onPushed);

    mButtons[14] = ui->simuScriptsPage->addScriptButton(tr("Generate Simulations Config Files"), R::Settings().getScriptPath(R::Settings::Scripts::GenerateSimulationsConfigFiles), func, onPushed);

    for (int i = 0; i < 14; ++i)
        setNext(i, i+1);

    lightup(0, true);

    // setup map
    auto center = AppSettings().getMapCenterPoint();

    ui->vesselsShapefileMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("OpenStreetMap", std::make_shared<qmapcontrol::MapAdapterOSM>()));
    ui->vesselsShapefileMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("Seamark", std::make_shared<qmapcontrol::MapAdapterOpenSeaMap>()));

    ui->vesselsShapefileMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(center.x(), center.y()));
    ui->vesselsShapefileMap->setZoom(7);

    ui->popShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("OpenStreetMap", std::make_shared<qmapcontrol::MapAdapterOSM>()));
    ui->popShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("Seamark", std::make_shared<qmapcontrol::MapAdapterOpenSeaMap>()));

    ui->popShapefilesMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(center.x(), center.y()));
    ui->popShapefilesMap->setZoom(7);

    mVesMapListAdapter = new MapListAdapter(ui->vesselsShapefileMap);
    ui->vesselsShapefileList->setModel(mVesMapListAdapter);

    mPopMapListAdapter = new MapListAdapter(ui->popShapefilesMap);
    ui->popShapefilesList->setModel(mPopMapListAdapter);
}

VesselEditorMainWindow::~VesselEditorMainWindow()
{
    delete ui;
}

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
    case 3:     // Vessels Specifications
        ui->vesselsCsvPage->setFilename(ui->gisPath->text() + VesselsSpecFilename);
        ui->vesselsCsvPage->load();
        mVesMapListAdapter->clearPaths();
        mVesMapListAdapter->addPath(ui->gisPath->text() + "/FISHERIES/SpatialLayers/");
        mVesMapListAdapter->refresh();
        break;
    case 1:     // Populations Specifications
        ui->popSpecs1->setFilename(ui->gisPath->text() + Pop1SpecFilename);
        ui->popSpecs1->load();
        ui->popSpecs2->setFilename(ui->gisPath->text() + Pop2SpecFilename);
        ui->popSpecs2->load();
        ui->popSpecs3->setFilename(ui->gisPath->text() + Pop3SpecFilename);
        ui->popSpecs3->load();
        mPopMapListAdapter->clearPaths();
        mPopMapListAdapter->addPath(ui->gisPath->text() + "/POPULATIONS/spatialLayers/");
        mPopMapListAdapter->refresh();
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

void VesselEditorMainWindow::lightup(int id, bool on)
{
    QFont f = mButtons[id]->font();
    f.setBold(on);
    mButtons[id]->setFont(f);
}

void VesselEditorMainWindow::setNext(int id, int next)
{
    mButtons[id]->setProperty("id", id);
    mButtons[id]->setProperty("next", next);
}

void VesselEditorMainWindow::on_configRScript_clicked()
{
    displace::R::Env env;

    QString dir = env.getRScriptHome();
    if (dir.isEmpty())
        dir = qApp->applicationDirPath();

    QString exe = QFileDialog::getOpenFileName(this, tr("Location of Rscript installation"), dir);
    if (!exe.isEmpty()) {
        QFileInfo info(exe);

        env.setRScriptHome(info.absolutePath());

        if (!env.check()) {
            QMessageBox::warning(this, tr("Object Editor setup check"),
                                 tr("Couldn't start Rscript. Please check the Path and executable."));
            return;
        }
    }
}
