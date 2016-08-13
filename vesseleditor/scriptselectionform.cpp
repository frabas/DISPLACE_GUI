#include "scriptselectionform.h"
#include "ui_scriptselectionform.h"

#include <defaults.h>
#include <settings.h>
#include <QFileDialog>

ScriptSelectionForm::ScriptSelectionForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptSelectionForm)
{
    ui->setupUi(this);

    displace::vesselsEditor::Settings s;
    ui->configPath->setText(s.getConfigScriptPath());
    ui->runPath->setText(s.getRunScriptPath());
}

ScriptSelectionForm::~ScriptSelectionForm()
{
    delete ui;
}

void ScriptSelectionForm::on_reset_clicked()
{
    ui->configPath->setText(displace::vesselsEditor::getDefaultConfigScriptPath());
    ui->runPath->setText(displace::vesselsEditor::getDefaultRunScriptPath());
}

void ScriptSelectionForm::on_ScriptSelectionForm_accepted()
{
    displace::vesselsEditor::Settings s;

    s.setConfigScriptPath(ui->configPath->text());
    s.setRunScriptPath(ui->runPath->text());
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
