#include "createshortestpathdialog.h"
#include "ui_createshortestpathdialog.h"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

CreateShortestPathDialog::CreateShortestPathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateShortestPathDialog)
{
    ui->setupUi(this);

    ui->graphName->setValidator(new QIntValidator);
    on_graphName_textChanged("");
    on_checkAllRelevantNodes_toggled(false);
}

CreateShortestPathDialog::~CreateShortestPathDialog()
{
    delete ui;
}

void CreateShortestPathDialog::setShortestPathFolder(QString path)
{
    ui->shortestFolder->setText(path);
}

QString CreateShortestPathDialog::getShortestPathFolder() const
{
    return ui->shortestFolder->text();
}

void CreateShortestPathDialog::setRelevantNodesFolder(QString path)
{
    ui->relevantFolder->setText(path);
}

QString CreateShortestPathDialog::getRelevantNodesFolder() const
{
    return ui->relevantFolder->text();
}

bool CreateShortestPathDialog::isAllNodesAreRelevantChecked() const
{
    return ui->checkAllRelevantNodes->isChecked();
}

QString CreateShortestPathDialog::getOutputFolder() const
{
    return ui->outFolder->text();
}

void CreateShortestPathDialog::setOutputFolder(const QString &folder)
{
    ui->outFolder->setText(folder);
}

QString CreateShortestPathDialog::getGraphName() const
{
    return ui->graphName->text();
}

void CreateShortestPathDialog::setGraphName(const QString &name)
{
    ui->graphName->setText(name);
}

void CreateShortestPathDialog::on_browseShortestFolder_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("last_spath", QDir::homePath()).toString();

    QString path = QFileDialog::getExistingDirectory(this, tr("Select shortest path output folder") , lastpath );
    if (!path.isEmpty()) {
        setShortestPathFolder(path);
        sets.setValue("last_spath", path);
    }
}

void CreateShortestPathDialog::on_browseRelevantFolder_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("last_relpath", QDir::homePath()).toString();

    QString path = QFileDialog::getOpenFileName(this, tr("Select Relevant nodes input file") , lastpath, tr("*.dat"));
    if (!path.isEmpty()) {
        setRelevantNodesFolder(path);
        sets.setValue("last_relpath", path);
    }
}

void CreateShortestPathDialog::on_ok_clicked()
{
    if (!isAllNodesAreRelevantChecked() && getRelevantNodesFolder().isEmpty()) {
        QMessageBox::warning(this, tr("No nodes specified"),
                             tr("No nodes are selected. Either tick 'All Nodes are relevant' or select the relevant nodes path"));
        return;
    }


    if (!isAllNodesAreRelevantChecked()) {
        QString refpath = ui->relevantFolder->text();
        QRegExp regexp("(.*)/vesselsspe_([^/_]+)_([^/]+).dat");

        if (regexp.indexIn(refpath) == -1) {
            QMessageBox::warning(this, tr("Relevant Nodes template check failed"),
                                 tr("The relevant nodes file name should fit the template: vesselsspe_XXX_YYY.dat.\n"
                                    "Please select a compliant file instead."));
            return;

        }

    }

    accept();
}

void CreateShortestPathDialog::on_browseOutFolder_clicked()
{
    QSettings sets;
    QString lastpath = sets.value("last_spath", QDir::homePath()).toString();

    QString path = QFileDialog::getExistingDirectory(this, tr("Select Graph output folder") , lastpath );
    if (!path.isEmpty()) {
        setOutputFolder(path);
        if (ui->shortestFolder->text().isEmpty())
            setShortestPathFolder(path);
        sets.setValue("last_spath", path);
    }
}

void CreateShortestPathDialog::on_graphName_textChanged(const QString &)
{
    ui->ok->setEnabled(!ui->graphName->text().isEmpty());
}

void CreateShortestPathDialog::on_checkAllRelevantNodes_toggled(bool)
{
    bool en = !ui->checkAllRelevantNodes->isChecked();
    ui->relevantFolder->setEnabled(en);
    ui->browseRelevantFolder->setEnabled(en);
}
