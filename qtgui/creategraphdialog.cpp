#include "creategraphdialog.h"
#include "ui_creategraphdialog.h"

CreateGraphDialog::CreateGraphDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateGraphDialog)
{
    ui->setupUi(this);
}

CreateGraphDialog::~CreateGraphDialog()
{
    delete ui;
}
