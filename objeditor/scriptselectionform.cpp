#include "scriptselectionform.h"
#include "ui_scriptselectionform.h"

#include <R/defaults.h>
#include <R/settings.h>
#include <QFileDialog>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>

using namespace displace;

ScriptSelectionForm::ScriptSelectionForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptSelectionForm)
{
    ui->setupUi(this);

    ui->container->setLayout(mGrid = new QGridLayout);

    ui->scriptsPath->setText(R::Settings().getScriptBasePath());

    addDelegate(tr("Generate Vessels Config Files"), R::Settings::Scripts::GenerateVesselsConfigFiles);
    addDelegate(tr("Run Vessels config files"), R::Settings::Scripts::RunVesselsConfigFiles);
    addDelegate(tr("Generate Metiers Files"), R::Settings::Scripts::GenerateMetiersVariousFiles);
    addDelegate(tr("Generate Metiers Selectivity Per Stock files"), R::Settings::Scripts::GenerateMetiersSelectivityPerStockFiles);
}

ScriptSelectionForm::~ScriptSelectionForm()
{
    delete ui;
}

void ScriptSelectionForm::on_reset_clicked()
{
    ui->scriptsPath->setText(R::defaults::getRScriptsPath());

    for(auto w : ui->container->children()) {
        auto ed = dynamic_cast<QLineEdit *>(w);
        if (ed) {
            ed->setText(R::defaults::getScriptFileName(ed->property("key").toString()));
        }
    }

}

void ScriptSelectionForm::on_ScriptSelectionForm_accepted()
{
    R::Settings s;

    s.setScriptBasePath(ui->scriptsPath->text());

    for(auto w : ui->container->children()) {
        auto ed = dynamic_cast<QLineEdit *>(w);
        if (ed) {
            s.setScriptPath(ed->property("key").toString(), ed->text());
        }
    }
}

void ScriptSelectionForm::on_browseBasePath_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Select Base Scripts Path"), ui->scriptsPath->text());
    if (!file.isEmpty()) {
        ui->scriptsPath->setText(file);
    }

}

void ScriptSelectionForm::addDelegate(const QString &label, const QString &key)
{
    int row = mGrid->rowCount();

    QLabel *lab = new QLabel(label);
    mGrid->addWidget(lab, row, 0);

    QLineEdit *ed = new QLineEdit();
    ed->setText(R::Settings().getScriptPath(key));
    ed->setProperty("key", key);
    mGrid->addWidget(ed, row, 1);

    QToolButton *tb = new QToolButton();
    tb->setIcon(QIcon(":/icons/remove.png"));
    connect (tb, &QToolButton::clicked, [this, ed, key]() {
        ed->setText(ui->scriptsPath->text() + "/" + key);
    });
    mGrid->addWidget(tb, row, 2);

    QPushButton *pb = new QPushButton(tr("Browse..."));
    connect (pb, &QPushButton::clicked, [this, ed, key]() {
        QString file = QFileDialog::getOpenFileName(this, QString(tr("Select %1 script")).arg(key),
                                                    ed->text(), tr("R Scripts (*.R);;All Files (*)"));
        if (!file.isEmpty()) {
            ed->setText(file);
        }
    });
    mGrid->addWidget(pb, row, 3);
}

