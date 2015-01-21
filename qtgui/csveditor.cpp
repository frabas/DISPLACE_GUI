#include "csveditor.h"
#include "ui_csveditor.h"

#include <utils/csvimporter.h>

#include <QFuture>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

CsvEditor::CsvEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CsvEditor),
    mModel(0),
    mData()
{
    ui->setupUi(this);

    ui->toolBar->addAction(ui->menuOptions->menuAction());

    mModel = new CsvTableModel(0, this);
    ui->table->setModel(mModel);

    QSettings set;
    restoreGeometry(set.value("CsvEditor.mainGeometry").toByteArray());
    restoreState(set.value("CsvEditor.mainState").toByteArray());

    updateCheckState(set.value("CsvEditor.headersShown", false).toBool());
}

CsvEditor::~CsvEditor()
{
    delete ui;
}

void CsvEditor::on_action_Open_triggered()
{
    QSettings set;
    QString dir = set.value("CsvEditor.LastPath", QDir::homePath()).toString();
    QString filter = set.value("CsvEditor.filter", "").toString();

    QString file = QFileDialog::getOpenFileName(this, tr("Open CSV file"),
                                                dir, tr("CSV Files (*.csv);;Text Files (*.txt);;Dat Files (*.dat);;All Files (*.*)"),
                                                &filter);

    if (!file.isEmpty()) {
        CsvImporter importer;

        try {
            QList<QStringList> res = importer.import(file);

            mData = std::shared_ptr<QList<QStringList>>( new QList<QStringList>(res));
            mModel->setSource(mData);

            QFileInfo info(file);
            set.setValue("CsvEditor.LastPath", info.absoluteFilePath());
            set.setValue("CsvEditor.filter", filter);

            updateCheckState(set.value("CsvEditor.headersShown", false).toBool());

            setWindowTitle(QString(tr("CsvEditor - %1 in %2", "1: filename 2: path"))
                           .arg(info.fileName())
                           .arg(info.path()));
        } catch (CsvImporter::Exception &x) {
            QMessageBox::warning(this, tr("Cannot load csv file"), x.what());
        }
    }
}

void CsvEditor::on_actionFirst_line_as_Headers_toggled(bool checked)
{
    if (mModel) {
        mModel->setFirstLineHeaders(checked);
    }

    QSettings set;
    if (set.value("CsvEditor.headersShown", false).toBool() != checked)
        set.setValue("CsvEditor.headersShown", checked);
}

void CsvEditor::closeEvent(QCloseEvent *)
{
    QSettings sets;
    sets.setValue("CsvEditor.mainGeometry", saveGeometry());
    sets.setValue("CsvEditor.mainState", saveState());
}

void CsvEditor::updateCheckState(bool state)
{
    ui->actionFirst_line_as_Headers->setChecked(state);
}
