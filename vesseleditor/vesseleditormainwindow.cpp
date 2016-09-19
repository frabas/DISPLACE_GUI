#include "vesseleditormainwindow.h"
#include "ui_vesseleditormainwindow.h"

#include <vesselsspec.h>
#include <vesselsspecmodel.h>
#include <scriptselectionform.h>
#include <R/env.h>
#include <settings.h>

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

VesselEditorMainWindow::VesselEditorMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VesselEditorMainWindow)
{
    ui->setupUi(this);

    mVesselsSpec = std::make_shared<VesselsSpec>();
    mVesselsSpecModel = std::make_shared<VesselsSpecModel>();
    mVesselsSpecModel->linkVesselsSpec(mVesselsSpec);

    mVesselsSpecProxyModel = new QSortFilterProxyModel(this);

    mVesselsSpecProxyModel->setSourceModel(mVesselsSpecModel.get());
    ui->tableView->setModel(mVesselsSpecProxyModel);

    QSettings s;
    ui->scriptsPath->setText(R::Settings().getScriptBasePath());

    ui->iGraph->setText(s.value("Vessel_LastIGraphValue", vesselsEditor::defaults::getIGraphValue()).toString());
    ui->inputPath->setText(s.value("Vessel_LastInputPath", vesselsEditor::defaults::getApplicationPath()).toString());
    ui->gisPath->setText(s.value("Vessel_LastGisPath", vesselsEditor::defaults::getGisDataPath()).toString());
    ui->applicationName->setText(s.value("Vessel_LastAppName", vesselsEditor::defaults::getApplicationName()).toString());

    ui->tabWidget->setCurrentIndex(0);
}

VesselEditorMainWindow::~VesselEditorMainWindow()
{
    delete ui;
}

void VesselEditorMainWindow::on_action_Load_Vessels_Spec_triggered()
{
    QSettings sets;
    QString lastpath;

    lastpath = sets.value("vessel_specs_lastpath", QDir::homePath()).toString();

    QString fn = QFileDialog::getOpenFileName(this, tr("Get Vessel Spec File"), lastpath);

    if (!fn.isEmpty()) {
        QFileInfo d (fn);
        sets.setValue("vessel_specs_lastpath", d.absoluteFilePath());

        std::ifstream f;

        f.open(fn.toStdString(), std::ios_base::in);
        if (f.fail()) {
            return;
        }

        if (!mVesselsSpec->loadFromSpecFile(f)) {
            return;
        }

        f.close();
    }
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

void VesselEditorMainWindow::loadCsv()
{
    QString fn = ui->gisPath->text() + "/FISHERIES/vessels_specifications_per_harbour_metiers.csv";

    std::ifstream f;

    f.open(fn.toStdString(), std::ios_base::in);
    if (f.fail()) {
        QMessageBox::warning(this, "CSV error",
                             tr("Cannot load %1: %2").arg(fn).arg(strerror(errno)));
        return;
    }

    try {
        if (!mVesselsSpec->loadFromSpecFile(f)) {
            throw std::runtime_error(strerror(errno));
        }
    } catch (std::exception &x) {
        QMessageBox::warning(this, "CSV error",
                             tr("Cannot load %1: %2").arg(fn).arg(x.what()));
        return;
    }

    f.close();
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
        loadCsv();
        break;
    }
}

