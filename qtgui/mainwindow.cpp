#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "displacemodel.h"
#include <mapobjectscontroller.h>
#include <objecttreemodel.h>
#include <objects/harbourentity.h>
#include <objects/nodeentity.h>
#include <objects/vesselentity.h>
#include <simulator.h>

#include <scenariodialog.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/ImageManager.h>

#include <QBoxLayout>
#include <QTextEdit>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>

const int MainWindow::maxModels = MAX_MODELS;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    models(),
    currentModel(0),
    currentModelIdx(0),
    mSimulation(0),
    mMapController(0),
    map(0),
    treemodel(0)
{
    ui->setupUi(this);

    for (int i = 0; i < maxModels; ++i)
        models[i] = 0;

    connect (this, SIGNAL(modelStateChanged()), this, SLOT(updateModelState()));

    mSimulation = new Simulator();
    connect (mSimulation, SIGNAL(log(QString)), this, SLOT(simulatorLogging(QString)));
    connect (mSimulation, SIGNAL(processStateChanged(QProcess::ProcessState)), this, SLOT(simulatorProcessStateChanged(QProcess::ProcessState)));
    connect (mSimulation, SIGNAL(simulationStepChanged(int)), this, SLOT(simulatorProcessStepChanged(int)));

    connect (mSimulation, SIGNAL(vesselMoved(int,float,float,float,float,int)),
             this, SLOT(vesselMoved(int,float,float,float,float,int)));

    simulatorProcessStateChanged(QProcess::NotRunning);

    map = new qmapcontrol::QMapControl(ui->mapWidget);
    mMapController = new MapObjectsController(map);

    QPixmap pixmap;
    pixmap.fill( Qt::white );
    qmapcontrol::ImageManager::get().setLoadingPixmap(pixmap);

    QHBoxLayout *layout = new QHBoxLayout;
    ui->mapWidget->setLayout(layout);
    layout->addWidget(map);

    ui->mapWidget->setWidget(map);

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

        mMapController->updateMapObjects(m);
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

    bool e = (currentModelIdx == 0);
    ui->play_bk->setEnabled(e);
    ui->play_fbk->setEnabled(e);
    ui->play_ffwd->setEnabled(e);
    ui->play_first->setEnabled(e);
    ui->play_fwd->setEnabled(e);
    ui->play_last->setEnabled(e);
}

void MainWindow::simulatorLogging(QString msg)
{
    ui->console->appendPlainText(msg);
//    ui->console->appendPlainText("\n");
}

void MainWindow::simulatorProcessStateChanged(QProcess::ProcessState state)
{
    if (models[0] != 0) {
        ui->cmdStart->setEnabled(state == QProcess::NotRunning);
        ui->cmdPause->setEnabled(false);
        ui->cmdStop->setEnabled(state == QProcess::Running);

        if (state != QProcess::Running)
            simulatorProcessStepChanged(-1);
    } else {
        ui->cmdStart->setEnabled(false);
        ui->cmdPause->setEnabled(false);
        ui->cmdStop->setEnabled(false);
        simulatorProcessStepChanged(-1);
    }
}

void MainWindow::simulatorProcessStepChanged(int step)
{
    if (step != -1) {
        ui->info_simstep->setText(QString(tr("Simulation step: %1")).arg(step));
    } else {
        ui->info_simstep->setText(QString(tr("Simulation step:")));
    }
}

void MainWindow::vesselMoved(int idx, float x, float y, float course, float fuel, int state)
{
    models[0]->updateVessel (idx, x, y, course, fuel, state);
    mMapController->updateVesselPosition(idx);
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

    int sel = -1;
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

void MainWindow::centerMap(const qmapcontrol::PointWorldCoord &pt)
{
    map->setMapFocusPointAnimated(pt, 5, std::chrono::milliseconds(100));
}

void MainWindow::centerMapOnHarbourId(int id)
{
    Harbour *h = currentModel->getHarboursList()[id];
    centerMap(qmapcontrol::PointWorldCoord(h->get_x(), h->get_y()));
}

void MainWindow::centerMapOnNodeId(int id)
{
    Node *h = currentModel->getNodesList()[id];
    centerMap(qmapcontrol::PointWorldCoord(h->get_x(), h->get_y()));
}

void MainWindow::centerMapOnVesselId(int id)
{
    Vessel *h = currentModel->getVesselList()[id];
    centerMap(qmapcontrol::PointWorldCoord(h->get_x(), h->get_y()));
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

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    if (!treemodel->isObject(index))
        return;

    ObjectTreeModel::Category cat = treemodel->getCategory(index);
    switch (cat) {

    case ObjectTreeModel::Harbours:
        centerMapOnHarbourId((reinterpret_cast<objecttree::HarbourEntity *>(treemodel->entity(index)))->getHarbourId());
        break;

    case ObjectTreeModel::Nodes:
        centerMapOnNodeId((reinterpret_cast<objecttree::NodeEntity *>(treemodel->entity(index)))->getNodeId());
        break;

    case ObjectTreeModel::Vessels:
        centerMapOnVesselId((reinterpret_cast<objecttree::VesselEntity *>(treemodel->entity(index)))->getVesselId());
        break;

    default:    // nothing to do
        return;
    }
}

void MainWindow::on_saveConsoleButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save Console output"),
                                                QString(), QString(tr("Text files (*.txt);;All files (*.*)")));

    if (!path.isEmpty()) {
        QFile file (path);
        if (!file.open(QFile::WriteOnly)) {
            QMessageBox::warning(this, tr("Save failed"),
                                 QString(tr("Cannot save file: %1")).arg(file.errorString()));
            return;
        }

        QTextStream strm(&file);
        strm << ui->console->toPlainText();
        file.close();
    }
}
