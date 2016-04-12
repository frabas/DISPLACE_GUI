#include "rundialog.h"
#include "ui_rundialog.h"

#include <batchcreatedialog.h>

#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExp>

RunDialog::RunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunDialog)
{
    ui->setupUi(this);
}

RunDialog::~RunDialog()
{
    delete ui;
}

void RunDialog::on_cmdBrowseModel_clicked()
{
    QSettings sets;
    QString lastpath;

    lastpath = sets.value("lastpath", QDir::homePath()).toString();

    QString dir  = QFileDialog::getOpenFileName(this,
                                                tr("Select a scenario file - look at simusspe_* directory"),
                                                lastpath, QString("*.dat"));
    if (dir.isEmpty())
        return;

    QDir d (dir);
    sets.setValue("lastpath", d.absolutePath());

    QString base, in, out;

    if (parsePathParts(dir, base, in, out)) {
        mData.setPath(dir);
        mData.setSimulationName(in);
        mData.setSimulationOutputName(out);
        mData.setName(base);

        ui->path->setText(base);
        ui->modName->setText(in);
        ui->outName->setText(out);
    } else {
        QMessageBox::warning(this, tr("Model path error"),
                             tr("Cannot parse the model pathname correctly. Check path name and model structure."));
    }
}

bool RunDialog::parsePathParts(const QString &path, QString &basepath, QString &inputname, QString &outputname)
{
    QRegExp regexp("(.*)/simusspe_([^/]+)/([^/]+).dat");

    if (regexp.indexIn(path) == -1) {
        return false;
    }

    basepath = regexp.cap(1);
    inputname = regexp.cap(2);
    outputname = regexp.cap(3);

    return true;
}

void RunDialog::on_cmdBatchCreate_clicked()
{
    BatchCreateDialog dlg;
    dlg.setBaseName(ui->simName->text());

    if (dlg.exec() == QDialog::Accepted) {
        ui->simName->setText(dlg.getResultingName());
    }
}

void RunDialog::on_RunDialog_accepted()
{
    mData.setPath(ui->path->text());
    mData.setName(ui->modName->text());
    mData.setSimulationOutputName(ui->outName->text());
    mData.setSimulationName(ui->simName->text());
    mData.setNumThreads(ui->threads->value());
    mData.setSimulationSteps(ui->simSteps->value());
}
