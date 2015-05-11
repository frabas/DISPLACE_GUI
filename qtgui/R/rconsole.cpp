#include "rconsole.h"
#include "ui_rconsole.h"

#include <RInside.h>

RConsole::RConsole(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RConsole),
    mR(new RInside)
{
    ui->setupUi(this);
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
        mR->parseEval(line, r);

        QString result = QString::fromStdString(::Rcpp::as<std::string>(r));
        ui->result->appendPlainText(result);
    } catch (std::exception &exc) {
        ui->result->appendPlainText("** ERROR: " + QString::fromStdString(exc.what()));
    }
}
