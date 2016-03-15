#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <utils/make_unique.h>

#include <QTableView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mScheduler = utils::make_unique<SchedulerJob>();
    ui->dataView->setModel(mAdapter = new SchedulerJobAdapter(*mScheduler, this));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Add_triggered()
{
    mAdapter->insertRow(0);
}
