#include "scriptselectionform.h"
#include "ui_scriptselectionform.h"

#include <R/defaults.h>
#include <R/settings.h>
#include <QFileDialog>

using namespace displace;

ScriptSelectionForm::ScriptSelectionForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptSelectionForm)
{
    ui->setupUi(this);

    R::Settings s;
    ui->configPath->setText(s.getScriptPath(R::Settings::Scripts::GenerateVesselsConfigFiles));
    ui->runPath->setText(s.getScriptPath(R::Settings::Scripts::RunVesselsConfigFiles));
}

ScriptSelectionForm::~ScriptSelectionForm()
{
    delete ui;
}

void ScriptSelectionForm::on_reset_clicked()
{
    ui->configPath->setText(R::defaults::getScriptFileName(R::Settings::Scripts::GenerateVesselsConfigFiles));
    ui->runPath->setText(R::defaults::getScriptFileName(R::Settings::Scripts::RunVesselsConfigFiles));
}

void ScriptSelectionForm::on_ScriptSelectionForm_accepted()
{
    R::Settings s;

    s.setScriptPath(R::Settings::Scripts::GenerateVesselsConfigFiles, ui->configPath->text());
    s.setScriptPath(R::Settings::Scripts::RunVesselsConfigFiles, ui->runPath->text());
}

void ScriptSelectionForm::on_browseConfig_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select Config Script"), ui->configPath->text(), tr("R Scripts (*.R);;All Files (*)"));
    if (!file.isEmpty()) {
        ui->configPath->setText(file);
    }
}

void ScriptSelectionForm::on_browseRun_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select Run Script"), ui->runPath->text(), tr("R Scripts (*.R);;All Files (*)"));
    if (!file.isEmpty()) {
        ui->runPath->setText(file);
    }
}
