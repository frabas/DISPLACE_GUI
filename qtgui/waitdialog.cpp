#include "waitdialog.h"
#include "ui_waitdialog.h"

WaitDialog::WaitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitDialog)
{
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
    ui->setupUi(this);

    setFixedSize(size());   /* Set a fixed size, unresizeable window */
}

WaitDialog::~WaitDialog()
{
    delete ui;
}
