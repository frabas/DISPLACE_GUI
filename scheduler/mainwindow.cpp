#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <utils/make_unique.h>

#include <rundialog.h>
#include <utils/MultifieldWriter.h>

#include <fstream>

#include <QTableView>
#include <QFileDialog>
#include <QSettings>

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
    RunDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        mAdapter->addSimulationRun(dlg.get());
    }
}

void MainWindow::on_action_Save_triggered()
{
    QSettings s;
    QString out = QFileDialog::getSaveFileName(this, tr("Save scheduler file"),
                                               s.value("last_scheduler").toString(),
                                               QString("Scheduler files (*.dsf);;All files (*)")
                                               );

    if (!out.isEmpty()) {
        std::ofstream of(out.toStdString(), std::ios_base::out | std::ios_base::trunc);

        if (of) {
            auto jobsAsTuple = mScheduler->getDataAsTuple();

            displace::formats::helpers::MultifieldWriter wr;
            wr.exportToStream(of, ",", jobsAsTuple.begin(), jobsAsTuple.end());
        }
    }
}
