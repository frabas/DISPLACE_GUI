#include "vesseleditormainwindow.h"
#include "ui_vesseleditormainwindow.h"

#include <settings.h>
#include <mapshapefilelistadapter.h>
#include <csv/csvtablemodel.h>
#include <csv/csvimporter.h>
#include <csv/csvexporter.h>
#include <singlepointmapmodel.h>
#include <graphics/fishfarmgraphics.h>
#include <graphics/harbourgraphics.h>

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
const QString VesselEditorMainWindow::VesselsSpec2Filename = "/OTHERS/othercatchespercountry.csv";

const QString VesselEditorMainWindow::Pop1SpecFilename = "/POPULATIONS/Stock_abundances_at_age.csv";
const QString VesselEditorMainWindow::Pop2SpecFilename = "/POPULATIONS/Stock_spatial_research_survey_vessel_data.csv";
const QString VesselEditorMainWindow::Pop3SpecFilename = "/POPULATIONS/Stock_biological_traits.csv";

const QString VesselEditorMainWindow::ShippingFeaturesFilename = "/SHIPPING/shipsspe_features.csv";
const QString VesselEditorMainWindow::ShippingLanesLatFilename = "/SHIPPING/shipsspe_lanes_lat.csv";
const QString VesselEditorMainWindow::ShippingLanesLonFilename = "/SHIPPING/shipsspe_lanes_lon.csv";

const QString VesselEditorMainWindow::FishfarmsFeaturesFilename = "/FISHFARMS/fishfarms_features.csv";

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
    ui->tabVesselsSpecsTabWidget->setCurrentIndex(0);
    ui->tabShippingSpecsContainer->setCurrentIndex(0);
    ui->tabBenthosSubpages->setCurrentIndex(0);

    ui->popSpecs1->setSeparator(QChar(';'));
    ui->popSpecs2->setSeparator(QChar(';'));
    ui->popSpecs3->setSeparator(QChar(';'));

    auto func = [this](QStringList &args, const QString &script) {
        fillRScriptsArgs(args, script);
    };

    auto onPushed = [this](QPushButton *button) {
        auto id = button->property("id").toInt();
        auto next = button->property("next").toInt();

        lightup (id, false);
        lightup (next, true);
    };

    mButtons.resize(30);

    R::Settings settings;

    int btn = 0;
    mButtons[btn++] = ui->popScriptsPage->addScriptButton(tr("Generate Populations Features"), settings.getScriptPath(R::Settings::Scripts::GeneratePopFeatures), func, onPushed);
    mButtons[btn++] = ui->popScriptsPage->addScriptButton(tr("Generate Pop Avail from Surveys [optional]"), settings.getScriptPath(R::Settings::Scripts::GeneratePopAvailFromSurveys), func, onPushed);
    mButtons[btn++] = ui->popScriptsPage->addScriptButton(tr("Generate Pop Avail GIS layer [optional]"), settings.getScriptPath(R::Settings::Scripts::GeneratePopAvailGisLayers), func, onPushed);
    mButtons[btn++] = ui->popScriptsPage->addScriptButton(tr("Generate Populations Config Files [optional]"), settings.getScriptPath(R::Settings::Scripts::GeneratePopulationsConfigFiles), func, onPushed);
    mButtons[btn++] = ui->popScriptsPage->addScriptButton(tr("Run Populations Config Files"), settings.getScriptPath(R::Settings::Scripts::RunPopulationsConfigFiles), func, onPushed);
    mButtons[btn++] = ui->popScriptsPage->addScriptButton(tr("Generate Populations Interaction Matrix File"), settings.getScriptPath(R::Settings::Scripts::GeneratePopulationsInteractionMatrixFile), func, onPushed);
    mButtons[btn++] = ui->popScriptsPage->addScriptButton(tr("Generate Populations Various Files"), settings.getScriptPath(R::Settings::Scripts::GeneratePopulationsVariousFiles), func, onPushed);

    mButtons[btn++] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Vessels Fishing Effort GIS layers [optional]"), settings.getScriptPath(R::Settings::Scripts::GenerateVesselFishingEffort), func, onPushed);
    mButtons[btn++] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Config Files"), settings.getScriptPath(R::Settings::Scripts::GenerateVesselsConfigFiles), func, onPushed);
    mButtons[btn++] = ui->vesselsScriptsPage->addScriptButton(tr("Run Vessels Config Files"), settings.getScriptPath(R::Settings::Scripts::RunVesselsConfigFiles), func, onPushed);
    mButtons[btn++] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Metiers Selectivity per Stock Files"), settings.getScriptPath(R::Settings::Scripts::GenerateMetiersSelectivityPerStockFiles), func, onPushed);
    mButtons[btn++] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Metiers Various Files"), settings.getScriptPath(R::Settings::Scripts::GenerateMetiersVariousFiles), func, onPushed);
    mButtons[btn++] = ui->vesselsScriptsPage->addScriptButton(tr("Generate Other Catches On Nodes"), settings.getScriptPath(R::Settings::Scripts::GenerateOtherCatchesOnNodes), func, onPushed);
    mButtons[btn++] = ui->vesselsScriptsPage->addScriptButton(tr("Overwrite Catch Equation Parameters [optional]"), settings.getScriptPath(R::Settings::Scripts::OverwriteCatchEquationParameters), func, onPushed);

    mButtons[btn++] = ui->harboursScriptsPage->addScriptButton(tr("Generate Harbours Files"), settings.getScriptPath(R::Settings::Scripts::GenerateHarboursFiles), func, onPushed);

    mButtons[btn++] = ui->shippingsScriptsPage->addScriptButton(tr("Generate Shipping Files"), settings.getScriptPath(R::Settings::Scripts::GenerateShippingFiles), func, onPushed);

    mButtons[btn++] = ui->fishfarmsScriptsPage->addScriptButton(tr("Generate Fishfarms Files"), settings.getScriptPath(R::Settings::Scripts::GenerateFishfarmsFiles), func, onPushed);

    mButtons[btn++] = ui->benthosScriptsPage->addScriptButton(tr("Generate Benthos landscapes on Nodes"), settings.getScriptPath(R::Settings::Scripts::GenerateBenthosFiles), func, onPushed);

    mButtons[btn++] = ui->simuScriptsPage->addScriptButton(tr("Generate Simulations Config Files"), settings.getScriptPath(R::Settings::Scripts::GenerateSimulationsConfigFiles), func, onPushed);

    for (int i = 0; i < btn-1; ++i)
        setNext(i, i+1);

    lightup(0, true);

    // setup map
    AppSettings appsettings;
    auto center = appsettings.getMapCenterPoint();
    auto zoom = appsettings.getMapZoom();

    ui->vesselsShapefileMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("OpenStreetMap", std::make_shared<qmapcontrol::MapAdapterOSM>()));
    ui->vesselsShapefileMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("Seamark", std::make_shared<qmapcontrol::MapAdapterOpenSeaMap>()));

    ui->vesselsShapefileMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(center.x(), center.y()));
    ui->vesselsShapefileMap->setZoom(zoom);

    ui->popShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("OpenStreetMap", std::make_shared<qmapcontrol::MapAdapterOSM>()));
    ui->popShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("Seamark", std::make_shared<qmapcontrol::MapAdapterOpenSeaMap>()));

    ui->popShapefilesMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(center.x(), center.y()));
    ui->popShapefilesMap->setZoom(zoom);

    ui->shippingShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("OpenStreetMap", std::make_shared<qmapcontrol::MapAdapterOSM>()));
    ui->shippingShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("Seamark", std::make_shared<qmapcontrol::MapAdapterOpenSeaMap>()));

    ui->shippingShapefilesMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(center.x(), center.y()));
    ui->shippingShapefilesMap->setZoom(zoom);

    ui->harbourCsvPage->setSaveEnabled(false);

    ui->benthosShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("OpenStreetMap", std::make_shared<qmapcontrol::MapAdapterOSM>()));
    ui->benthosShapefilesMap->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("Seamark", std::make_shared<qmapcontrol::MapAdapterOpenSeaMap>()));

    ui->benthosShapefilesMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(center.x(), center.y()));
    ui->benthosShapefilesMap->setZoom(zoom);

    ui->benthosCsvPage1->setSaveEnabled(false);
    ui->benthosCsvPage2->setSaveEnabled(false);

    mVesMapListAdapter = new MapShapefileListAdapter(ui->vesselsShapefileMap);
    ui->vesselsShapefileList->setModel(mVesMapListAdapter);

    mPopMapListAdapter = new MapShapefileListAdapter(ui->popShapefilesMap);
    ui->popShapefilesList->setModel(mPopMapListAdapter);

    mBenthosMapListAdapter = new MapShapefileListAdapter(ui->benthosShapefilesMap);
    ui->benthosShapefilesList->setModel(mBenthosMapListAdapter);

    ui->fishfarmsCsvPage->enableMap();
    ui->fishfarmsCsvPage->setupMapInitialDisplayConditions(center, zoom);
    ui->fishfarmsCsvPage->setupIdLatLonCsvIndex(0,4,3);
    auto mapmodel = std::make_shared<SinglePointMapModel>(ui->fishfarmsCsvPage->getMapControlWidget());
    mapmodel->setGeometryBuilder([](float lat, float lon) {
        return std::make_shared<FishfarmGraphics>(lat, lon);
    });
    ui->fishfarmsCsvPage->setMapControlGraphicsModel(mapmodel);

    ui->harbourCsvPage2->enableMap();
    ui->harbourCsvPage2->setupMapInitialDisplayConditions(center, zoom);
    ui->harbourCsvPage2->setupIdLatLonCsvIndex(3,2,1);
    mapmodel = std::make_shared<SinglePointMapModel>(ui->harbourCsvPage2->getMapControlWidget());
    mapmodel->setGeometryBuilder([](float lat, float lon) {
        return std::make_shared<HarbourGraphics>(lat, lon);
    });
    ui->harbourCsvPage2->setMapControlGraphicsModel(mapmodel);
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
        R::Settings().setScriptBasePath(file);
        ui->scriptsPath->setText(file);
    }
}

void VesselEditorMainWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    default:
        break;
    case 3: {    // Vessels Specifications
        ui->vesselsCsvPage->setFilename(ui->gisPath->text() + VesselsSpecFilename);
        ui->vesselsCsvPage->load();
        ui->vesselsCsvPage_2->setFilename(ui->gisPath->text() + VesselsSpec2Filename);
        ui->vesselsCsvPage_2->load();
        mVesMapListAdapter->clearPaths();
        auto path = ui->gisPath->text() + "/FISHERIES/SpatialLayers/";
        mVesMapListAdapter->addPath(path);
        mVesMapListAdapter->refresh();
        ui->vesselsShapefilePath->setText(path);
        }
        break;
    case 1: {    // Populations Specifications
        ui->popSpecs1->setFilename(ui->gisPath->text() + Pop1SpecFilename);
        ui->popSpecs1->load();
        ui->popSpecs2->setFilename(ui->gisPath->text() + Pop2SpecFilename);
        ui->popSpecs2->load();
        ui->popSpecs3->setFilename(ui->gisPath->text() + Pop3SpecFilename);
        ui->popSpecs3->load();
        auto path = ui->gisPath->text() + "/POPULATIONS/spatialLayers/";
        mPopMapListAdapter->clearPaths();
        mPopMapListAdapter->addPath(path);
        mPopMapListAdapter->refresh();
        ui->popShapefilesPath->setText(path);
        }
        break;
    case 5:     // Harbour Specs
        ui->harbourCsvPage->setFilename(ui->gisPath->text() + "/POPULATIONS/Stock_prices_data.csv");
        ui->harbourCsvPage->load();
        ui->harbourCsvPage2->setFilename(ui->gisPath->text() + "/GRAPH/harbours.dat");
        ui->harbourCsvPage2->load();
        break;
    case 7:     // Shipping specs
        ui->ShippingCsvPage1->setFilename(ui->gisPath->text() + ShippingFeaturesFilename);
        ui->ShippingCsvPage1->load();
        ui->ShippingCsvPage2->setFilename(ui->gisPath->text() + ShippingLanesLatFilename);
        ui->ShippingCsvPage2->load();
        ui->ShippingCsvPage3->setFilename(ui->gisPath->text() + ShippingLanesLonFilename);
        ui->ShippingCsvPage3->load();
        break;
    case 9:     // Fishfarms specs
        ui->fishfarmsCsvPage->setFilename(ui->gisPath->text() + FishfarmsFeaturesFilename);
        ui->fishfarmsCsvPage->load();
        break;
    case 11:    // Benthos Specs
        ui->benthosCsvPage1->setFilename(ui->gisPath->text() + "/HABITATS/prop_loss_on_habitat_after_one_passage_per_metier_per_sz.csv");
        ui->benthosCsvPage1->load();
        ui->benthosCsvPage2->setFilename(ui->gisPath->text() + "/HABITATS/tot_benthos_biomass_on_habitat_per_node_per_sz.csv");
        ui->benthosCsvPage2->load();
        mBenthosMapListAdapter->clearPaths();
        mBenthosMapListAdapter->addPath(ui->gisPath->text() + "/HABITATS/");
        mBenthosMapListAdapter->refresh();
        break;
    }
}

void VesselEditorMainWindow::fillRScriptsArgs(QStringList &args, const QString &script)
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
    args << script;
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
