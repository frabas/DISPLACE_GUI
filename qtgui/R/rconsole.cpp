#include "rconsole.h"
#include "ui_rconsole.h"

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
