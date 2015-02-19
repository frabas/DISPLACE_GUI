#include "waitdialog.h"
#include "ui_waitdialog.h"

#include <QDebug>
#include <QApplication>
#include <QThread>

WaitDialog::WaitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitDialog),
    mAbortListener(0)
{
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
    ui->setupUi(this);

    setFixedSize(size());   /* Set a fixed size, unresizeable window */
    ui->progress->setVisible(false);
    ui->progress->setValue(0);
    ui->cmdAbort->setVisible(false);

    connect (this, SIGNAL(self_set_progression(int)), this, SLOT(setProgression(int)), Qt::QueuedConnection);
}

WaitDialog::~WaitDialog()
{
    delete ui;
}

void WaitDialog::setFormat(QString format)
{
    ui->progress->setFormat(format);
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

void WaitDialog::setProgress(int pos, int max)
{
    ui->progress->setVisible(true);
    ui->progress->setMinimum(pos);
    ui->progress->setMaximum(max);
}

void WaitDialog::enableAbort(bool enable)
{
    if (enable)
        ui->cmdAbort->show();
    else
        ui->cmdAbort->hide();
}

void WaitDialog::setProgression(int level)
{
    if (QApplication::instance()->thread() != QThread::currentThread())
        emit self_set_progression(level);
    else
        ui->progress->setValue(level);
}

void WaitDialog::on_cmdAbort_clicked()
{
    qDebug() << "ABORT";
    if (mAbortListener)
        mAbortListener->abortIssued();
    emit aborted();
}
