#include "batchcreatedialog.h"
#include "ui_batchcreatedialog.h"

BatchCreateDialog::BatchCreateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchCreateDialog)
{
    ui->setupUi(this);
}

BatchCreateDialog::~BatchCreateDialog()
{
    delete ui;
}

void BatchCreateDialog::setBaseName(QString b)
{
    ui->basename->setText(b);
}

QString BatchCreateDialog::getResultingName()
{
    return QString("%1[%2-%3]")
            .arg(ui->basename->text())
            .arg(ui->first->value())
            .arg(ui->last->value());
}

void BatchCreateDialog::on_first_valueChanged(int arg1)
{
    ui->last->setMinimum(arg1);
}
