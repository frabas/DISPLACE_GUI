#include "vesseleditormainwindow.h"
#include "ui_vesseleditormainwindow.h"

#include <vesselsspec.h>
#include <vesselsspecmodel.h>
#include <R/env.h>

#include <fstream>

#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>

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
    ui->shapefilePath->setText(s.value("Vessel_LastShapefilePath").toString());
    ui->harbourFilePath->setText(s.value("Vessel_LastHarboursPath").toString());
    ui->outputPath->setText(s.value("Vessel_LastOutputPath").toString());

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
    QSettings s;
    auto script = s.value("VesselRScriptPath").toString();

    if (script.isEmpty()) {
        QMessageBox::warning(this, tr("Run R Scrpt"),
                             tr("Please select an R Script on the 'script' field above."));
        return;
    }

    displace::R::Env env;

    mProcess = new QProcess;

    connect(mProcess, SIGNAL(started()), this, SLOT(processStarted()));
    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
    connect(mProcess, SIGNAL(finished(int)), this, SLOT(processExit(int)));

    QStringList args;
    args << script;

    mProcess->setEnvironment(env.environment().toStringList());
    mProcess->setWorkingDirectory(env.getRScriptHome());
    mProcess->start(env.getRScriptExe(), args);

    qDebug() << "START:" << env.getRScriptExe() << args;
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
    QSettings s;
    QString dir = s.value("VesselRScriptPath").toString();
    QFileInfo idir(dir);

    QString script = QFileDialog::getExistingDirectory(this, tr("Location of R script"), idir.absolutePath());
    if (!script.isEmpty()) {
        s.setValue("VesselRScriptPath", script);
    }
}

void VesselEditorMainWindow::on_browseShapefilePath_clicked()
{
    QSettings s;
    QString dir = s.value("Vessel_LastShapefilePath").toString();
    QFileInfo idir(dir);

    QString path = QFileDialog::getOpenFileName(this, tr("ShapefilePath"), idir.absolutePath(),
                                                tr("Shapefiles (*.shp)"));
    if (!path.isEmpty()) {
        s.setValue("Vessel_LastShapefilePath", path);
        ui->shapefilePath->setText(path);
    }
}

void VesselEditorMainWindow::on_browseHarboursPath_clicked()
{
    QSettings s;
    QString dir = s.value("Vessel_LastHarboursPath").toString();
    QFileInfo idir(dir);

    QString path = QFileDialog::getOpenFileName(this, tr("Harbours file"), idir.absolutePath(),
                                                tr("Harbours file (*.dat)"));
    if (!path.isEmpty()) {
        s.setValue("Vessel_LastHarboursPath", path);
        ui->harbourFilePath->setText(path);
    }
}

void VesselEditorMainWindow::on_browseOutputPath_clicked()
{
    QSettings s;
    QString dir = s.value("Vessel_LastOutputPath").toString();
    QFileInfo idir(dir);

    QString path = QFileDialog::getExistingDirectory(this, tr("Output Path"), idir.absolutePath());
    if (!path.isEmpty()) {
        s.setValue("Vessel_LastOutputPath", path);
        ui->outputPath->setText(path);
    }
}
