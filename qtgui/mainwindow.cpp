#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "displacemodel.h"

#include <QMapControl/QMapControl.h>
#include <QMapControl/Layer.h>
#include <QMapControl/MapAdapterOSM.h>
#include <objecttreemodel.h>
//#include <QMapControl/MapAdapterOpenSea.h>
#include <simulator.h>
#include <QMapControl/LayerGeometry.h>
#include <QMapControl/GeometryPointCircle.h>
#include <QMapControl/LayerMapAdapter.h>

#include <scenariodialog.h>

#include <QBoxLayout>
#include <QTextEdit>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

const int MainWindow::maxModels = MAX_MODELS;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    models(),
    currentModel(0),
    currentModelIdx(0),
    mSimulation(0),
    map(0),
    mapadapter(0),
    mainlayer(0),
    treemodel(0)
{
    ui->setupUi(this);

    for (int i = 0; i < maxModels; ++i)
        models[i] = 0;

    connect (this, SIGNAL(modelStateChanged()), this, SLOT(updateModelState()));

    mSimulation = new Simulator();
    connect (mSimulation, SIGNAL(log(QString)), this, SLOT(simulatorLogging(QString)));
    connect (mSimulation, SIGNAL(processStateChanged(QProcess::ProcessState)), this, SLOT(simulatorProcessStateChanged(QProcess::ProcessState)));
    simulatorProcessStateChanged(QProcess::NotRunning);

   // map = std::shared_ptr<qmapcontrol::QMapControl>(new qmapcontrol::QMapControl(ui->mapWidget));
    map = new qmapcontrol::QMapControl(ui->mapWidget);

    QPixmap pixmap;
    pixmap.fill( Qt::white );
//    map->setLoadingPixmap(pixmap);

    QHBoxLayout *layout = new QHBoxLayout;
    ui->mapWidget->setLayout(layout);
    layout->addWidget(map);

    ui->mapWidget->setWidget(map);

//    map->showScale(false);

    //seamarkadapter = new qmapcontrol::OpenSeaMapAdapter();

    // create mapadapter, for mainlayer and overlay
    mapadapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOSM());

    // create a layer with the mapadapter and type MapLayer
    mainlayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("OpenStreetMap", mapadapter));
    //seamarklayer = new qmapcontrol::MapLayer("Seamark", seamarkadapter);
    entitylayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Entities"));

    // add Layer to the MapControl
    map->addLayer(mainlayer);
   // map->addLayer(seamarklayer);
    map->addLayer(entitylayer);

   // map->setView(QPointF(11.54105,54.49299));
    map->setZoom(10);

    /* Tree model setup */
    treemodel = new ObjectTreeModel(map);
    ui->treeView->setModel(treemodel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Load_triggered()
{
    QSettings sets;
    QString lastpath;

    lastpath = sets.value("lastpath", QDir::homePath()).toString();
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Model directory"),
                                                    lastpath);

    if (dir.isEmpty())
        return;

    QDir d (dir);
    QStringList parts = d.dirName().split("_");
    d.cdUp();

    if (parts.count() < 2) {
        // fails.
        QMessageBox::warning(this, tr("Name not valid"),
                             tr("The selected directory doesn't seems to be a valid Model directory."));
        return;
    }

    int res = QMessageBox::question(this, tr("Confirm loading model"),
                                    QString(tr("Do you want to load the model named %1 ?")).arg(parts.at(1)),
                                    QMessageBox::Yes, QMessageBox::No);
    if (res == QMessageBox::Yes) {
        sets.setValue("lastpath", d.absolutePath());

        // load the model named x

        DisplaceModel *m = new DisplaceModel();

        if (!m->load(d.absolutePath(), parts.at(1), "baseline")) {
            QMessageBox::warning(this, tr("Load failed."),
                                 QString(tr("Error loading model %1: %2")).arg(parts.at(1)).arg(m->getLastError()));
            return;
        }
        models[0] = m;

        emit modelStateChanged();
    }
}

void MainWindow::on_modelSelector_currentIndexChanged(int index)
{
    currentModelIdx = ui->modelSelector->itemData(index).toInt();
    if (currentModelIdx >= 0)
        currentModel = models[currentModelIdx];
    else
        currentModel = 0;
    treemodel->setCurrentModel(currentModel);
}

void MainWindow::simulatorLogging(QString msg)
{
    ui->console->appendPlainText(msg);
    ui->console->appendPlainText("\n");
}

void MainWindow::simulatorProcessStateChanged(QProcess::ProcessState state)
{
    if (models[0] != 0) {
        ui->cmdStart->setEnabled(state == QProcess::NotRunning);
        ui->cmdPause->setEnabled(false);
        ui->cmdStop->setEnabled(state == QProcess::Running);
    } else {
        ui->cmdStart->setEnabled(false);
        ui->cmdPause->setEnabled(false);
        ui->cmdStop->setEnabled(false);
    }
}

void MainWindow::updateModelState()
{
    simulatorProcessStateChanged(mSimulation->processState());
    updateModelList();
}

void MainWindow::updateModelList()
{
    int n = ui->modelSelector->currentData().toInt();
    ui->modelSelector->clear();

    int sel;
    for (int i = 0; i < MAX_MODELS; ++i) {
        if (models[i] != 0) {
            ui->modelSelector->addItem(
                        QString(tr("[%1] %2")).arg(i).arg(models[i]->name()),
                        i);
            if (i == n)
                sel = i;
        }
    }

    ui->modelSelector->setCurrentIndex(sel);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (mSimulation->isRunning()) {
        int res = QMessageBox::question(this, tr("Simulation running"),
                              tr("A simulation process is running. Closing the app will force close it. Do you want to proceed?"),
                              QMessageBox::No, QMessageBox::Yes);

        if (res == QMessageBox::Yes) {
            mSimulation->forceStop();
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void MainWindow::on_cmdStart_clicked()
{
    if (!mSimulation->isRunning() && models[0] != 0) {
        mSimulation->start(models[0]->name(), models[0]->basepath());
    }
}

void MainWindow::on_cmdStop_clicked()
{
    if (mSimulation && mSimulation->isRunning()) {
        mSimulation->forceStop();
    }
}

void MainWindow::on_actionScenario_triggered()
{
    if (currentModel) {
        Scenario d = currentModel->scenario();
        ScenarioDialog dlg (d, this);
        if (dlg.exec() == QDialog::Accepted) {
            currentModel->setScenario(dlg.getScenario());
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (models[0] && models[0]->save()) {
        QMessageBox::information(this, tr("Model saved"),
                                 QString(tr("The model %1 has been saved successfully.")).arg(models[0]->name()));
        return;
    } else {
        QMessageBox::warning(this, tr("Load failed"),
                             tr("There was an error saving the model."));
        return;
    }
}
