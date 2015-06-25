#include "tseditorwindow.h"
#include "ui_tseditorwindow.h"

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

TsEditorWindow::TsEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TsEditorWindow),
    mFilename(),
    mData(),
    mModel(0),
    colVar(-1), colArea(-1), colADim(-1),
    mProcess(0),
    mDestFile("ts-XXXXXX.dat"),
    mParFile("par-XXXXXX.dat"),
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

TsEditorWindow::~TsEditorWindow()
{
    delete ui;
}

void TsEditorWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    if (mDirty) {
        int r = QMessageBox::question(this, tr("Close TimeSeriesEditor?"),
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

void TsEditorWindow::checkEnv()
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
        QMessageBox::warning(this, tr("Time Series setup check"),
                             tr("Couldn't start Rscript. Please setup the Rscript path properly in the Settings screen."));
        return;
    }

    QFileInfo file(getScriptPath());

    if (!file.exists() || !file.isReadable()) {
        QMessageBox::warning(this, tr("Time Series setup check"),
                             tr("Time Series generation script (%1) is not accessible or is missing. Please fix the path in the Settings screen.")
                             .arg(file.fileName()));
        return;
    }
}

QString TsEditorWindow::getScriptPath()
{
    QSettings set;
    return set.value("TsEditor.script", qApp->applicationDirPath() + "/scripts/gen_ts.R").toString();
}

void TsEditorWindow::on_action_Open_triggered()
{
    QSettings set;
    QString dir = set.value("TsEditor.LastPath", QDir::homePath()).toString();
    QString filter = set.value("TsEditor.filter", "").toString();

    QString file = QFileDialog::getOpenFileName(this, tr("Open CSV file"),
                                                dir, tr("Dat Files (*.dat);;CSV Files (*.csv);;Text Files (*.txt);;All Files (*.*)"),
                                                &filter);

    if (!file.isEmpty()) {
        mFilename = file;

        load(mFilename);

        QFileInfo info(file);
        set.setValue("TsEditor.LastPath", info.absoluteFilePath());
        set.setValue("TsEditor.filter", filter);
    }
}

void TsEditorWindow::load(QString filename)
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

    colVar = colADim = colArea = -1;
    QStringList h = mData->at(0);
    for (int i = 0; i < h.size(); ++i) {
        if (h[i] == "variable")
            colVar = i;
        else if (h[i] == "area")
            colArea = i;
        else if (h[i] == "a_dim")
            colADim = i;
    }

    if (colVar == -1 || colADim == -1 || colArea == -1) {
        QMessageBox::warning(this, tr("Cannot load file"), QString(tr("The header of the file is malformed.")));
        return;
    }

    mDirty = false;
    emit dataDirty();
    ui->actionGenerate->setEnabled(true);

    updateKeys();
}

void TsEditorWindow::updateKeys()
{
    QSet<QString> var, area, adim;

    for (int i = 1; i < mData->length(); ++i) { // skip header
        var.insert(mData->at(i).at(colVar));
        area.insert(mData->at(i).at(colArea));
        adim.insert(mData->at(i).at(colADim));
    }

    QStringList vl(var.toList());
    vl.sort();
    ui->varSelect->clear();
    ui->varSelect->addItems(vl);

    vl = area.toList();
    vl.sort();
    ui->areaSelect->clear();
    ui->areaSelect->addItems(vl);

    vl = adim.toList();
    vl.sort();
    ui->adimSelect->clear();
    ui->adimSelect->addItems(vl);
}

void TsEditorWindow::genSampleFile()
{
    ui->plot->clearGraphs();

    QString v = ui->varSelect->currentText();
    QString a = ui->areaSelect->currentText();
    QString n = ui->adimSelect->currentText();

    qDebug() << mDestFile.fileName() << mParFile.fileName() << v << a << n;

    if (v.isEmpty() || a.isEmpty() || n.isEmpty())
        return;

    saveTempParamFile();
    generate(mParFile.fileName(), mDestFile.fileName(), v, a, n);
}

void TsEditorWindow::loadSampleFileGraph(QString name)
{
    QFile f(name);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "***" << f.errorString();
        return;
    }

    bool ok;
    QTextStream strm(&f);
    strm.readLine();        // drop the first line
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

    ui->plot->clearGraphs();
    QCPGraph *graph = ui->plot->addGraph();
    graph->setData(x,dt);
    graph->rescaleAxes();
    ui->plot->replot();

    f.close();
}

void TsEditorWindow::saveTempParamFile()
{
    CsvExporter exporter;
    exporter.setSeparator(' ');
    exporter.exportFile(mParFile.fileName(),*mData);
}

void TsEditorWindow::generate(QString param_file, QString dest, QString variable, QString area, QString adim)
{
    QString script_file = getScriptPath();

    mProcess = new QProcess;

    connect(mProcess, SIGNAL(started()), this, SLOT(processStarted()));
    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
    connect(mProcess, SIGNAL(finished(int)), this, SLOT(processExit(int)));

    QStringList args;
    args << script_file << dest << variable << area << adim << param_file;

    displace::R::Env env;
    mProcess->setEnvironment(env.environment().toStringList());
    mProcess->setWorkingDirectory(env.getRScriptHome());
    mProcess->start(env.getRScriptExe(), args);

    qDebug() << "START:" << env.getRScriptExe() << args;
}

void TsEditorWindow::generateAll(QString outpath)
{
    mExporterWorkerDialog = new QProgressDialog(this);
    mExporterWorkerDialog->show();
    connect (this, SIGNAL(exportProgress(int)), mExporterWorkerDialog, SLOT(setValue(int)));
    connect (this, SIGNAL(exportTotalChanged(int)), mExporterWorkerDialog, SLOT(setMaximum(int)));

    mExportWorker = QtConcurrent::run(this, &TsEditorWindow::generateAllWorker, outpath);
    connect (&mExportWorkerWatcher, SIGNAL(finished()), this, SLOT(exportFinished()));

    mExportWorkerWatcher.setFuture(mExportWorker);
}

QString TsEditorWindow::generateAllWorker(QString outpath)
{
    saveTempParamFile();

    QString script_file = getScriptPath();

    int n = ui->areaSelect->count() * ui->varSelect->count() * ui->adimSelect->count();
    emit exportTotalChanged(n);

    displace::R::Env env;

    int ngood = 0;
    n=0;
    for (int var = 0; var < ui->varSelect->count(); ++var) {
        for (int area = 0; area < ui->areaSelect->count(); ++area) {
            for (int adim = 0; adim < ui->adimSelect->count(); ++adim) {
                QString variable = ui->varSelect->itemText(var);
                QString areas = ui->areaSelect->itemText(area);
                QString adims = ui->adimSelect->itemText(adim);

                QString outname = QString("%1-%2-%3.dat")
                        .arg(variable)
                        .arg(areas)
                        .arg(adims);

                QString dest = outpath + "/" + outname;

                mProcess = new QProcess;

                connect(mProcess, SIGNAL(started()), this, SLOT(processStarted()));
                connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
                connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));

                QStringList args;
                args << script_file << dest << variable << areas << adims << mParFile.fileName();

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

void TsEditorWindow::on_varSelect_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    genSampleFile();
}

void TsEditorWindow::on_areaSelect_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    genSampleFile();
}

void TsEditorWindow::on_adimSelect_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    genSampleFile();
}

void TsEditorWindow::processStarted()
{
    ui->logText->appendPlainText("\n");
    ui->logText->appendHtml(QString("<font color=\"#0000bb\">%1 %2</font>")
                            .arg(tr("Starting:"))
                            .arg(mProcess->arguments().join(" ")));
}

void TsEditorWindow::readOutput()
{
    QString t = mProcess->readAllStandardOutput();
    ui->logText->appendPlainText(t + "\n");
}

void TsEditorWindow::readError()
{
    QString t = mProcess->readAllStandardError();
    ui->logText->appendHtml("<font color=\"#aa0000\">" + t + "</font>");
}

void TsEditorWindow::processExit(int code)
{
    if (mProcess->error() != 5) {
        QMessageBox::warning(this, tr("Cannot start Rscript"),
                             tr("Cannot start Rscript: %1").arg(mProcess->errorString()));
    } else {
        if (code == 0) {
            loadSampleFileGraph(mDestFile.fileName());
        } else {
            statusBar()->showMessage(QString(tr("R Script exited with exit code %1")).arg(code), 5000);
        }
    }

    delete mProcess;
    mProcess = 0;
}

void TsEditorWindow::dataChanged(QModelIndex from, QModelIndex to, QVector<int> roles)
{
    Q_UNUSED(roles);

    mDirty = true;
    emit dataDirtyChanged();

    bool need_refresh = false;
    for (int row = from.row(); row <= to.row(); ++row) {
        // Notice: Headers are at row #0
        if (mData->at(row+1).at(colArea) == ui->areaSelect->currentText() ||
            mData->at(row+1).at(colADim) == ui->adimSelect->currentText() ||
            mData->at(row+1).at(colVar) == ui->varSelect->currentText() )
            need_refresh = true;
    }

    if (need_refresh)
        genSampleFile();
}

void TsEditorWindow::dataDirtyChanged()
{
    mDirtyIndicator->setVisible(mDirty);
    ui->action_Save->setEnabled(mDirty);
}

void TsEditorWindow::exportFinished()
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

void TsEditorWindow::on_action_Log_Window_triggered()
{
    ui->dockLogWindow->setVisible(ui->action_Log_Window->isChecked());
}

void TsEditorWindow::on_clearLog_clicked()
{
    ui->logText->clear();
}

void TsEditorWindow::on_dockLogWindow_visibilityChanged(bool visible)
{
    QSignalBlocker b(ui->action_Log_Window);
    ui->action_Log_Window->setChecked(visible);
}

void TsEditorWindow::on_actionQuit_triggered()
{
    close();
}

void TsEditorWindow::on_action_Save_triggered()
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

void TsEditorWindow::on_actionGenerate_triggered()
{
    QSettings set;
    QString dir = set.value("TsEditor.LastOutPath", QDir::homePath()).toString();

    QString outdir = QFileDialog::getExistingDirectory(this, tr("Select output directory"), dir);
    if (!outdir.isEmpty()) {
        generateAll(outdir);

        set.setValue("TsEditor.LastOutPath", outdir);
    }
}

void TsEditorWindow::on_actionRScript_location_triggered()
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

void TsEditorWindow::on_actionGen_Script_location_triggered()
{
    QSettings set;
    QString dir = set.value("TsEditor.script", qApp->applicationDirPath() + "/scripts/gen_ts.R").toString();
    QFileInfo idir(dir);

    QString script = QFileDialog::getOpenFileName(this, tr("Location of R script"), idir.absolutePath(),
                                                  tr("R scripts (*.R *.r);;All files (*)"));
    if (!script.isEmpty()) {
        set.setValue("TsEditor.script", script);
        checkEnv();
    }
}

