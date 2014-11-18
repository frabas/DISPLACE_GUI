#include "waitdialog.h"
#include "ui_waitdialog.h"

WaitDialog::WaitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitDialog)
{
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
    ui->setupUi(this);

    setFixedSize(size());   /* Set a fixed size, unresizeable window */
    ui->progress->setVisible(false);
    ui->progress->setValue(0);
}

WaitDialog::~WaitDialog()
{
    delete ui;
}

void WaitDialog::setText(QString text)
{
    ui->text->setText(text);
}

void WaitDialog::setProgress(bool shown, int max)
{
    ui->progress->setVisible(shown);
    ui->progress->setMinimum(0);
    ui->progress->setMaximum(max);
}

void WaitDialog::setProgression(int level)
{
    ui->progress->setValue(level);
}
