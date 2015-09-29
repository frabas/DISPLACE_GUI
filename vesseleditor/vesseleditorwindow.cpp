#include "vesseleditorwindow.h"
#include "ui_vesseleditorwindow.h"

#include <csv/csvimporter.h>
#include <csv/csvtablemodel.h>
#include <csv/csvexporter.h>

#include <R/env.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QModelIndex>
#include <QtConcurrent>
#include <QProgressDialog>
#include <QDebug>

#include <iostream>

VesselEditorWindow::VesselEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VesselEditorWindow),
    mFilename(),
    mData(),
    mModel(0),
    colVid(-1), colMet(-1), colHarb(-1),
    mProcess(0),
    mDestFile("fgrounds-XXXXXX.dat"),
    mParFile("param_fgrounds-XXXXXX.dat"),
    mDirty(false)
{
    ui->setupUi(this);

    connect (this, SIGNAL(dataDirty()), this, SLOT(dataDirtyChanged()));

    ui->action_Log_Window->setChecked(false);
    ui->dockLogWindow->setVisible(false);
    ui->actionGenerate->setEnabled(false);

    mDestFile.open();
    mParFile.open();

    mDestFile.close();
    mParFile.close();

    mDirtyIndicator = new QLabel;
    QPixmap ic (":/icons/save.png");
    mDirtyIndicator->setPixmap(ic.scaled(16,16));
    statusBar()->addPermanentWidget(mDirtyIndicator);
    mDirtyIndicator->setVisible(false);
    emit dataDirty();

    QSettings set;
    restoreGeometry(set.value("mainGeometry").toByteArray());
    restoreState(set.value("mainState").toByteArray());

    checkEnv();
}

VesselEditorWindow::~VesselEditorWindow()
{
    delete ui;
}

void VesselEditorWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    if (mDirty) {
        int r = QMessageBox::question(this, tr("Close VesselEditor?"),
                                      tr("The parameters file has unsaved changes. If you proceed, the changes will be lost. Proceed?"),
                                      tr("No"), tr("Yes, Proceed and lose the changes"));
        if (r == 0) {
            event->ignore();
            return;
        }
    }

    QSettings sets;
    sets.setValue("mainGeometry", saveGeometry());
    sets.setValue("mainState", saveState());

    event->accept();
}

void VesselEditorWindow::checkEnv()
{
    displace::R::Env env;

    QProcess p;
    QStringList args;
    args << "--version";

    // If process starts regularly, it will return error()==5 "unknown error".
    // If it cannot start, it will return error() == 0 "Failed to start".

    p.setEnvironment(env.environment().toStringList());
    p.start(env.getRScriptExe(), args);
    p.waitForFinished(-1);

    if (p.error() != 5 || p.exitCode() == -1) {
        QMessageBox::warning(this, tr("Vessel setup check"),
                             tr("Couldn't start Rscript. Please setup the Rscript path properly in the Settings screen."));
        return;
    }

    QFileInfo file(getScriptPath());

    if (!file.exists() || !file.isReadable()) {
        QMessageBox::warning(this, tr("Vessel setup check"),
                             tr("Vessel generation script (%1) is not accessible or is missing. Please fix the path in the Settings screen.")
                             .arg(file.fileName()));
        return;
    }
}

QString VesselEditorWindow::getScriptPath()
{
    QSettings set;
    return set.value("VesselEditor.script", qApp->applicationDirPath() + "/scripts/gen_vessels.R").toString();
}

void VesselEditorWindow::on_action_Open_triggered()
{
    QSettings set;
    QString dir = set.value("VesselEditor.LastPath", QDir::homePath()).toString();
    QString filter = set.value("VesselEditor.filter", "").toString();

    QString file = QFileDialog::getOpenFileName(this, tr("Open geodata CSV file e.g. param_fgrounds.dat"),
                                                dir, tr("Dat Files (*.dat);;CSV Files (*.csv);;Text Files (*.txt);;All Files (*.*)"),
                                                &filter);

    if (!file.isEmpty()) {
        mFilename = file;

        load(mFilename);

        QFileInfo info(file);
        set.setValue("VesselEditor.LastPath", info.absoluteFilePath());
        set.setValue("VesselEditor.filter", filter);



    }
}

void VesselEditorWindow::load(QString filename)
{
    CsvImporter importer;
    try {
        importer.setSeparator(' ');
        QList<QStringList> data = importer.import(filename);
        mData = std::shared_ptr<QList<QStringList>>( new QList<QStringList>(data));
    } catch (QException &ex) {
        QMessageBox::warning(this, tr("Cannot load file"), QString(tr("There was an error parsing the file. %1")).arg(ex.what()));
        return;
    }

    if (mModel != nullptr)
        delete mModel;

    mModel = new CsvTableModel(mData);
    mModel->setFirstLineHeaders(true);
    ui->table->setModel(mModel);
    connect (mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

    colVid = colMet = colHarb= -1;
    QStringList h = mData->at(0);
    for (int i = 0; i < h.size(); ++i) {
        if (h[i] == "VE_REF")
            colVid = i;
        else if (h[i] == "LE_MET_level6")
            colMet = i;
        else if (h[i] == "land_harb")
            colHarb = i;
    }

    if (colVid == -1 || colMet == -1 || colHarb == -1) {
        QMessageBox::warning(this, tr("Cannot load file"), QString(tr("The header of the file is malformed.")));
        return;
    }

    mDirty = false;
    emit dataDirty();
    ui->actionGenerate->setEnabled(true);

    updateKeys();
}




void VesselEditorWindow::updateKeys()
{
    QSet<QString> vid, met, harb;

    for (int i = 1; i < mData->length(); ++i) { // skip header
        vid.insert(mData->at(i).at(colVid));
        met.insert(mData->at(i).at(colMet));
        harb.insert(mData->at(i).at(colHarb));
    }

    QStringList vl(vid.toList());
    vl.sort();
    ui->varSelect->clear();
    ui->varSelect->addItems(vl);

    vl = met.toList();
    vl.sort();
    ui->areaSelect->clear();
    ui->areaSelect->addItems(vl);

    vl = harb.toList();
    vl.sort();
    ui->adimSelect->clear();
    ui->adimSelect->addItems(vl);
}

void VesselEditorWindow::genSampleFile()
{
    ui->plot->clearGraphs();

    QString v = ui->varSelect->currentText();
    QString m = ui->areaSelect->currentText();
    QString h = ui->adimSelect->currentText();

    qDebug() << mDestFile.fileName() << mParFile.fileName() << v << m << h;

    if (v.isEmpty() || m.isEmpty() || h.isEmpty())
        return;

    saveTempParamFile();
    generate(mParFile.fileName(), mDestFile.fileName(), v, m, h);
}

void VesselEditorWindow::loadSampleFileGraph(QString name)
{
    QFile f(name);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "***" << f.errorString();
        return;
    }

    bool ok;
    QTextStream strm(&f);
    strm.readLine();        // drop the first line

    // The first three lines are the thresholds.
    double th1v,th2v,th3v;
    try {
        th1v = parseThreshold(strm.readLine());
        th2v = parseThreshold(strm.readLine());
        th3v = parseThreshold(strm.readLine());
    } catch (std::invalid_argument &) {
        f.close();
        qDebug() << "*** invalid conversion of threshold.";
        return;
    }

    QVector<double> dt, x;
    while (!strm.atEnd()) {
        QString l = strm.readLine();
        double v = l.toDouble(&ok);
        if (!ok) {
            // error...
            f.close();
            qDebug() << "*** invalid conversion " << l;
            return;
        }

        x.push_back(dt.size());
        dt.push_back(v);
    }

    ui->plot->clearItems();
    ui->plot->clearGraphs();
    QCPGraph *graph = ui->plot->addGraph();
    graph->setData(x,dt);
    graph->rescaleAxes();

    if (th1v != NAN) {
        QCPItemLine *th1 = new QCPItemLine(ui->plot);
        th1->start->setCoords(x.at(0), th1v);
        th1->end->setCoords(x.at(x.size()-1), th1v);
        th1->setPen(QPen(Qt::red));
        ui->plot->addItem(th1);
    }

    if (th2v != NAN) {
        QCPItemLine *th2 = new QCPItemLine(ui->plot);
        th2->start->setCoords(x.at(0), th2v);
        th2->end->setCoords(x.at(x.size()-1), th2v);
        th2->setPen(QPen(Qt::magenta));
        ui->plot->addItem(th2);
    }

    if (th3v != NAN) {
        QCPItemLine *th3 = new QCPItemLine(ui->plot);
        th3->start->setCoords(x.at(0), th3v);
        th3->end->setCoords(x.at(x.size()-1), th3v);
        th3->setPen(QPen(Qt::darkGreen));
        ui->plot->addItem(th3);
    }

    ui->plot->replot();

    f.close();
}

void VesselEditorWindow::saveTempParamFile()
{
    CsvExporter exporter;
    exporter.setSeparator(' ');
    exporter.exportFile(mParFile.fileName(),*mData);
}

void VesselEditorWindow::generate(QString param_file, QString dest, QString vid, QString met, QString harb)
{
    QString script_file = getScriptPath();

    mProcess = new QProcess;

    connect(mProcess, SIGNAL(started()), this, SLOT(processStarted()));
    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
    connect(mProcess, SIGNAL(finished(int)), this, SLOT(processExit(int)));

    QStringList args;
    args << script_file << dest << vid << met << harb << param_file;

    displace::R::Env env;
    mProcess->setEnvironment(env.environment().toStringList());
    mProcess->setWorkingDirectory(env.getRScriptHome());
    mProcess->start(env.getRScriptExe(), args);

    qDebug() << "START:" << env.getRScriptExe() << args;
 }

void VesselEditorWindow::generateAll(QString outpath)
{
    mExporterWorkerDialog = new QProgressDialog(this);
    mExporterWorkerDialog->show();
    connect (this, SIGNAL(exportProgress(int)), mExporterWorkerDialog, SLOT(setValue(int)));
    connect (this, SIGNAL(exportTotalChanged(int)), mExporterWorkerDialog, SLOT(setMaximum(int)));

    mExportWorker = QtConcurrent::run(this, &VesselEditorWindow::generateAllWorker, outpath);
    connect (&mExportWorkerWatcher, SIGNAL(finished()), this, SLOT(exportFinished()));

    mExportWorkerWatcher.setFuture(mExportWorker);
}

QString VesselEditorWindow::generateAllWorker(QString outpath)
{
    saveTempParamFile();

    QString script_file = getScriptPath();

    int n = ui->areaSelect->count() * ui->varSelect->count() * ui->adimSelect->count();
    emit exportTotalChanged(n);

    displace::R::Env env;

    int ngood = 0;
    n=0;
    for (int vid = 0; vid < ui->varSelect->count(); ++vid) {
        for (int met = 0; met < ui->areaSelect->count(); ++met) {
            for (int harb = 0; harb < ui->adimSelect->count(); ++harb) {
                QString vesselid = ui->varSelect->itemText(vid);
                QString metier = ui->areaSelect->itemText(met);
                QString harbour = ui->adimSelect->itemText(harb);

                QString outname = QString("%1-%2-%3.dat")
                        .arg(vesselid)
                        .arg(metier)
                        .arg(harbour);

                QString dest = outpath + "/" + outname;

                mProcess = new QProcess;

                connect(mProcess, SIGNAL(started()), this, SLOT(processStarted()));
                connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
                connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));

                QStringList args;
                args << script_file << dest << vesselid << metier << harbour << mParFile.fileName();

                qDebug() << args;

                mProcess->setEnvironment(env.environment().toStringList());
                mProcess->setWorkingDirectory(env.getRScriptHome());
                mProcess->start(env.getRScriptExe(), args);
                mProcess->waitForFinished(-1);

                if (mProcess->exitCode() == 0)
                    ++ngood;
                ++n;
                emit exportProgress(n);
            }
        }
    }

    return tr("%1 files generated out of %2 possible combinations.").arg(ngood).arg(n);
}

double VesselEditorWindow::parseThreshold(QString l) throw (std::invalid_argument)
{
    bool ok;
    double th1v;
    if (l.trimmed() != "NA") {
        th1v = l.toDouble(&ok);
        if (!ok) {
            throw std::invalid_argument("");
        }
    } else {
        th1v = NAN;
    }

    return th1v;
}

void VesselEditorWindow::on_varSelect_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    genSampleFile();
}

void VesselEditorWindow::on_areaSelect_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    genSampleFile();
}

void VesselEditorWindow::on_adimSelect_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    genSampleFile();
}

void VesselEditorWindow::processStarted()
{
    ui->logText->appendHtml(QString("<font color=\"#0000bb\">%1 %2</font>")
                            .arg(tr("Starting:"))
                            .arg(mProcess->arguments().join(" ")));
}

void VesselEditorWindow::readOutput()
{
    QString t = mProcess->readAllStandardOutput();
    ui->logText->appendPlainText(t + "\n");
}

void VesselEditorWindow::readError()
{
    QString t = mProcess->readAllStandardError();
    ui->logText->appendHtml("<font color=\"#aa0000\">" + t + "</font>");
}

void VesselEditorWindow::processExit(int code)
{
    if (mProcess->error() != 5) {
        QString t(tr("Cannot start Rscript: %1").arg(mProcess->errorString()));
        ui->logText->appendHtml("<font color=\"#aa0000\">" + t + "</font>");
        QMessageBox::warning(this, tr("Cannot start Rscript"),
                             tr("Cannot start Rscript: %1").arg(mProcess->errorString()));
    } else {
        if (code == 0) {
            QString t(tr("Program exited with result code: %1").arg(code));
            ui->logText->appendHtml("<font color=\"#0000aa\">" + t + "</font>");
            loadSampleFileGraph(mDestFile.fileName());
        } else {
            QString t(tr("Program exited with result code: %1").arg(code));
            ui->logText->appendHtml("<font color=\"#aa0000\">" + t + "</font>");
            statusBar()->showMessage(QString(tr("R Script exited with exit code %1")).arg(code), 5000);
        }
        ui->logText->appendPlainText("\n");
    }

    delete mProcess;
    mProcess = 0;
}

void VesselEditorWindow::dataChanged(QModelIndex from, QModelIndex to, QVector<int> roles)
{
    Q_UNUSED(roles);

    mDirty = true;
    emit dataDirtyChanged();

    bool need_refresh = false;
    for (int row = from.row(); row <= to.row(); ++row) {
        // Notice: Headers are at row #0
        if (mData->at(row+1).at(colVid) == ui->areaSelect->currentText() ||
            mData->at(row+1).at(colMet) == ui->adimSelect->currentText() ||
            mData->at(row+1).at(colHarb) == ui->varSelect->currentText() )
            need_refresh = true;
    }

    if (need_refresh)
        genSampleFile();
}

void VesselEditorWindow::dataDirtyChanged()
{
    mDirtyIndicator->setVisible(mDirty);
    ui->action_Save->setEnabled(mDirty);
}

void VesselEditorWindow::exportFinished()
{
    QString msg = mExportWorkerWatcher.result();
    if (msg.isEmpty()) {
        QMessageBox::information(this, tr("File export complete"), tr("All files has been generated."));
    } else {
        QMessageBox::warning(this, tr("File export complete"), tr("%1").arg(msg));
    }

    if (mExporterWorkerDialog) {
        mExporterWorkerDialog->close();
        delete mExporterWorkerDialog;
        mExporterWorkerDialog = 0;
    }
}

void VesselEditorWindow::on_action_Log_Window_triggered()
{
    ui->dockLogWindow->setVisible(ui->action_Log_Window->isChecked());
}

void VesselEditorWindow::on_clearLog_clicked()
{
    ui->logText->clear();
}

void VesselEditorWindow::on_dockLogWindow_visibilityChanged(bool visible)
{
    QSignalBlocker b(ui->action_Log_Window);
    ui->action_Log_Window->setChecked(visible);
}

void VesselEditorWindow::on_actionQuit_triggered()
{
    close();
}

void VesselEditorWindow::on_action_Save_triggered()
{
    QSettings set;
    QString dir = set.value("TsEditor.LastPath", QDir::homePath()).toString();
    QString filter = set.value("TsEditor.filter", "").toString();

    QString out = QFileDialog::getSaveFileName(this, tr("Save parameter file"),
                                               dir, tr("Dat Files (*.dat);;CSV Files (*.csv);;Text Files (*.txt);;All Files (*.*)"),
                                               &filter);
    if (!out.isEmpty()) {
        try {
            CsvExporter exporter;
            exporter.setSeparator(' ');
            if (!exporter.exportFile(out, *mData)) {
                QMessageBox::warning(this, tr("File save failed"), tr("The file couldn't be saved."));
                return;
            }
        } catch (QException &exc) {
            QMessageBox::warning(this, tr("File save failed"), tr("The file couldn't be saved: %1").arg(exc.what()));
            return;
        }

        QFileInfo info(out);
        set.setValue("TsEditor.LastPath", info.absoluteFilePath());
        set.setValue("TsEditor.filter", filter);
    }
}

void VesselEditorWindow::on_actionGenerate_triggered()
{
    QSettings set;
    QString dir = set.value("TsEditor.LastOutPath", QDir::homePath()).toString();

    QString outdir = QFileDialog::getExistingDirectory(this, tr("Select output directory"), dir);
    if (!outdir.isEmpty()) {
        generateAll(outdir);

        set.setValue("TsEditor.LastOutPath", outdir);
    }
}

void VesselEditorWindow::on_actionRScript_location_triggered()
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

void VesselEditorWindow::on_actionGen_Script_location_triggered()
{
    QSettings set;
    QString dir = set.value("VesselEditor.script", qApp->applicationDirPath() + "/scripts/gen_vessels.R").toString();
    QFileInfo idir(dir);

    QString script = QFileDialog::getOpenFileName(this, tr("Location of R script"), idir.absolutePath(),
                                                  tr("R scripts (*.R *.r);;All files (*)"));
    if (!script.isEmpty()) {
        set.setValue("VesselEditor.script", script);
        checkEnv();
    }
}

