#include "tseditorwindow.h"
#include "ui_tseditorwindow.h"

#include <csv/csvimporter.h>
#include <csv/csvtablemodel.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

TsEditorWindow::TsEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TsEditorWindow),
    mDestFile("ts")
{
    ui->setupUi(this);
}

TsEditorWindow::~TsEditorWindow()
{
    delete ui;
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
        set.setValue("CTsEditor.filter", filter);
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

    mModel = new CsvTableModel(mData);
    mModel->setFirstLineHeaders(true);
    ui->table->setModel(mModel);

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
    QString v = ui->varSelect->currentText();
    QString a = ui->areaSelect->currentText();
    QString n = ui->adimSelect->currentText();

    qDebug() << mDestFile << v << a << n;

    if (v.isEmpty() || a.isEmpty() || n.isEmpty())
        return;

    generate(mDestFile, v, a, n);
}

void TsEditorWindow::generate(QString dest, QString variable, QString area, QString adim)
{
    QSettings set;
    QString param_file = set.value("TsEditor.paramfile", qApp->applicationDirPath() + "/data/param_timeseries.dat").toString();
    QString script_file = set.value("TsEditor.script", qApp->applicationDirPath() + "/scripts/gen_ts.R").toString();

    mProcess = new QProcess;

    //mProcess->setWorkingDirectory(info.absolutePath());
    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
    connect(mProcess, SIGNAL(finished(int)), this, SLOT(processExit(int)));

    QStringList args;
    args << script_file << dest << variable << area << adim << param_file;

    mProcess->start("Rscript", args);

}

void TsEditorWindow::on_varSelect_currentIndexChanged(const QString &arg1)
{
    genSampleFile();
}

void TsEditorWindow::on_areaSelect_activated(const QString &arg1)
{
    genSampleFile();
}

void TsEditorWindow::on_adimSelect_activated(const QString &arg1)
{
    genSampleFile();
}

void TsEditorWindow::readOutput()
{
    QString t = mProcess->readAllStandardOutput();
    qDebug() << t;
}

void TsEditorWindow::readError()
{
    QString t = mProcess->readAllStandardError();
    qDebug() << "*** " << t;
}

void TsEditorWindow::processExit(int code)
{
    if (code == 0) {
        // TODO update the graph
    } else {
        statusBar()->showMessage(QString(tr("R Script exited with exit code %1")).arg(code), 5000);
    }
}
