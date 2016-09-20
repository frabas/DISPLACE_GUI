#include "vesseleditormainwindow.h"
#include "ui_vesseleditormainwindow.h"

#include <scriptselectionform.h>
#include <R/env.h>
#include <settings.h>
#include <csv/csvtablemodel.h>
#include <csv/csvimporter.h>
#include <csv/csvexporter.h>

#include <fstream>

#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>

#include <defaults.h>
#include <R/settings.h>

using namespace displace;

const QString VesselEditorMainWindow::VesselsSpecFilename = "/FISHERIES/vessels_specifications_per_harbour_metiers.csv";

// TODO assign more generic names...
const QString VesselEditorMainWindow::Pop1SpecFilename = "/POPULATIONS/DISPLACE_datainput_abundances_at_age_Baltic_22-32_2015.csv";
const QString VesselEditorMainWindow::Pop2SpecFilename = "/POPULATIONS/DISPLACE_datainput_DATRAS_CPUE_all_sp_per_length_per_haul_BITS_2013_2014_2015.csv";
const QString VesselEditorMainWindow::Pop3SpecFilename = "/POPULATIONS/DISPLACE_datainput_stock_parameters_Baltic_22-32.csv";

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

    ui->popSpecs1->setSeparator(QChar(','));
    ui->popSpecs2->setSeparator(QChar(','));
    ui->popSpecs3->setSeparator(QChar(','));
}

VesselEditorMainWindow::~VesselEditorMainWindow()
{
    delete ui;
}

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

bool VesselEditorMainWindow::runScript(QString script)
{

    displace::R::Env env;

    mProcess = new QProcess;

    connect(mProcess, SIGNAL(started()), this, SLOT(processStarted()));
    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
    connect(mProcess, SIGNAL(finished(int)), this, SLOT(processExit(int)));

    QStringList args;
    args << script;
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

    mProcess->setEnvironment(env.environment().toStringList());
    mProcess->setWorkingDirectory(env.getRScriptHome());
    mProcess->start(env.getRScriptExe(), args);

    qDebug() << "START:" << env.getRScriptExe() << args;

    return true;
}

void VesselEditorMainWindow::processStarted()
{
    ui->log->clear();
    ui->run->setDisabled(true);
}

void VesselEditorMainWindow::readOutput()
{
    QString t = mProcess->readAllStandardOutput();
    ui->log->appendPlainText(t + "\n");
}

void VesselEditorMainWindow::readError()
{
    QString t = mProcess->readAllStandardError();
    ui->log->appendHtml("<font color=\"#aa0000\">" + t + "</font>");
}

void VesselEditorMainWindow::processExit(int result)
{
    ui->run->setEnabled(true);
    qDebug() << "Completed: " << result;
}

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

void VesselEditorMainWindow::checkEnv()
{
    displace::R::Env env;
    if (!env.check()) {
        QMessageBox::warning(this, tr("Vessel Editor setup check"),
                             tr("Couldn't start Rscript. Please setup the Rscript path properly in the Settings screen."));
    }
}

void VesselEditorMainWindow::on_actionScripts_location_triggered()
{
    ScriptSelectionForm f;
    f.exec();
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
