#include "rconsole.h"
#include "ui_rconsole.h"

#include <QFileDialog>
#include <QProcess>

RConsole::RConsole(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RConsole)
{
    ui->setupUi(this);

    ui->result->setReadOnly(true);
}

RConsole::~RConsole()
{
    delete ui;
}

void RConsole::on_execute_clicked()
{
    try {
        QString result = "no R interfacing implemented yet.";
        ui->result->appendPlainText(result);
    } catch (std::exception &exc) {
        ui->result->appendPlainText("** ERROR: " + QString::fromStdString(exc.what()));
    }
}

void RConsole::on_action_Close_triggered()
{
    close();
}

void RConsole::on_actionExecute_triggered()
{
    QString obj = QFileDialog::getOpenFileName(this, tr("Open R script"), QString(), tr("R scripts (*.R);;All files (*)"));
    if (!obj.isEmpty()) {
        QFileInfo info(obj);

        mProcess = new QProcess;
        mProcess->setWorkingDirectory(info.absolutePath());
        connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
        connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
        QStringList args;
        args << obj;

        mProcess->start("Rscript", args);
    }
}

void RConsole::readOutput()
{
    QByteArray out = mProcess->readAllStandardOutput();

    QString txt = QString::fromLatin1(out);
    ui->result->appendPlainText(txt);
}

void RConsole::readError()
{
    QByteArray out = mProcess->readAllStandardError();

    QString txt = QString::fromLatin1(out);
    ui->result->appendHtml("<font color=\"#aa0000\">" + txt + "</font>");
}
