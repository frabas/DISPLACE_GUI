#include "rconsole.h"
#include "ui_rconsole.h"

#include <RInside.h>
#include <Rcpp.h>
#include <R.h>

RConsole::RConsole(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RConsole),
    mR(new RInside(0,0,false,false,false))
{
    ui->setupUi(this);

    ui->result->setReadOnly(true);
}

RConsole::~RConsole()
{
    delete ui;
    delete mR;
}

void RConsole::on_execute_clicked()
{
    try {
        SEXP r;
        std::string line = ui->command->text().toStdString();
        int err;
        if (!(err = mR->parseEval(line, r))) {
            QString result = QString::fromStdString(::Rcpp::as<std::string>(r));
            ui->result->appendPlainText(result);

            ui->command->clear();
        } else {
            ui->result->appendPlainText(QString("**Error %1\n").arg(err));
        }
    } catch (std::exception &exc) {
        ui->result->appendPlainText("** ERROR: " + QString::fromStdString(exc.what()));
    }
}

void RConsole::on_action_Close_triggered()
{
    close();
}
