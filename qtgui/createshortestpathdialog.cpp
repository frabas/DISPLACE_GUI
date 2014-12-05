#include "createshortestpathdialog.h"
#include "ui_createshortestpathdialog.h"

#include <QFileDialog>
#include <QSettings>

CreateShortestPathDialog::CreateShortestPathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateShortestPathDialog)
{
    ui->setupUi(this);
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
